
This code searches for PLL0 and SPI0 clock settings, given a desired SPI0 clock.

The PLL0 clock can't be set to just any value. There is a complicated calculation that takes a requested frequency and determines the nearest clock frequency and sets that as PLL0 clock.

The SPI0 clock is dependent on (half of !?!) the PLL0 frequency and also can't be set to just any value. This time the calculation is rather simple, there needs to be an intenger ratio (baudr) between the half of the PLL0 and the SPI0 clock frequency. In the code the integer ratio can have values [2, 65534]. 

However, I observed some issues when producing a very large stream of alternating 0s and 1s in the VGA experiments.

## buadr = 2 or 3

The highest SPI0 frequencies for a given PLL0 frequency are obtained for low baudr values. If the baudr is 2 or 3, the signal I observe is:

<img src="https://github.com/GitJer/Some-Sipeed-MAIX-GO-k210-stuff/blob/master/src/VGA/PLL0_and_SPI0_clock_settings/baudr_2_3.bmp" width="500">

Note that there are 16 values (8 1s with 0s inbetween). So, this setting may be usable for sending e.g. commands of 16 bits, but not for a large stream of bits.


## buadr = 4 or 5

If the baudr is 4 or 5, the signal I observe is:

<img src="https://github.com/GitJer/Some-Sipeed-MAIX-GO-k210-stuff/blob/master/src/VGA/PLL0_and_SPI0_clock_settings/baudr_4_5.bmp" width="500">

Note that there are blocks of 16 and 32 bits, but there was no obvious pattern in when a block of 16 or when a block of 32 bits was sent. So, again, this setting may be usable for sending e.g. commands of 16 bits, but not for a large stream of bits.


## buadr = 6 or higher

If the baudr is 6 or hiher, the signal I observe is correct:

<img src="https://github.com/GitJer/Some-Sipeed-MAIX-GO-k210-stuff/blob/master/src/VGA/PLL0_and_SPI0_clock_settings/baudr_6_and_further.bmp" width="500">

## Usage
You can specify the minimum and maximum PLL0 frequencies to consider at the top of the main function. There you can also specify the desired SPI0 frequency. 

For example, when trying to find a SPI0 frequency of 65MHz and specifying to search between PLL0 frequencies from 200MHz to 800MHz, the result is:

```
The requested SPI0 frequency is 65000000
below: pll0=767000000 spi0=63916666 (diff=1083334)
above: pll0=780000000 spi0=65000000 (diff=0)
```

