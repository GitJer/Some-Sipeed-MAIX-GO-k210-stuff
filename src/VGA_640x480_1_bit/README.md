This code produces a VGA signal of 640x480 @ 60Hz. This version is only for a binary image.

## VGA signal
On many web-sites on the internet the timing of VGA signals is explained, and how to connect it to a monitor.
This code produces a VGA 640x480@60Hz signal.
The future goal is to encoded the image to be displayed into one byte per pixel:
* bit 0: the horizontal sync signal (on the MaixGo this is LCD pin D0)
* bit 1,2: the red component of the image (currently only bit 1: the MaixGo LCD pin D1)
* bit 3,4,5: the green component of the image
* bit 6,7: the blue component of the image
In this implementation only bit 0 (the horizontal sync signal) and bit 1 is implemented.
The horizontal sync needs to be 1 for almost all of the signal, the sync puls itself is 0.

## Vertical sync
The vertical sync signal is not explicitly produced. 
The lines of the image are send out via SPI and DMA using an interrupt with a frequency of 60Hz.
After some experimenting it turned out that sending the image takes a little bit less than 1/60 of a second.
The SPI makes all signals 0 after finishing. 
Thus there is a gap of all zero signals after sending an image line and before sending a new one.
The monitor I use is capable of infering that this must be the horizontal sync.

## SPI
Normal SPI uses only one data line. The SPI on the MaixGo can also do DUAL, QUAD and OCTAL, resp. 2, 4 and 8 data lines in parallel.
The SPI0 clock is based on PLL0. There is a complicated calculation to determine the actual PLL0 clock based on the 
requested frequency in sysctl_pll_set_freq(). Additionally, the SPI clock is an integer divide of the PLL0 clock
and the requested SPI clock in spi_set_clk_rate().
