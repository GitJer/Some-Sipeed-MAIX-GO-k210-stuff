// This is a simple example for the Sipeed MAIX GO board.
// On core 0 it blinks the 3 color LED 
// On core 1 it reads the pin status of the 3-way switch

// Adapted from various sources by GitJer

// Original copyright notice of one of the sources:
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
#include <stdio.h>
#include <unistd.h>
#include "fpioa.h"
#include "gpio.h"
#include "bsp.h"

int core1_function(void *ctx)
{
    uint64_t core = current_coreid();
    printf("Core %ld (should be 1)\n", core);

    int press_key = 3; 
    int down_key = 4; 
    int up_key = 5; 

    fpioa_set_function(15, FUNC_GPIO3); // PRESS KEY = BOOT KEY
    fpioa_set_function(16, FUNC_GPIO4); // DOWN
    fpioa_set_function(17, FUNC_GPIO5); // UP KEY

    // initialize the GPIOs
    gpio_init();
    // set all three keys pins as input
    gpio_set_drive_mode(press_key, GPIO_DM_INPUT); 
    gpio_set_drive_mode(down_key, GPIO_DM_INPUT); 
    gpio_set_drive_mode(up_key, GPIO_DM_INPUT); 
    
    while(1) {
        // f=function number, e.g. FUNC_GPIO3
        // p=pin number, e.g. 3
        // b=pin number on the board, e.g. 15 
        printf("value on pins f3,p3,b15,press=%d, f4,p4,b16,down=%d, f5,p5,b17,up=%d\n", gpio_get_pin(press_key), gpio_get_pin(down_key), gpio_get_pin(up_key));
        sleep(0.5);
    }
}


int main(void)
{
    int RED = 1;
    int BLUE = 2;
    int GREEN = 0;

    uint64_t core = current_coreid();
    printf("Core %ld (should be 0)\n", core);
    register_core1(core1_function, NULL);
    // sleep for 1 second to let core 1 print a message
    // note: printing on both cores causes garbled text output
    // sleep(1);

    // map the functions of the LEDS to pins
    fpioa_set_function(12, FUNC_GPIO1); // RED
    fpioa_set_function(13, FUNC_GPIO2); // BLUE
    fpioa_set_function(14, FUNC_GPIO0); // GREEN

/* NOTE:
    I still do not understand how the fpioa maps the GPIO0 function to pin 14, but you have to 
    set pin 0 as gpio in gpio_set_drive and gpio_set_pin.
    According to fpioa FUNC_GPIO0 = 56
    Physical pin on the board = 14 (has green LED attached to it)
        NOTE: THERE IS AN ERROR IN THE LABELING ON THE SILK SCREEN (on the first version of this board)!
        according to the schematic and the silk screen the green LED is attached to physical pin 12
*/

    // initialize the GPIOs
    gpio_init();
    // set all three LED pins as output
    gpio_set_drive_mode(RED, GPIO_DM_OUTPUT); 
    gpio_set_drive_mode(BLUE, GPIO_DM_OUTPUT); 
    gpio_set_drive_mode(GREEN, GPIO_DM_OUTPUT); 

    // make sure all three leds are off at the start
    gpio_set_pin(RED, GPIO_PV_HIGH);
    gpio_set_pin(BLUE, GPIO_PV_HIGH);
    gpio_set_pin(GREEN, GPIO_PV_HIGH);

    while (1)
    {
        // Note: printing on both cores causes garbled text output
        // printf("Start loop\n");
        gpio_set_pin(RED, GPIO_PV_LOW);
        sleep(1);
        gpio_set_pin(RED, GPIO_PV_HIGH);
        gpio_set_pin(GREEN, GPIO_PV_LOW);
        sleep(1);
        gpio_set_pin(GREEN, GPIO_PV_HIGH);
        gpio_set_pin(BLUE, GPIO_PV_LOW);
        sleep(1);
        gpio_set_pin(BLUE, GPIO_PV_HIGH);
    }
    return 0;
}
