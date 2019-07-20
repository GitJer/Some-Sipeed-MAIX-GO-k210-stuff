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

There is also a method that alows to take a number of steps. It accomplishes this by toggeling 
the step_gpiohs itself. But, to do this it must assign the step_gpiohs to the step_pin, and later
assign the PWM to the step_gpiohs again.

ALL NOTES BELOW ARE TESTED ON 12V

NOTE: the stepper motor I have only wants to do step size 0, 1, 2, and 3, not 4 (1/16) or 5,6,7 (1/32). I don't know why.
NOTE: the stepper motor I have is not always reliable at low RPM (cold start problems?). Adding this to the code helps:
        my_stepper.start(1, 5, 2);
        sleep(1);
NOTE: the stepper motor can't change direction at high speed (e.g. high RPM clockwise to high RPM counter clockwise) It will still make sound, but there is no rotation
NOTE: the maximum RPM changes with step_size:
      on step_size 0 the maxRPM is about 800
      on step_size 1 the maxRPM is about 850
      on step_size 2 the maxRPM is about 950
      on step_size 3 the maxRPM is about 970
      If the voltage is higher, (much) higher RPM can be obtained. For example at 30V and step_size 3, the maxRPM is about 1850
*/

#include <gpiohs.h>
#include <fpioa.h>
#include <pwm.h>

class DRV8825
{
public:
    // initialize the class by giving it all 5 pins. For each a physical pin as well as a gpiohs must be supplied
    DRV8825(int _step_pin, int _step_gpio, int _direction_pin, int _direction_gpio, int _m1_pin, int _m1_gpio, int _m2_pin, int _m2_gpio, int _m3_pin, int _m3_gpio);

    // set the direction of the motor
    void set_direction(int _direction);

    // set the step size: from full step (0) to 1/32th step (5), see the array 'm_settings'
    void set_step_size(int _step_size);
    // set the rotation speed in RPM
    void set_RPM(float _RPM);
    // start the motor with new parameters
    void start(int _direction, float _RPM, int _step_size);
    // start the motor with current parameters
    void start(void);
    // stop the motor
    void stop(void);
    // make a specified number of steps in the current direction and step_size
    void do_steps(int _number_of_steps);

private:
    // the stepper motor I use has 200 steps per full rotation
    int steps_per_revolution = 200; // 1.8 degree per step
    // variabel to hold the RPM (not really used)
    float RPM = 1;
    // the maximum rotational speed
    float max_RPM = 500; // See notes above: it depends on the step_size and the voltage to the stepper motor!  
    // the number of micro steps within a normal step
    int micro_steps = 1;
    // the derection of rotation
    int direction = 0;
    // the step size
    int step_size = 0;
    // the physical pins
    int step_pin, direction_pin, m1_pin, m2_pin, m3_pin;
    // the gpiohs numbers
    int step_gpio, direction_gpio, m1_gpio, m2_gpio, m3_gpio;

    // array to go from an integer 'step_size' to the three pin settings to select a step size.
    // The 4th column is the number of steps within one full step.
    int m_settings[8][4] = {
        {0, 0, 0, 1},  // Full step
        {1, 0, 0, 2},  // 1/2 step
        {0, 1, 0, 4},  // 1/4 step
        {1, 1, 0, 8},  // 1/8 step
        {0, 0, 1, 16}, // 1/16 step
        {1, 0, 1, 32}, // 1/32 step
        {0, 1, 1, 32}, // 1/32 step
        {1, 1, 1, 32}  // 1/32 step
    };

    // private method to check the pin and gpio and assign them using fpioa
    void init_pin(int pin_num, int gpio_num);
};