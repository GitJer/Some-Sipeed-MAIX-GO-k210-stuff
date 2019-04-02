This is a repository with some 'stuff' I created while playing with the Sipeed MAIX GO board. That board features a Kendryte K210 processor.
I use the [kendryte-standalone-sdk](https://github.com/kendryte/kendryte-standalone-sdk) and [kendryte-toolchain](https://kendryte.com/downloads/) for this work, and often base it on examples in [kendryte-standalone-demo](https://github.com/kendryte/kendryte-standalone-demo). 

The 'stuff' is:

* blink_and_three_way_button: shows the on-board LED changing color and it prints the status of the three-way button.This is done on two separate cores.
* fft: shows the fft of the on-board microphone signal with tickmarks at 1kHz ... 8kHz.
* LCD: allowes for using the Adafruit GFX with the LCD screen.
* lights_on_mic_array: Controls the LEDs on the microphone array via toggling the pins.
* lights_on_mic_array_SPI: Controls the LEDs on the microphone array via SPI.
* mics_and_lights_on_array: Reads the microphone signals of the array and indicates the direction of the sound (more or less, not using APU or FFT!)
* only_circular_mics_on_array: Only use the 6 circularly placed mics on the array.
* pcm_play_happy: generates pcm sound (poorly).
* camera_lcd:	demo that shows the ov2640 camera on the LCD screen.
* binocular_camera_lcd:	demo for the binocular camera module.
* touchscreen: simple demo for the LCD touchscreen.
* accelerometer: reads the x, y and z acceleration of the MSA300 accelerometer


The kendryte demos are (probably) for their KD233 board. There are some differences between that board and the Sipeed MAIX GO but often the main difference is the assignment of functions to pins. For example, assigning the microphone outputs to a pin in the kendryte demos looks like this:
```
  // the four I2S channels for I2S device 0
  fpioa_set_function(42, FUNC_I2S0_IN_D0);
  fpioa_set_function(43, FUNC_I2S0_IN_D1);
  fpioa_set_function(44, FUNC_I2S0_IN_D2);
  fpioa_set_function(45, FUNC_I2S0_IN_D3);
  // the left/right clock
  fpioa_set_function(46, FUNC_I2S0_WS);
  // the data clock
  fpioa_set_function(39, FUNC_I2S0_SCLK);
```
While for the MAIX GO it looks like this:
```
  // the four I2S channels for I2S device 0
  fpioa_set_function(20, FUNC_I2S0_IN_D0);
  fpioa_set_function(21, FUNC_I2S0_IN_D1);
  fpioa_set_function(22, FUNC_I2S0_IN_D2);
  fpioa_set_function(23, FUNC_I2S0_IN_D3);
  // the left/right clock
  fpioa_set_function(19, FUNC_I2S0_WS);
  // the data clock
  fpioa_set_function(18, FUNC_I2S0_SCLK);
```
You can find the correct pin numbers by looking at examples, making educated guesses, and looking at the schematic of the MAIX GO. For example, for the microphone array:

<img src="https://github.com/GitJer/Some-Sipeed-MAIX-GO-k210-stuff/blob/master/part_of_MAIX_GO_schematic.png" width="100">

## Compiling:

Note that to get this compiled the -fpermissive compiler flag needs to be set. In the file kendryte-standalone-sdk/cmake/compile-flags.cmake this flag can be added to add_compile_flags:

add_compile_flags(CXX -std=gnu++17 -fpermissive)

At first I used the pre-compiled toolchain from the Kendryte website, but then (25-3-2019) switched to the versions on github [kendryte-gnu-toolchain](https://github.com/kendryte/kendryte-gnu-toolchain). This caused two additional problems. There was an "undefined reference to `vtable for __cxxabiv1::__si_class_type_info" error, which was solved by adding yet another compile flag: 

```
add_compile_flags(CXX -std=gnu++17 -fpermissive -fno-rtti)
```

There is also a linker error "undefined reference to `__cxa_pure_virtual'" for some of the files. The root cause probably has something to do with the Arduino origin of those files. This was solved by adding the following at the start of the problematic files:

```
extern "C" void __cxa_pure_virtual() { while (1); }
```
