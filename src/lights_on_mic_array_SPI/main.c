#include <unistd.h>
#include <stdio.h>
#include "fpioa.h"
#include "sleep.h"
#include "spi.h"

// The Sipeed microphone array has 12 SK9822 LEDs
#define NUMPIX 12
uint8_t red[NUMPIX], green[NUMPIX], blue[NUMPIX];
// The SK9822 has an overal brightness factor
uint8_t brightness;

// The buffer to send the data to the mic array
#define TX_LEN 4+NUMPIX*4+4
uint8_t tx_buffer[TX_LEN];

// send the pixel data to the mic array using SPI
void write_pixels()
{
    uint8_t tx_place=0;

    // the SK9822 first expects 4 times 0000 0000 
    for (int i=0; i<4; i++) tx_buffer[tx_place+i] = 0; 
    tx_place+=4; 
    
    // write the color data to the tx_buffer
    for (uint8_t i=0; i<NUMPIX; i++) {
        // set the overall brightness factor
        tx_buffer[tx_place] = (0b11100000 | brightness);
        // set the RGB values
        tx_buffer[tx_place+1] = blue[i];
        tx_buffer[tx_place+2] = green[i];
        tx_buffer[tx_place+3] = red[i];
        tx_place += 4;
    }

    // the SK9822 finally expects 4 times 1111 1111
    for (int i=0; i<4; i++) tx_buffer[tx_place+i] = 255; 

    // send the data to the mic array
    spi_send_data_standard(0, 0, NULL, 0, tx_buffer, TX_LEN);
}


// Adapted from Adafruit strandtest: 
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void wheel(uint8_t i, uint8_t j) {
  uint16_t wheelPos = 255 - ((i*256/NUMPIX + j) & 255);
  if(wheelPos < 85) {
    red[i] = 255 - wheelPos * 3;
    green[i] = 0,
    blue[i] = wheelPos * 3;
    return;
  }
  if(wheelPos < 170) {
    wheelPos -= 85;
    red[i] = 0; 
    green[i] = wheelPos * 3;
    blue[i] = 255 - wheelPos * 3;
    return;
  }
  wheelPos -= 170;
  red[i] = wheelPos * 3;
  green[i] = 255 - wheelPos * 3;
  blue[i] =  0; 
}

void rainbow(void) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<NUMPIX; i++) {
        wheel(i, j);
    }
    write_pixels();
    usleep(10000);
  }
}


int main(void) 
{
    // data pin
    fpioa_set_function(24, FUNC_SPI0_D0); //MOSI
    // clock pin
    fpioa_set_function(25, FUNC_SPI0_SCLK); //CLK
    // init SPI
    spi_init(0, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(0, 25000000);

    // set the overall brighness factor
    brightness = 5;

    while(1) {
       rainbow();
    }
}
