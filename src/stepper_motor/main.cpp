/*
This is a class for running a stepper motor with a DRV8825 driver.
Driver: DRV8825
Stepper motor: Nema17 Stepper 42BYGH 17HS4401S 4-lead

The DRV8825 has:
- a step pin that triggers a single step. Only the up edge matters, the pulse width doesn't matter (they say)
- a direction pin that changes the direction of the stepper motor
- three pins that together determine the step size (from full step to 1/32th step)

Given a RPM, a PWM signal is used to generate pulses that cause steps. The function do_steps uses bit-banging, however.

This code uses: PWM device 0 and channel 0
This code uses: FUNC_TIMER0_TOGGLE1 assigned to the step_pin (which outputs the PWM signal)
*/

#include <unistd.h>
#include "stepper.h"
// #include <stdio.h>

int main()
{
    // initialize the stepper/DRV8825/k210
    /* This demo uses:
                            Physical, GPIOHS
        step pin:           1           1 
        direction pin:      2           2
        micro step pin m1:  3           3
        micro step pin m2:  4           4
        micro step pin m3:  5           5
        You can change these based on what physical pins you desire 
        and which other GPIOHS you use in your code
    */
    DRV8825 my_stepper(1, 1, 2, 2, 3, 3, 4, 4, 5, 5);
    while (1)
    {
        float maxRPM = 100;

        /*
            Demo to show micro steps and that the motor can be stopped and started
        */

        // // to help against a 'cold start' (this is not an official term, I think ...)
        // // Note, the direction is the same as the next rotation
        // // Its not perfect though ...

        my_stepper.start(1, 5, 2);
        sleep(1);
        
        // start with large steps, progressively make finer steps (same rotational speed)
        my_stepper.start(1, maxRPM, 0);
        sleep(1);
        my_stepper.start(1, maxRPM, 1);
        sleep(1);
        // stop and then continue
        my_stepper.stop();
        sleep(1);
        my_stepper.start();
        sleep(1);
        my_stepper.start(1, maxRPM, 2);
        sleep(1);
        my_stepper.start(1, maxRPM, 3);
        sleep(1);
        // start with fine steps (but change direction), progressively make larger steps (same rotational speed)
        my_stepper.start(0, maxRPM, 3);
        sleep(1);
        my_stepper.start(0, maxRPM, 2);
        sleep(1);
        my_stepper.start(0, maxRPM, 1);
        sleep(1);
        my_stepper.start(0, maxRPM, 0);
        // pause for 2 seconds
        my_stepper.stop();
        sleep(5);


        /*
            Demo to show increasing speed
        */

        // to help against a 'cold start' (this is not an official term, I think ...)
        // Note, the direction is the same as the next rotation
        // Its not perfect though ...
        my_stepper.start(1, 5, 2);
        sleep(1);

        maxRPM = 500;
        // speed up
        for (float RPM = 0; RPM < maxRPM; RPM += 2.) {
            my_stepper.start(1, RPM, 3);
            usleep(10000);
        }
        // slow down
        for (float RPM = maxRPM; RPM > 0; RPM -= 2.) {
            my_stepper.start(1, RPM, 3);
            usleep(10000);
        }
        // start from slow in the other direction
        for (float RPM = 0; RPM < maxRPM; RPM += 2.) {
            my_stepper.start(0, RPM, 3);
            usleep(10000);
        }
        // slow down
        for (float RPM = maxRPM; RPM > 0; RPM -= 2.) {
            my_stepper.start(0, RPM, 3);
            usleep(10000);
        }
        my_stepper.stop();
        sleep(5);

        /* 
            Demo to make a number of steps back and forth
        */

        my_stepper.set_step_size(0);

        my_stepper.set_direction(0);
        my_stepper.do_steps(200);
        sleep(1);
        my_stepper.set_direction(1);
        my_stepper.do_steps(200);
        sleep(1);
    }
}