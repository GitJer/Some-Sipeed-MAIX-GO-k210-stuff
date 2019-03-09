This is a demo that uses the Adafruit_GFX library to control the MAIX GO LCD screen.
It has been assembled from various sources:
- Adafruit
- Arduino
- Maixpy

I tried to leave the Adafruit and Arduino stuff as close to the original as possible. But in order to get it compiled I had to change/add some lines of code.

Note that to get this compiled the -fpermissive compiler flag needs to be set. In the file kendryte-standalone-sdk/cmake/compile-flags.cmake this flag can be added to add_compile_flags:

add_compile_flags(CXX -std=gnu++17 -fpermissive)

