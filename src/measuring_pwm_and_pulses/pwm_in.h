#include <gpiohs.h>

#ifndef PWMIN
#define PWMIN

class PwmIn
{
public:
    // Create a PwmIn. The inputs are the physical pin and the gpiohs number
    PwmIn(int _physical_pin, int _gpiohs_number);

    // return the current PWM period
    uint32_t get_period(void) const;

    // return the current pulse width
    uint32_t get_pulsewidth(void) const;

    // return the current PWM duty cycle
    float get_dutycycle(void) const;

    // reset the pulsewidth (handy for measureing pulsewidth for non-PWM signals)
    void reset_pulsewidth(void);

private:
    // the call back function for the interrupt on the pin value change
    static int irq_gpiohs_key(void *ctx);

    // the physical pin number
    int physical_pin;
    // the gpiohs number
    volatile static int gpiohs_number;

    // the pulsewidth and the period of the PWM signal
    volatile static uint32_t pulsewidth, period;
    volatile static uint64_t zero_time;
};

#endif