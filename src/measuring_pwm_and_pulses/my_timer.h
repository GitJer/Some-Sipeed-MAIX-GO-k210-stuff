#include <timer.h>

#ifndef MY_TIMER
#define MY_TIMER

class my_timer
{
public:
    my_timer();
    // set the timer device, channel and the time step
    void set_timer_device_channel_dt(int, int, int);
    // disable the timer
    void stop_my_timer(void);
    // start the timer
    void start_my_timer(void);
    // get the current clock-ticks
    uint32_t get_current_time(void);
    // reset the current clock-ticks to 0
    void reset(void);

private:
    // the call back function to increase the clock-ticks
    static int timer_callback(void *ctx);

    // the timer device and channel
    timer_device_number_t timer_device = (timer_device_number_t)2;
    timer_channel_number_t timer_channel = (timer_channel_number_t)2;

    // what it says: a dummy variable, needed because of how interrupts are defined in the SDK
    uint32_t dummy;
    // the clock-ticks
    volatile static uint32_t irq_count;
    // the unit/resolution of time measurements
    // The resolution is specified in nano seconds 
    // Note: with the default clock settings the value of time_step has to be about 700 or more. 
    // The most natural value would be 1000, i.e. 1 microsecond).
    // Thus, the time in nanoseconds is irq_count * time_step 
    size_t time_step; 
};

#endif