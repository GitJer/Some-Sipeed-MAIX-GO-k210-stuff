#include <fpioa.h>
#include <i2c.h>
#include <sysctl.h>
#include <stdio.h>


int i2c_num = 0;

void io_mux_init(void)
{
    fpioa_set_function(30, FUNC_I2C0_SCLK);
    fpioa_set_function(31, FUNC_I2C0_SDA);
}

static uint8_t read(uint8_t cmd, int *val)
{
    uint8_t ret, buf[2];
    ret = i2c_recv_data(i2c_num, &cmd, 1, buf, 2);
    if (ret != 0) return 0;
    if (val) *val = (buf[0] << 4) | (buf[1] >> 4);
    return 1;
}

#define LOW_POWER_READ_X 0xc0
#define LOW_POWER_READ_Y 0xd0
#define LOW_POWER_READ_Z1 0xe0
#define LOW_POWER_READ_Z2 0xf0


int main(void) 
{
    io_mux_init();
    plic_init();
    sysctl_enable_irq();
    i2c_init(i2c_num, 0x48, 7, 400000);

    int x = 0, y = 0, z1 = 0; 

    while(1) {
        if (read(LOW_POWER_READ_Z1, &z1))
        { 
            if ((z1 > 70) && (z1 < 2000))
            { 
                read(LOW_POWER_READ_X, &x);
                read(LOW_POWER_READ_Y, &y);
                printf("x=%d y=%d z1=%d\n",x, y, z1);
            }
        }
    }
}
