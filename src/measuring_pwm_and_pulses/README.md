This example code is meant for measuring the period, pulse length and duty cycle of PWM signals. If a PWM signal of period 1 ms and pulse width of 0.5 ms is used (duty cycle is 0.5), the output looks like this:
```
period=1000 pulse_width=491 duty_cycle=0.491000
period=1000 pulse_width=491 duty_cycle=0.491000
period=1001 pulse_width=491 duty_cycle=0.490509
period=1000 pulse_width=491 duty_cycle=0.491000
period=1000 pulse_width=490 duty_cycle=0.490000
period=1000 pulse_width=491 duty_cycle=0.491000
period=1000 pulse_width=490 duty_cycle=0.490000
period=1000 pulse_width=491 duty_cycle=0.491000
```
Period and pulse_width are measured in microseconds. The pulse width is a bit shorter than expected, but other measurements are spot on, so maybe the pulse generator produces a pulse that is a bit shorter than 0.5 ms.

It can also be used to measure the width of a pulse without assuming a PWM signal, see main.cpp.
When the input signal is a pulse of 10ms, 5 seconds silence, then a pulse of 25ms and then again 5 seconds of no pulses, the output looks like this:
```
Pulse width = 10001
Pulse width = 25003
```