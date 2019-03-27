A demo that shows output of the touchscreen (x, y and z1). z1 and z2 are pressure related.
It does not do calibration or scaling, it is just the raw output.

Note: at some point it stopped working when I was trying to read z2. After a lot of trying and checking the i2c library I could only get it to work again by detaching and after a while (discharging capacitors?) reattaching the LCD screen.
