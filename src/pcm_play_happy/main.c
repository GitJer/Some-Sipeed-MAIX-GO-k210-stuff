// made by GitJer, inspired by work of Canaan

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
#include "i2s.h"
#include "sysctl.h"
#include "fpioa.h"
#include "uarths.h"
#include "math.h"
#include "gpio.h"


#define RX_BUF_THRESHOLD     (1024*8)
#define MAX_BUF_LEN          (1024*16)

// This length should provide 1 second of audio data
#define play_frequency 705600
uint32_t test_pcm[play_frequency];

// happy birthday.
// Some tones do not sound very happy. Maybe it is the speaker that has trouble
// producing this tone, maybe I am not a good musician.
// This array specifies tones to be played (note: 89 means no tone)
uint8_t notes[50]={8, 89, 8, 89, 10, 89, 8, 89, 13, 89, 12, 89, 
                   8, 89, 8, 89, 10, 89, 8, 89, 15, 89, 13, 89,
                   8, 89, 8, 89, 20, 89, 17, 89, 13, 89, 12, 89, 10, 89,
                   18, 89, 18, 89, 17, 89, 13, 89, 15, 89, 13, 89};
// This array specifies the duration of a tone
float lengths[50]={.75, 0.05, .25, 0.05, 1., 0.05, 1., 0.05, 1., 0.05, 2., 0.05, 
                   .75, 0.05, .25, 0.05, 1., 0.05, 1., 0.05, 1., 0.05, 2., 0.05, 
                   .75, 0.05, .25, 0.05, 1., 0.05, 1., 0.05, 1., 0.05, 1., 0.05, 1., 0.05,
                   .75, 0.05, .25, 0.05, 1., 0.05, 1., 0.05, 1., 0.05, 2., 2.00};
               
// from Arduino 
uint32_t tones[] = {31, //NOTE_B0 
33, //NOTE_C1 
35, //NOTE_CS1 
37, //NOTE_D1 
39, //NOTE_DS1 
41, //NOTE_E1 
44, //NOTE_F1 
46, //NOTE_FS1 
49, //NOTE_G1 
52, //NOTE_GS1 
55, //NOTE_A1 
58, //NOTE_AS1 
62, //NOTE_B1 
65, //NOTE_C2 
69, //NOTE_CS2 
73, //NOTE_D2 
78, //NOTE_DS2 
82, //NOTE_E2 
87, //NOTE_F2 
93, //NOTE_FS2 
98, //NOTE_G2 
104, //NOTE_GS2 
110, //NOTE_A2 
117, //NOTE_AS2 
123, //NOTE_B2 
131, //NOTE_C3 
139, //NOTE_CS3 
147, //NOTE_D3 
156, //NOTE_DS3 
165, //NOTE_E3 
175, //NOTE_F3 
185, //NOTE_FS3 
196, //NOTE_G3 
208, //NOTE_GS3 
220, //NOTE_A3 
233, //NOTE_AS3 
247, //NOTE_B3 
262, //NOTE_C4 
277, //NOTE_CS4 
294, //NOTE_D4 
311, //NOTE_DS4 
330, //NOTE_E4 
349, //NOTE_F4 
370, //NOTE_FS4 
392, //NOTE_G4 
415, //NOTE_GS4 
440, //NOTE_A4 
466, //NOTE_AS4 
494, //NOTE_B4 
523, //NOTE_C5 
554, //NOTE_CS5 
587, //NOTE_D5 
622, //NOTE_DS5 
659, //NOTE_E5 
698, //NOTE_F5 
740, //NOTE_FS5 
784, //NOTE_G5 
831, //NOTE_GS5 
880, //NOTE_A5 
932, //NOTE_AS5 
988, //NOTE_B5 
1047, //NOTE_C6 
1109, //NOTE_CS6 
1175, //NOTE_D6 
1245, //NOTE_DS6 
1319, //NOTE_E6 
1397, //NOTE_F6 
1480, //NOTE_FS6 
1568, //NOTE_G6 
1661, //NOTE_GS6 
1760, //NOTE_A6 
1865, //NOTE_AS6 
1976, //NOTE_B6 
2093, //NOTE_C7 
2217, //NOTE_CS7 
2349, //NOTE_D7 
2489, //NOTE_DS7 
2637, //NOTE_E7 
2794, //NOTE_F7 
2960, //NOTE_FS7 
3136, //NOTE_G7 
3322, //NOTE_GS7 
3520, //NOTE_A7 
3729, //NOTE_AS7 
3951, //NOTE_B7 
4186, //NOTE_C8 
4435, //NOTE_CS8 
4699, //NOTE_D8 
4978, //NOTE_DS8 
0}; //space


// generate the data for one tone
void produce_tone(uint32_t frequency, uint32_t length, uint32_t start_position)
{
    float freq_fac;
    float time_fac;
    float pos_sine;
    float amplitude;

    freq_fac = (float)frequency*(2.*M_PI);
    for (uint32_t i=0; i<length; i++) {
        time_fac = (float)(i)/(float)length;
        pos_sine = (1.+sin(freq_fac*time_fac));
        amplitude = 32767.;
        test_pcm[i+start_position] = amplitude*pos_sine;
    }
}

// build the array that contains the music
void build_music_pcm(void)
{
    float l, total_lenght = 0.;
    uint32_t start_position = 0;
    uint32_t n;
    
    for (int i=0; i<50; i++) {
        total_lenght += lengths[i];
    }
    
    float length_per_unit = (float)play_frequency/total_lenght;
    printf("tl=%f lpu=%f\n", total_lenght, length_per_unit);
    
    for (int i=0; i<50; i++) {
        n = notes[i];
        l = lengths[i] * length_per_unit;
        printf("t[n]=%d l=%lf sp=%d\n",tones[n], l, start_position);
        produce_tone(tones[n], l, start_position);
        start_position += (uint32_t)l;
    }
}

int main(void)
{
    uint32_t frequency;

    sysctl_pll_set_freq(SYSCTL_PLL0, 320000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    uarths_init();

    // assign functions to pins. According to the schematic (K210 & DAC), pins 33, 34 and 35 control the speaker.
    fpioa_set_function(34, FUNC_I2S0_OUT_D1);
    fpioa_set_function(35, FUNC_I2S0_SCLK);
    fpioa_set_function(33, FUNC_I2S0_WS);

    // power on audio amplifier
    fpioa_set_function(32, FUNC_GPIO0); 
    gpio_init();
    gpio_set_drive_mode(0, GPIO_DM_OUTPUT);
    gpio_set_pin(0, GPIO_PV_HIGH);


    // It doesn't seem to matter which of the following two bits are 1:
    // 11000000(0xC0), 00110000(0x30), 00001100(0x0C), 00000011(0x03).
    // Two bits indicate stereo sound (I think) but only one channel is actually 
    // connected on the MAIX GO, the other does have some crosstalk, but not loud.
    // If more than one channel is active, e.g. 00001111, the music will play twice as fast!
    i2s_init(I2S_DEVICE_0, I2S_TRANSMITTER, 0x03);

    i2s_tx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_1, RESOLUTION_16_BIT, SCLK_CYCLES_32, TRIGGER_LEVEL_4, RIGHT_JUSTIFYING_MODE);

    // make the array with tones
    build_music_pcm();

    // repeat endlessly
    while (1) {
        printf("start\n");
        i2s_play(I2S_DEVICE_0, DMAC_CHANNEL0, (uint8_t *)test_pcm, sizeof(test_pcm), 1024, 16, 2);
    }
}
