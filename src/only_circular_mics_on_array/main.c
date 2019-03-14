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
#include <inttypes.h>
#include "i2s.h"
#include "sysctl.h"
#include "fpioa.h"
#include "uarths.h"


#define FRAME_LEN   128
uint32_t rx_buf[1024];
#define NUM_MICS 6

/* microphones on the array (silk screen readable, flex flat cable behind mic 4 and LED 6):

       5
   2       6
       C            microphone on the MAIX GO board: B
   1       3
       4

   Notes:
    - two microphones are the left/right half of a stereo I2S channel. 
    - on a single I2S channel the left/right are placed alternating in memory

   In the code below only the 6 mics on the array are active, not C or B.
   Using DMA the data of the 6 mics are placed after each other: 1, 2, 3, 4, 5, 6

   not active: microphone on MAIX GO board: pin 20, FUNC_I2S0_IN_D0 (even bytes)
   not active: microphone C on mic array: pin 20, FUNC_I2S0_IN_D0 (odd bytes)
   microphone 1 on mic array: pin 21, FUNC_I2S0_IN_D1 (even bytes)
   microphone 2 on mic array: pin 21, FUNC_I2S0_IN_D1 (odd bytes)
   microphone 3 on mic array: pin 22, FUNC_I2S0_IN_D2 (even bytes)
   microphone 4 on mic array: pin 22, FUNC_I2S0_IN_D2 (odd bytes)
   microphone 5 on mic array: pin 23, FUNC_I2S0_IN_D3 (even bytes)
   microphone 6 on mic array: pin 23, FUNC_I2S0_IN_D3 (odd bytes)
 */

void io_mux_init(){
    // assign functions to pins

    // first the four I2S channels for I2S device 0
    // not active mics B and C: fpioa_set_function(20, FUNC_I2S0_IN_D0);
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

    // initialize 3 I2S channels, stereo -> 6 microphones
    i2s_init(I2S_DEVICE_0, I2S_RECEIVER, 0xFC);
    // Note that this is 0xFF - 0x03: the first two bits are for mics B and C.

    // configure the 3 I2S channels
    // not active mics B and C: i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_0, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);
    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_1, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);
    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_2, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);
    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_3, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, STANDARD_MODE);

    while (1)
    {
        // read the data of the mics via DMA
        i2s_receive_data_dma(I2S_DEVICE_0, &rx_buf[0], FRAME_LEN * NUM_MICS, DMAC_CHANNEL1);
        // print the results
        for (int m=0; m<NUM_MICS; m++) {
            printf("%d\t\t", rx_buf[m]);
        }
        printf("\n");
    }
    return 0;
}
