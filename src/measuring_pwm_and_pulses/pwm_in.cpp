#include <gpiohs.h>
#include <plic.h>
#include <sysctl.h>
#include <fpioa.h>
#include <stdio.h>

#include "pwm_in.h"

// make the variables
uint32_t dummy;
volatile int PwmIn::gpiohs_number;
volatile uint32_t PwmIn::period;
volatile my_timer PwmIn::my_time;
volatile uint32_t PwmIn::pulsewidth;

PwmIn::PwmIn(int _physical_pin, int _gpiohs_number)
{ // the physical pin and the gpiohs to use
    physical_pin = _physical_pin;
    gpiohs_number = _gpiohs_number;

    plic_init();
    sysctl_enable_irq();
    // set the timer configuration: timer device, channel and time step (1000 = 1 microsecond)
    my_time.set_timer_device_channel_dt(0, 0, 1000);

    // connect physical pin (physical_pin) to function gpiohs_number
    fpioa_set_function(physical_pin, (fpioa_function_t)(FUNC_GPIOHS0 + gpiohs_number));
    gpiohs_set_drive_mode(gpiohs_number, GPIO_DM_INPUT_PULL_UP);
    gpiohs_set_pin_edge(gpiohs_number, GPIO_PE_BOTH);
    gpiohs_irq_register(gpiohs_number, 1, irq_gpiohs_key, &dummy);

    // set the PWM period and pulse width to 0
    period = 0;
    pulsewidth = 0;
}

int PwmIn::irq_gpiohs_key(void *ctx)
{ // This interrupt call back is called whenever the pin changes value
    if (gpiohs_get_pin(gpiohs_number == GPIO_PV_HIGH))
    {
        // there was a rising edge
        // recored the period (i.e. time since last rising edge)
        period = my_time.get_current_time();
        // restart the timer
        my_time.reset();
    }
    else
    {
        // there was a falling edge
        // recored the time, this is the length of the pulse
        pulsewidth = my_time.get_current_time();
    }
    return 0;
}

uint32_t PwmIn::get_period(void) const
{
    return period;
}

uint32_t PwmIn::get_pulsewidth(void) const
{
    return pulsewidth;
}

float PwmIn::get_dutycycle(void) const
{
    return (float)pulsewidth / (float)period;
}

void PwmIn::reset_pulsewidth(void)
{
    pulsewidth = 0;
}
