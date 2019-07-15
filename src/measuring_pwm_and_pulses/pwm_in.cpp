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
volatile uint32_t PwmIn::pulsewidth;
volatile uint64_t PwmIn::zero_time;


PwmIn::PwmIn(int _physical_pin, int _gpiohs_number)
{ // the physical pin and the gpiohs to use
    physical_pin = _physical_pin;
    gpiohs_number = _gpiohs_number;

    plic_init();
    sysctl_enable_irq();
    // set the timer configuration: timer device, channel and time step (1000 = 1 microsecond)

    // connect physical pin (physical_pin) to function gpiohs_number
    fpioa_set_function(physical_pin, (fpioa_function_t)(FUNC_GPIOHS0 + gpiohs_number));
    gpiohs_set_drive_mode(gpiohs_number, GPIO_DM_INPUT_PULL_UP);
    gpiohs_set_pin_edge(gpiohs_number, GPIO_PE_BOTH);
    gpiohs_irq_register(gpiohs_number, 1, irq_gpiohs_key, &dummy);

    // set the PWM period and pulse width to 0
    period = 0;
    pulsewidth = 0;
    zero_time = 0;
}

int PwmIn::irq_gpiohs_key(void *ctx)
{ // This interrupt call back is called whenever the pin changes value
    if (gpiohs_get_pin(gpiohs_number == GPIO_PV_HIGH))
    {
        // there was a rising edge
        // recored the period (i.e. time since last rising edge)
        uint64_t current_time = sysctl_get_time_us();
        period = current_time - zero_time;
        // 'restart' the timer by saving this point in time
        zero_time = current_time;
    }
    else
    {
        // there was a falling edge
        // recored the time, this is the length of the pulse
        pulsewidth = sysctl_get_time_us() - zero_time;
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
