#include <timer.h>
#include <unistd.h>
#include <plic.h>
#include <sysctl.h>
#include <my_timer.h>

volatile uint32_t my_timer::irq_count;

my_timer::my_timer()
{
    // the default device and channel. This is overwritten by the function 'set_timer_device_channel_dt'
    timer_device = (timer_device_number_t)2;
    timer_channel = (timer_channel_number_t)2;
    // default time step/resolution is 1000 ns, i.e. 1 microsecond
    time_step = 1000;
    // start the timer at 0
    irq_count = 0;
}

void my_timer::set_timer_device_channel_dt(int device, int channel, int _time_step)
{
    // check of the input is valid, then set the variables
    if ((device >= TIMER_DEVICE_0) and (device < TIMER_DEVICE_MAX))
    {
        timer_device = (timer_device_number_t)device;
    }
    if ((channel >= TIMER_CHANNEL_0) and (channel < TIMER_CHANNEL_MAX))
    {
        timer_channel = (timer_channel_number_t)channel;
    }
    if (_time_step>0)
    {
        time_step = _time_step;
    }
    // disable the timer if it is running
    stop_my_timer();
    // start the timer
    start_my_timer();
}

void my_timer::reset()
{   // start the timer at 0
    irq_count = 0;
}

void my_timer::stop_my_timer()
{   // disable the timer
    timer_set_enable(timer_device, timer_channel, 0);
}

void my_timer::start_my_timer()
{
    // Init Platform-Level Interrupt Controller(PLIC)
    plic_init();
    // Enable global interrupt for machine mode of RISC-V
    sysctl_enable_irq();

    // initialize the timer device
    timer_init(timer_device);
    // Set timer interval to 1ms
    timer_set_interval(timer_device, timer_channel, time_step);
    // Set timer callback function (not single shot) method
    timer_irq_register(timer_device, timer_channel, 0, 1, my_timer::timer_callback, &dummy);
    // Enable timer
    timer_set_enable(timer_device, timer_channel, 1);
}

uint32_t my_timer::get_current_time()
{   // get the current clock-ticks
    return irq_count;
}

int my_timer::timer_callback(void *ctx)
{   // the call back function that actually makes the clock tick.
    irq_count++;
    return 0;
}
