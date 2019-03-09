#include <stdio.h>
#include <unistd.h>
#include "fpioa.h"
#include "gpio.h"
#include "bsp.h"
#include "sleep.h"

#define NUMPIX 12
uint8_t red[NUMPIX], green[NUMPIX], blue[NUMPIX];
uint8_t brightness;

uint8_t data_pin=3;
uint8_t clock_pin=4;


void set_pin(uint8_t pin, uint8_t value)
{
    // set pin to value
    if (value==0) {
        gpio_set_pin(pin, GPIO_PV_LOW);
    } else {
        gpio_set_pin(pin, GPIO_PV_HIGH);
    }
}


void transfer(uint8_t data)
{
    for (int bit=7; bit>=0; bit--) {
        // set the data on the data pin
        set_pin(data_pin, data >> bit & 1);
        // give a clock pulse on the clock pin
        set_pin(clock_pin, 1);
        set_pin(clock_pin, 0);
    }
}

// adapted from https://github.com/pololu/apa102-arduino/blob/master/APA102.h
void set_low(void)
{
    // set the data pin low
    set_pin(data_pin, 0);
    // set the clock pin low
    set_pin(clock_pin, 0);
}

void start_frame(void)
{
    // start with both the data and clock pin values low
    set_low();
    // the SK9822 first expects 4 times 0000 0000 
    transfer(0);
    transfer(0);
    transfer(0);
    transfer(0);
}

void send_color(uint8_t pix_num)
{
    // set the overall brightness factor
    transfer(0b11100000 | brightness);
    // send the RGB values
    transfer(blue[pix_num]);
    transfer(green[pix_num]);
    transfer(red[pix_num]);
}

void end_frame(void)
{
    // the SK9822 finally expects 4 times 1111 1111
    for (uint8_t i = 0; i < 4; i++) transfer(255);
}

void write_pixels()
{
    // write the color data to the LEDs
    start_frame();
    for (uint8_t i=0; i<NUMPIX; i++) {
        send_color(i);
    }
    end_frame();
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
    // data_pin is output
    fpioa_set_function(24, FUNC_GPIO3);
    gpio_set_drive_mode(data_pin, GPIO_DM_OUTPUT); 
    // clock_pin is output
    fpioa_set_function(25, FUNC_GPIO4);
    gpio_set_drive_mode(clock_pin, GPIO_DM_OUTPUT); 

    brightness = 5;

    while(1) {
        rainbow();
    }
}
