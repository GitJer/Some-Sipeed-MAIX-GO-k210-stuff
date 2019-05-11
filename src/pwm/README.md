In the kendryte document "kendryte_standalone_programming_guide_20190311144158_en.pdf" there is an error in the chapter on PWM. The correct implementations are either:
```
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
```
Note the '+1' in fpioa_set_function, which is missing in the kendryte document.
Or, if PWM_CHANNEL_0 is used (without the '+1'):

```
    fpioa_set_function(1, FUNC_TIMER0_TOGGLE1);
    pwm_init(PWM_DEVICE_0);
    pwm_set_frequency(PWM_DEVICE_0, PWM_CHANNEL_0, 200000, 0.5);
    pwm_set_enable(PWM_DEVICE_0, PWM_CHANNEL_0, 1);

```
