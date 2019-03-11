This is a repository with some 'stuff' I created while playing with the Sipeed MAIX GO board. That board features a Kendryte K210 processor.
I use the [kendryte_standalone_sdk](https://github.com/kendryte/kendryte-standalone-sdk) and [kendryte_toolchain](https://kendryte.com/downloads/) for this work.

The 'stuff' is:

* blink_and_three_way_button: shows the on-board LED changing color and it prints the status of the three-way button.This is done on two separate cores.
* fft: shows the fft of the on-board microphone signal with tickmarks at 1kHz ... 8kHz.
* LCD: allowes for using the Adafruit GFX with the LCD screen.
* lights_on_mic_array: Controls the LEDs on the microphone array via toggling the pins.
* lights_on_mic_array_SPI: Controls the LEDs on the microphone array via SPI.
* mics_and_lights_on_array: Reads the microphone signals of the array and indicates the direction of the sound (more or less, not using FFT!) 

