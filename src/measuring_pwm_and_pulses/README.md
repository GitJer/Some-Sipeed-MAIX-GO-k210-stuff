It is meant for measuring the period, pulse length and duty cycle of PWM signals. If a PWM signal of period 1 ms and pulse width of 0.5 ms is used (duty cycle is 0.5), the output looks like this:
```
period=830 pulse_width=410 duty_cycle=0.493976
period=835 pulse_width=405 duty_cycle=0.485030
period=830 pulse_width=410 duty_cycle=0.493976
period=836 pulse_width=410 duty_cycle=0.490431
period=830 pulse_width=410 duty_cycle=0.493976
```
This means it actually comes up a bit short (it should have given a period of 1000 and a pulse width of 500).

It can also be used to measure the width of a pulse without assuming a PWM signal, see main.cpp.
When externally generating a pulse of 10ms, 5 seconds silence, then a pulse of 25ms and then again 5 seconds of no pulses, the output looks like this:
```
Pulse width = 20833
Pulse width = 8179
Pulse width = 20456
Pulse width = 8181
Pulse width = 20835
Pulse width = 8333
```
Again, the measured times are a bit too short (it should have been alternating 25000 and 10000). 

Although the k210 has some nice clocks, the SDK only offers timers, it doesn't seem to expose a simple clock. If it did, and this shouldn't be all that hard judging by the timer class, my code could be simpler and possibly more accurate. Basically the 'my_timer' class can then be deleted.
