/*
This is a class for running a stepper motor with a DRV8825 driver.
Driver: DRV8825
Stepper motor: Nema17 Stepper 42BYGH 17HS4401S 4-lead

The DRV8825 has:
- a step pin that triggers a single step. Only the up edge matters, the pulse width doesn't matter
- a direction pin that changes the direction of the stepper motor
- three pins that together determine the step size (from full step to 1/32th step)

Given a RPM, a PWM signal is used to generate pulses that cause steps.

This code uses: PWM device 0 and channel 0
This code uses: FUNC_TIMER0_TOGGLE1 assigned to the step_pin (which outputs the PWM signal)

*/

#include <gpiohs.h>
#include <fpioa.h>
#include <pwm.h>
#include <unistd.h>
#include "stepper.h"
// #include <stdio.h>

// constructor
DRV8825::DRV8825(int _step_pin, int _step_gpio,
                 int _direction_pin, int _direction_gpio,
                 int _m1_pin, int _m1_gpio,
                 int _m2_pin, int _m2_gpio,
                 int _m3_pin, int _m3_gpio)
{
    init_pin(_step_pin, _step_gpio); // NOTE: the allocation of this gpio to the step pin is overwritten if PWM is used!
    step_pin = _step_pin;
    step_gpio = _step_gpio;
    init_pin(_direction_pin, _direction_gpio);
    direction_pin = _direction_pin;
    direction_gpio = _direction_gpio;
    init_pin(_m1_pin, _m1_gpio);
    m1_pin = _m1_pin;
    m1_gpio = _m1_gpio;
    init_pin(_m2_pin, _m2_gpio);
    m2_pin = _m2_pin;
    m2_gpio = _m2_gpio;
    init_pin(_m3_pin, _m3_gpio);
    m3_pin = _m3_pin;
    m3_gpio = _m3_gpio;

    // initialize the PWM
    fpioa_set_function(step_pin, FUNC_TIMER0_TOGGLE1);
    pwm_init(PWM_DEVICE_0);
}

void DRV8825::start(int _direction, float _RPM, int _step_size)
{
    set_direction(_direction);
    set_step_size(_step_size);
    set_RPM(_RPM);
    start();
}

void DRV8825::start(void)
{ // enable PWM signal
    pwm_set_enable((pwm_device_number_t)0, (pwm_channel_number_t)0, 1);
}

void DRV8825::stop(void)
{ // disable PWM signal
    pwm_set_enable((pwm_device_number_t)0, (pwm_channel_number_t)0, 0);
}

void DRV8825::init_pin(int pin, int gpio)
{
    // TODO: nothing is done if the data is incorrect!
    if ((gpio > 0) && (gpio <= 31) && (pin >= 0) && (pin <= 47))
    {
        fpioa_set_function(pin, (fpioa_function_t)(FUNC_GPIOHS0 + gpio));
        gpiohs_set_drive_mode(pin, GPIO_DM_OUTPUT);
    }
}

void DRV8825::set_direction(int _direction)
{
    direction = _direction;
    if (direction == 1)
        gpiohs_set_pin(direction_gpio, GPIO_PV_HIGH);
    else
        gpiohs_set_pin(direction_gpio, GPIO_PV_LOW);
}

void DRV8825::set_step_size(int _step_size)
{
    if ((_step_size >= 0) and (_step_size < 8))
    {   // set the three values that together determine the micro step
        // printf("m1=%d m2=%d m3=%d\n", m1_gpio, m2_gpio, m3_gpio);
        gpiohs_set_pin(m1_gpio, (gpio_pin_value_t)m_settings[_step_size][0]);
        gpiohs_set_pin(m2_gpio, (gpio_pin_value_t)m_settings[_step_size][1]);
        gpiohs_set_pin(m3_gpio, (gpio_pin_value_t)m_settings[_step_size][2]);
        // the number of micro_steps per full step
        micro_steps = m_settings[_step_size][3];
        step_size = _step_size;
    }
}

void DRV8825::set_RPM(float _RPM)
{
    if (_RPM > max_RPM) // check against the maximum RPM
        _RPM = max_RPM;

    RPM = _RPM;
    // from the RPM calculate the frequency of the PWM
    float steps_per_minute = _RPM * steps_per_revolution * micro_steps;
    float steps_per_second = steps_per_minute * 0.016666666666666;
    // set the PWM frequency for device 0 and channel 0
    pwm_set_frequency((pwm_device_number_t)0, (pwm_channel_number_t)0, steps_per_second, 0.1);
    // enable PWM signal
}

void DRV8825::do_steps(int num_of_steps)
{   // Normally the PWM signal is attached to the step_gpio. Thus, make sure the step_gpio value
    // is assigned to the step_pin.
    fpioa_set_function(step_pin, (fpioa_function_t)(FUNC_GPIOHS0 + step_gpio));
    gpiohs_set_drive_mode(step_pin, GPIO_DM_OUTPUT);

    for (int step = 0; step < num_of_steps; step++)
    {
        // The time between gpio value changes needs to be about 750 us. I don't know why because the PWM is much much faster (about 40us for step_size 3 (1/8) and RPM 970)
        gpiohs_set_pin(step_gpio, GPIO_PV_HIGH);
        usleep(750);
        gpiohs_set_pin(step_gpio, GPIO_PV_LOW);
        usleep(750);
    }
    // Restore the assignment of the PWM signal to the step_pin.
    fpioa_set_function(step_pin, FUNC_TIMER0_TOGGLE1);
    pwm_init(PWM_DEVICE_0);
}