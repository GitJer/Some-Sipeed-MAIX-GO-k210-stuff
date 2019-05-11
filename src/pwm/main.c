#include <fpioa.h>
#include <unistd.h>
#include <pwm.h>

int main(void)
{
    fpioa_set_function(1, FUNC_TIMER0_TOGGLE1+1);
    pwm_init(PWM_DEVICE_0);
    pwm_set_frequency(PWM_DEVICE_0, PWM_CHANNEL_1, 200000, 0.5);
    pwm_set_enable(PWM_DEVICE_0, PWM_CHANNEL_1, 1);

    while (1)
    {
        sleep(1);
    }
}
