In this directory three experiments with VGA are provided, along with two tools: one to calculate the clock parameters and one to transform an image into RGB232:

* VGA 480 lines 640 pixels 1 bit data
* VGA 480 lines 640 pixels 7 bit data: RGB232
* VGA 768 lines 1024 pixels 7 bit data: RGB232
* Calculate PLL0 and SPI0 clock settings
* transform an image into RGB232

Note that these experiments do not explicitly use a vertical sync puls. Because it uses SPI via DVP, at the end of sending the SPI data, all signals become 0, this is enough for my monitor to think the negative vertical sync puls is given.
When looking at the different VGA modes, see e.g. [here](https://www.digikey.com/eewiki/pages/viewpage.action?pageId=15925278), and filtering for modes with negative v_sync polarity, The only two modes that come up are 640x480 and 1024x768 (and 640x350, but that didn't seem useful for me).
