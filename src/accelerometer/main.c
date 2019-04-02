#include <fpioa.h>
#include <i2c.h>
#include <sysctl.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>


/** MSA300 Registers */
#define MSA300_REG_PARTID               (0x01) ///< Part ID (R)
#define MSA300_REG_ACC_X_LSB            (0x02) ///< X-acceleration LSB (R)
#define MSA300_REG_ACC_X_MSB            (0x03) ///< X-acceleration MSB (R)
#define MSA300_REG_ACC_Y_LSB            (0x04) ///< Y-acceleration LSB (R)
#define MSA300_REG_ACC_Y_MSB            (0x05) ///< Y-acceleration MSB (R)
#define MSA300_REG_ACC_Z_LSB            (0x06) ///< Z-acceleration LSB (R)
#define MSA300_REG_ACC_Z_MSB            (0x07) ///< Z-acceleration MSB (R)
#define MSA300_REG_RES_RANGE            (0x0F) ///< Resolution/Range (R/W)
#define MSA300_REG_ODR                  (0x10) ///< Output Data Rate (R/W)
#define MSA300_REG_PWR_MODE_BW          (0x11) ///< Power Mode/Bandwidth (R/W)
#define MSA300_REG_INT_SET_0            (0x16) ///< Interrupt Set 0 (R/W)
#define MSA300_REG_INT_SET_1            (0x17) ///< Interrupt Set 1 (R/W)
#define MSA300_REG_INT_LATCH            (0x21) ///< Interrupt Latch (R/W)

#define MSA300_MG2G_MULTIPLIER_4_G      (0.000488) ///< 0.488mg per lsb
#define MSA300_DATARATE_1000_HZ         (0x0F)   ///<  500Hz Bandwidth, not available in low power mode
#define GRAVITY                         (9.80665F) ///< Gravity constant


int i2c_num = 0;

void io_mux_init(void)
{
    fpioa_set_function(30, FUNC_I2C0_SCLK);
    fpioa_set_function(31, FUNC_I2C0_SDA);
}

// read a register of the MSA300 via i2c
uint8_t read_register(uint8_t cmd) {
    uint8_t reg_val;
    int ret;

    ret = i2c_recv_data(i2c_num, &cmd, 1, &reg_val, 1);
    if (ret != 0) {
        printf("Problem: reading register %d\n", cmd);
        return -1;
    } 
    return reg_val;
}

// set a register of the MSA300 via i2c
int set_register(uint8_t cmd, uint8_t val) {
    uint8_t send_buf[2];
    int ret;

    send_buf[0] = cmd;
    send_buf[1] = val;
    ret = i2c_send_data(i2c_num, send_buf, 2);
    if (ret != 0) {
        return -1;
    }
    return 0;
}


int main(void) 
{
    uint8_t part_id;    // the device ID
    uint8_t pwr_mode;   // the power mode the device is in
    int ret;            // return status

    io_mux_init();
    plic_init();
    sysctl_enable_irq();

    // initialize the i2c: i2c device number, 7 bits data width, speed 500000
    // Note: the i2c address is 0x26. Internally the i2c software adds a 0 (0x4C, write) or a 1 (0x4D, read).
    i2c_init(i2c_num, 0x26, 7, 500000);

    // check the device ID. The MSA300 accelerometer should have ID 0x13
    uint8_t correct_id = 0x13;
    part_id = read_register(MSA300_REG_PARTID);
    if (part_id != correct_id) {
        printf("MSA device not found (ID should be %d but is %d)\n", correct_id, part_id);
    } else {
        printf("device ID is correct (%d).\n", part_id);
    }

    // set (and check) the power mode to 0x1A: normal power mode + 500Hz bandwidth
    uint8_t desired_mode = 0x1A;
    ret = set_register(MSA300_REG_PWR_MODE_BW, desired_mode);
    if (ret != 0) {
        printf("Problem: setting power mode went wrong\n");
    } else {
        printf("Setting power mode done\n");
    }
    pwr_mode = read_register(MSA300_REG_PWR_MODE_BW);
    if (pwr_mode != desired_mode) {   
        printf("Power mode should be %d but is %d\n", desired_mode, pwr_mode);
    } else {
        printf("Power mode is correct (%d).\n", pwr_mode);
    }

    // TODO: 
    //      This setting causes the output to behave strangely (wrong scaling?) I don't know why.
    //      set_register(MSA300_REG_ODR, MSA300_DATARATE_1000_HZ); // Set Output Data Rate to 1000 Hz
    // Enable x, y and z + set output data rate to 500 Hz
    set_register(MSA300_REG_ODR, 0x09); 
    // resolution 14 bits (MSB=8 bits, LSB=6 bits + 2 zero bits), range +- 4G
    set_register(MSA300_REG_RES_RANGE, 0x01);
    // no interrupts
    set_register(MSA300_REG_INT_SET_0, 0x00);
    set_register(MSA300_REG_INT_SET_1, 0x00);
    // reset all latched interrupts, temporary latched 250ms
    set_register(MSA300_REG_INT_LATCH, 0x81);


    int16_t tx, ty, tz;
    float x, y, z;

    while(1) {
        // for x, y, and z: read the LSB (6 bits + 2 zero bits) and the MSB (8 bits)
        // shift the MSB left 8 bits, add the LSB, and shift the whole two bits right
        tx = (read_register(MSA300_REG_ACC_X_LSB) + (int8_t)(read_register(MSA300_REG_ACC_X_MSB))*256)/4;
        x = MSA300_MG2G_MULTIPLIER_4_G * GRAVITY * tx;
        ty = (read_register(MSA300_REG_ACC_Y_LSB) + (int8_t)(read_register(MSA300_REG_ACC_Y_MSB))*256)/4;
        y = MSA300_MG2G_MULTIPLIER_4_G * GRAVITY * ty;
        tz = (read_register(MSA300_REG_ACC_Z_LSB) + (int8_t)(read_register(MSA300_REG_ACC_Z_MSB))*256)/4;
        z = MSA300_MG2G_MULTIPLIER_4_G * GRAVITY * tz;
        printf("x=%f y=%f z=%f (size=%f)\n", x, y, z, sqrt(x*x+y*y+z*z));
        usleep(100000);
    }
}
