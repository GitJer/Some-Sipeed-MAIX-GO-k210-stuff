/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include "Adafruit_GFX.h"    // Core graphics library
#include "my_st7789.h" // Hardware-specific library for ST7789
#include <unistd.h>

#define PGM_P   const char *

// For the MAIX GO LCD screen:
my_ST7789 tft = my_ST7789(240, 320);

float p = 3.1415926;

void testlines(uint16_t color) {
    tft.fillScreen(BLACK);
    for (int16_t x=0; x < tft.width(); x+=6) {
        tft.drawLine(0, 0, x, tft.height()-1, color);
        sleep(0);
    }
    for (int16_t y=0; y < tft.height(); y+=6) {
        tft.drawLine(0, 0, tft.width()-1, y, color);
        sleep(0);
    }

    tft.fillScreen(BLACK);
    for (int16_t x=0; x < tft.width(); x+=6) {
        tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
        sleep(0);
    }
    for (int16_t y=0; y < tft.height(); y+=6) {
        tft.drawLine(tft.width()-1, 0, 0, y, color);
        sleep(0);
    }

    tft.fillScreen(BLACK);
    for (int16_t x=0; x < tft.width(); x+=6) {
        tft.drawLine(0, tft.height()-1, x, 0, color);
        sleep(0);
    }
    for (int16_t y=0; y < tft.height(); y+=6) {
        tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
        sleep(0);
    }

    tft.fillScreen(BLACK);
    for (int16_t x=0; x < tft.width(); x+=6) {
        tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
        sleep(0);
    }
    for (int16_t y=0; y < tft.height(); y+=6) {
        tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
        sleep(0);
    }
}

void testdrawtext(char *text, uint16_t color) {
    tft.setCursor(0, 0);
    tft.setTextColor(color);
    tft.setTextWrap(true);
    tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
    tft.fillScreen(BLACK);
    for (int16_t y=0; y < tft.height(); y+=5) {
        tft.drawFastHLine(0, y, tft.width(), color1);
    }
    for (int16_t x=0; x < tft.width(); x+=5) {
        tft.drawFastVLine(x, 0, tft.height(), color2);
    }
}

void testdrawrects(uint16_t color) {
    tft.fillScreen(BLACK);
    for (int16_t x=0; x < tft.width(); x+=6) {
        tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
    }
}

void testfillrects(uint16_t color1, uint16_t color2) {
    tft.fillScreen(BLACK);
    for (int16_t x=tft.width()-1; x > 6; x-=6) {
        tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
        tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
    }
}

void testfillcircles(uint8_t radius, uint16_t color) {
    for (int16_t x=radius; x < tft.width(); x+=radius*2) {
        for (int16_t y=radius; y < tft.height(); y+=radius*2) {
            tft.fillCircle(x, y, radius, color);
        }
    }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
    for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
        for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
            tft.drawCircle(x, y, radius, color);
        }
    }
}

void testtriangles() {
    tft.fillScreen(BLACK);
    int color = 0xF800;
    int t;
    int w = tft.width()/2;
    int x = tft.height()-1;
    int y = 0;
    int z = tft.width();
    for(t = 0 ; t <= 15; t++) {
        tft.drawTriangle(w, y, y, x, z, x, color);
        x-=4;
        y+=4;
        z-=4;
        color+=100;
    }
}

void testroundrects() {
    tft.fillScreen(BLACK);
    int color = 100;
    int i;
    int t;
    for(t = 0 ; t <= 4; t+=1) {
        int x = 0;
        int y = 0;
        int w = tft.width()-2;
        int h = tft.height()-2;
        for(i = 0 ; i <= 16; i+=1) {
            tft.drawRoundRect(x, y, w, h, 5, color);
            x+=2;
            y+=3;
            w-=4;
            h-=6;
            color+=1100;
        }
        color+=100;
    }
}

void tftPrintTest() {
    tft.setTextWrap(false);
    tft.fillScreen(BLACK);
    tft.setCursor(0, 30);
    tft.setTextColor(RED);
    tft.setTextSize(1);
    tft.println("Hello World!");
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.println("Hello World!");
    tft.setTextColor(GREEN);
    tft.setTextSize(3);
    tft.println("Hello World!");
    tft.setTextColor(BLUE);
    tft.setTextSize(4);
    tft.print(1234.567);
    sleep(1.500);
    tft.setCursor(0, 0);
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(0);
    tft.println("Hello World!");
    tft.setTextSize(1);
    tft.setTextColor(GREEN);
    tft.print(p, 6);
    tft.println(" Want pi?\n");
    tft.print(8675309, HEX); // print 8,675,309 out in HEX!
    tft.println(" Print HEX!");
    tft.println(" ");
    tft.setTextColor(WHITE);
}

void mediabuttons() {
    // play
    tft.fillScreen(BLACK);
    tft.fillRoundRect(25, 10, 78, 60, 8, WHITE);
    tft.fillTriangle(42, 20, 42, 60, 90, 40, RED);
    sleep(.500);
    // pause
    tft.fillRoundRect(25, 90, 78, 60, 8, WHITE);
    tft.fillRoundRect(39, 98, 20, 45, 5, GREEN);
    tft.fillRoundRect(69, 98, 20, 45, 5, GREEN);
    sleep(.500);
    // play color
    tft.fillTriangle(42, 20, 42, 60, 90, 40, BLUE);
    sleep(.50);
    // pause color
    tft.fillRoundRect(39, 98, 20, 45, 5, RED);
    tft.fillRoundRect(69, 98, 20, 45, 5, RED);
    // play color
    tft.fillTriangle(42, 20, 42, 60, 90, 40, GREEN);
}

int main(void) {

    tft.init();

    tft.fillScreen(WHITE);
    sleep(.500);

    // large block of text
    //tft.fillScreen(BLACK);
    testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", GREEN);
    sleep(1.000);

    // tft print function!
    tftPrintTest();
    sleep(4.000);

    // a single pixel
    tft.drawPixel(tft.width()/2, tft.height()/2, GREEN);
    sleep(.500);

    // line draw test
    testlines(YELLOW);
    sleep(.500);

    // optimized lines
    testfastlines(RED, BLUE);
    sleep(.500);

    testdrawrects(GREEN);
    sleep(.500);

    testfillrects(YELLOW, MAGENTA);
    sleep(.500);

    tft.fillScreen(BLACK);
    testfillcircles(10, BLUE);
    testdrawcircles(10, WHITE);
    sleep(.500);

    testroundrects();
    sleep(.500);

    testtriangles();
    sleep(.500);

    mediabuttons();
    sleep(.500);

    while (1);
}
