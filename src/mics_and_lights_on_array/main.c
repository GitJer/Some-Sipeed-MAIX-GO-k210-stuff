/* made by GitJer, based on work by Canaan */

/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "i2s.h"
#include "sysctl.h"
#include "fpioa.h"
#include "uarths.h"
#include "mic_array_leds.h"


#define FRAME_LEN   128
uint32_t rx_buf[1024];

/* microphones on the array (silk screen readable, flex flat cable (anoyingly) over mic 4 and LED 6):

       5
   2       6
       C
   1       3
       4

   microphone on the MAIX GO board: B

   Note, two microphones are the left/right half of a stereo I2S channel. 
   On a single I2S channel the left/right are placed alternating in memory
   Also note that when using DMA all 8 mics are placed after each other: B, C, 1, 2, 3, 4, 5, 6

   microphone on MAIX GO board: pin 20, FUNC_I2S0_IN_D0 (even bytes)
   microphone C on mic array: pin 20, FUNC_I2S0_IN_D0 (odd bytes)
   microphone 1 on mic array: pin 21, FUNC_I2S0_IN_D1 (even bytes)
   microphone 2 on mic array: pin 21, FUNC_I2S0_IN_D1 (odd bytes)
   microphone 3 on mic array: pin 22, FUNC_I2S0_IN_D2 (even bytes)
   microphone 4 on mic array: pin 22, FUNC_I2S0_IN_D2 (odd bytes)
   microphone 5 on mic array: pin 23, FUNC_I2S0_IN_D3 (even bytes)
   microphone 6 on mic array: pin 23, FUNC_I2S0_IN_D3 (odd bytes)

   The top LED is number 0, counting clock-wise
   microphone 1 is at LED 8
   microphone 2 is at LED 10
   microphone 3 is at LED 4
   microphone 4 is at LED 6
   microphone 5 is at LED 0
   microphone 6 is at LED 2
*/

void io_mux_init(){
    // assign functions to pins

    // first the four I2S channels for I2S device 0
    fpioa_set_function(20, FUNC_I2S0_IN_D0);
    fpioa_set_function(21, FUNC_I2S0_IN_D1);
    fpioa_set_function(22, FUNC_I2S0_IN_D2);
    fpioa_set_function(23, FUNC_I2S0_IN_D3);
    // the left/right clock
    fpioa_set_function(19, FUNC_I2S0_WS);
    // the data clock
    fpioa_set_function(18, FUNC_I2S0_SCLK);
}

int main(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, 320000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    uarths_init();
    io_mux_init();

    // initialize the LEDs, and set them initially to off
    init_mic_array_lights();
    for (int l=0; l<12; l++) set_light(l, 0, 0, 0);

    // initialize all 4 I2S channels, stereo -> 8 microphones
    i2s_init(I2S_DEVICE_0, I2S_RECEIVER, 0xFF);

    // configure the 4 I2S channels
    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_0, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);
    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_1, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);
    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_2, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);
    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_3, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);

    // some variables for taking averages
    int32_t av[8]; 
    int num_averages = 50;
    float if_samples;

    // inverse, float of the number of samples taken
    if_samples = 1./(float)(num_averages * FRAME_LEN);

    while (1)
    {
        // set averages to zero to start
        for (int i=0; i<8; i++) av[i] = 0;

        // for take num_averages samples (of FRAME_LEN samples each)
        for (int a=0; a<num_averages; a++) {
            // read the data 
            i2s_receive_data_dma(I2S_DEVICE_0, &rx_buf[0], FRAME_LEN * 8, DMAC_CHANNEL1);
            
            // read the samples of the 8 mics from the rx_buffer
            for (int f=0; f<FRAME_LEN; f++) {
                for (int i=0; i<8; i++) {
                    av[i] += abs(rx_buf[8*f+i]);
                }
            }
        }

        // correct for the number of samples taken
        for (int i=0; i<8; i++) {
            av[i] = (int) ((float)av[i]*if_samples);
            if (av[i]>255) av[i]=255;
        }

        // if only noise is present (a low central microphone output), set all array mic averages to 0.
        // else subract the central microphone output from the other array mics, and make sure that their 
        // value is between 0 and 255.
        if (av[1] > 10) {
            for (int i=2; i<8; i++) {
                av[i] -= av[1];
                if (av[i]<0) av[i] = 0;
                if (av[i]>255) av[i]=255;
            }
        } else {
            for (int i=2; i<8; i++) av[i] = 0;
        }

        // printf("mB=%d\tmC=%d\tm1=%d\tm2=%d\tm3=%d\tm4=%d\tm5=%d\tm6=%d\n", av[0], av[1], av[2], av[3], av[4], av[5], av[6], av[7]);

        // set the LEDs to white with brightness according to their average microphone output
        set_light(8, av[2], av[2], av[2]);
        set_light(10, av[3], av[3], av[3]);
        set_light(4, av[4], av[4], av[4]);
        set_light(6, av[5], av[5], av[5]);
        set_light(0, av[6], av[6], av[6]);
        set_light(2, av[7], av[7], av[7]);
        write_pixels();
    }
    return 0;
}
