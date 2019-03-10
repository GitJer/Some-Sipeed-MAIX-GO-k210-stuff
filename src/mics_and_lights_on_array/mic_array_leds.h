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


void init_mic_array_lights(void) 
{
    // data pin
    fpioa_set_function(24, FUNC_SPI0_D0); //MOSI
    // clock pin
    fpioa_set_function(25, FUNC_SPI0_SCLK); //CLK
    // init SPI
    spi_init(0, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(0, 20000000);

    // set the overall brighness factor
    brightness = 5;
}

void set_light(int light_number, int R, int G, int B)
{
   red[light_number] = R;
   green[light_number] = G;
   blue[light_number] = B;
}
