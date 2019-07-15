#include <stdio.h>
#include <unistd.h>
#include "pwm_in.h"

int main(void)
{
    PwmIn my_pwm_in(46, 1);

    while (1)
    {
        usleep(100000);

        // uncomment this for PWM
        printf("period=%d pulse_width=%d duty_cycle=%f\n", my_pwm_in.get_period(), my_pwm_in.get_pulsewidth(), my_pwm_in.get_dutycycle());

        // uncomment this for occasional pulses
        // uint32_t pulsewidth = my_pwm_in.get_pulsewidth();
        // if (pulsewidth > 0)
        // {
        //     printf("Pulse width = %d\n", pulsewidth);
        //     my_pwm_in.reset_pulsewidth();
        // }
    }
    return 0;
}
