#include <string>
#include "gpiohs.h"
#include "spi.h"
#include "sleep.h"
#include "fpioa.h"
#include "sysctl.h"
#include "Adafruit_GFX.h"
#include "my_st7789.h"
#include "font.h"

static lcd_ctl_t lcd_ctl;

my_ST7789::my_ST7789(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) 
{
    _height = h;
    _width = w;
}

void my_ST7789::drawPicture(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint32_t *ptr)
{
    lcd_set_area(x1, y1, x1 + width - 1, y1 + height - 1);
    tft_write_word(ptr, width * height / 2, lcd_ctl.mode ? 2 : 0);
}

void my_ST7789::drawPixel(int16_t x, int16_t y, uint16_t color) 
{
    lcd_set_area(x, y, x, y);
    tft_write_half(&color, 1);
}

void my_ST7789::lcd_set_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint8_t data[4] = {0};

    data[0] = (uint8_t)(x1 >> 8);
    data[1] = (uint8_t)(x1);
    data[2] = (uint8_t)(x2 >> 8);
    data[3] = (uint8_t)(x2);
    tft_write_command(HORIZONTAL_ADDRESS_SET);
    tft_write_byte(data, 4);

    data[0] = (uint8_t)(y1 >> 8);
    data[1] = (uint8_t)(y1);
    data[2] = (uint8_t)(y2 >> 8);
    data[3] = (uint8_t)(y2);
    tft_write_command(VERTICAL_ADDRESS_SET);
    tft_write_byte(data, 4);

    tft_write_command(MEMORY_WRITE);
}

void my_ST7789::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
{
    uint32_t data = ((uint32_t)color << 16) | (uint32_t)color;

    lcd_set_area(x, y, x+w-1, y+h-1);
    tft_set_datawidth(32);
    tft_fill_data(&data, _height * _width / 2);
}

void my_ST7789::tft_set_datawidth(uint8_t width)
{
    if (width == 32) {
        spi[SPI_CHANNEL]->ctrlr0 = 0x007F0180;
        spi[SPI_CHANNEL]->spi_ctrlr0 = 0x22;
    } else if (width == 16) {
        spi[SPI_CHANNEL]->ctrlr0 = 0x006F0180;
        spi[SPI_CHANNEL]->spi_ctrlr0 = 0x0302;
    } else {
        spi[SPI_CHANNEL]->ctrlr0 = 0x00670180;
        spi[SPI_CHANNEL]->spi_ctrlr0 = 0x0202;
    }
}

void my_ST7789::init(void) 
{
    uint8_t data = 0;

    fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);
    fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(36, FUNC_SPI0_SS3);
    fpioa_set_function(39, FUNC_SPI0_SCLK);

    sysctl_set_spi0_dvp_data(1);
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);

    tft_hard_init();
    tft_write_command(SLEEP_OFF);
    usleep(100000);
    /*pixel format*/
    tft_write_command(PIXEL_FORMAT_SET);
    data = 0x55;
    tft_write_byte(&data, 1);
    lcd_set_direction(DIR_XY_LRDU);

    /*display on*/
    tft_write_command(DISPALY_ON);
    lcd_polling_enable();
}

void my_ST7789::lcd_draw_string(uint16_t x, uint16_t y, std::string text, uint16_t color)
{
    for (int i=0; i<text.length(); ++i)
    { lcd_draw_char(x, y, text[i], color);
      x += 8;
    }
}

void my_ST7789::lcd_draw_char(uint16_t x, uint16_t y, char c, uint16_t color)
{
    uint16_t i = 0, j = 0, data = 0;

    for (i = 0; i < 16; i++)
    {   data = ascii0816[c * 16 + i];
        for (j = 0; j < 8; j++)
        {   if (data & 0x80) drawPixel(x + j, y, color);
            data <<= 1;
        }
        y++;
    }
}


static void my_ST7789::init_rst(void)
{   gpiohs_set_drive_mode(RST_GPIONUM, GPIO_DM_OUTPUT);
    gpiohs_set_pin(RST_GPIONUM, GPIO_PV_LOW);
    usleep(100000);
    gpiohs_set_pin(RST_GPIONUM, GPIO_PV_HIGH);
    usleep(100000);
}

static void  my_ST7789::init_dcx(void)
{   gpiohs_set_drive_mode(DCX_GPIONUM, GPIO_DM_OUTPUT);
    gpiohs_set_pin(DCX_GPIONUM, GPIO_PV_HIGH);
}

void my_ST7789::tft_hard_init(void)
{   init_dcx();
    /* Hardware Reset */
    init_rst();
    spi_init(SPI_CHANNEL, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    /* Change SPI clock */
    uint32_t clk = spi_set_clk_rate(SPI_CHANNEL, 20000000);
}

void my_ST7789::tft_write_command(uint8_t cmd)
{
    set_dcx_control();
    spi_init(SPI_CHANNEL, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    // channel, instrction length, address length, wait cycles, spi address trans mode
    spi_init_non_standard(SPI_CHANNEL, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
    spi_send_data_normal_dma(DMAC_CHANNEL0, SPI_CHANNEL, SPI_SLAVE_SELECT, (uint8_t *)(&cmd), 1,SPI_TRANS_CHAR);
}

void my_ST7789::tft_write_byte(uint8_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_init(SPI_CHANNEL, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    // channel, instrction length, address length, wait cycles, spi address trans mode
    spi_init_non_standard(SPI_CHANNEL, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
    spi_send_data_normal_dma(DMAC_CHANNEL0, SPI_CHANNEL, SPI_SLAVE_SELECT, data_buf, length, SPI_TRANS_CHAR);
}

void my_ST7789::tft_write_half(uint16_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_init(SPI_CHANNEL, SPI_WORK_MODE_0, SPI_FF_OCTAL, 16, 0);
    // channel, instrction length, address length, wait cycles, spi address trans mode
    spi_init_non_standard(SPI_CHANNEL, 16, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
    spi_send_data_normal_dma(DMAC_CHANNEL0, SPI_CHANNEL, SPI_SLAVE_SELECT,data_buf, length, SPI_TRANS_SHORT);
}

void my_ST7789::tft_write_word(uint32_t *data_buf, uint32_t length, uint32_t flag)
{
    set_dcx_data();
    spi_init(SPI_CHANNEL, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);
    // channel, instrction length, address length, wait cycles, spi address trans mode
    spi_init_non_standard(SPI_CHANNEL, 0, 32, 0, SPI_AITM_AS_FRAME_FORMAT);
    spi_send_data_normal_dma(DMAC_CHANNEL0, SPI_CHANNEL, SPI_SLAVE_SELECT,data_buf, length, SPI_TRANS_INT);
}

void my_ST7789::tft_fill_data(uint32_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_init(SPI_CHANNEL, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);
    // channel, instrction length, address length, wait cycles, spi address trans mode
    spi_init_non_standard(SPI_CHANNEL, 0, 32, 0, SPI_AITM_AS_FRAME_FORMAT);
    spi_fill_data_dma(DMAC_CHANNEL0, SPI_CHANNEL, SPI_SLAVE_SELECT,data_buf, length);
}

void my_ST7789::lcd_set_direction(lcd_dir_t dir)
{
    lcd_ctl.dir = dir;
    if (dir & DIR_XY_MASK)
    { lcd_ctl.width = _width - 1;
    }
    else
    { lcd_ctl.width = _height - 1;
      lcd_ctl.height = _width - 1;
    }

    tft_write_command(MEMORY_ACCESS_CTL);
    tft_write_byte((uint8_t *)&dir, 1);
}

void my_ST7789::lcd_polling_enable(void) 
{
    lcd_ctl.mode = 0;
}

void my_ST7789::lcd_interrupt_enable(void) 
{
    lcd_ctl.mode = 1;
}

static void my_ST7789::set_dcx_control(void) 
{
    gpiohs_set_pin(DCX_GPIONUM, GPIO_PV_LOW);
}

static void my_ST7789::set_dcx_data(void) 
{
    gpiohs_set_pin(DCX_GPIONUM, GPIO_PV_HIGH);
}
