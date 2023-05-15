#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

// Register address
#define I2C_ADDRESS 						0x29
#define TCS34725_COMMAND_BIT 				0x80
#define TCS34725_ENABLE 					0x00
#define TCS34725_ATIME                 	 	0x01
#define TCS34725_CONTROL                	0x0F
#define TCS34725_ID            				0x12
#define TCS34725_CDATAL        				0x14
#define TCS34725_ATIME        				0x01
#define TCS34725_ENABLE_AIEN		    	0x10
#define TCS34725_ENABLE_WEN 				0x08
#define TCS34725_ENABLE_AEN 				0x02
#define TCS34725_ENABLE_PON 				0x01

// Gain Constants
#define TCS34725_GAIN_1X 					0x00
#define TCS34725_GAIN_4X 					0x01
#define TCS34725_GAIN_16X 					0x02
#define TCS34725_GAIN_60X 					0x03

// Integration time constants
#define TCS34725_INTEGRATIONTIME_2_4MS   	0xFF
#define TCS34725_INTEGRATIONTIME_24MS    	0xF6
#define TCS34725_INTEGRATIONTIME_50MS    	0xEB
#define TCS34725_INTEGRATIONTIME_101MS   	0xD5
#define TCS34725_INTEGRATIONTIME_154MS   	0xC0
#define TCS34725_INTEGRATIONTIME_700MS   	0x00

// Color register
#define TCS34725_CDATAL 					0x14
#define TCS34725_CDATAH 					0x15
#define TCS34725_RDATAL 					0x16
#define TCS34725_RDATAH 					0x17
#define TCS34725_GDATAL 					0x18
#define TCS34725_GDATAH 					0x19
#define TCS34725_BDATAL					    0x1A
#define TCS34725_BDATAH 					0x1B

int file;

void writeByte(uint8_t reg, uint8_t value) {
    uint8_t data[2];
    data[0] = reg | TCS34725_COMMAND_BIT;
    data[1] = value;
    if (write(file, data, 2) != 2) {
        printf("Error writing byte to I2C slave.\n");
        exit(1);
    }
}

uint8_t readByte(uint8_t reg) {
    uint8_t data[1];
    data[0] = reg | TCS34725_COMMAND_BIT;
    if (write(file, data, 1) != 1) {
        printf("Error writing byte to I2C slave.\n");
        exit(1);
    }
    if (read(file, data, 1) != 1) {
        printf("Error reading byte from I2C slave.\n");
        exit(1);
    }
    return data[0];
}

void setIntegrationTime(uint8_t time) {
    writeByte(TCS34725_ATIME, time);
}

uint8_t get_Integration_time() {
    return readByte(TCS34725_ATIME);
}

void setGain(uint8_t gain) {
    writeByte(TCS34725_CONTROL, gain);
}

uint8_t getGain() {
    return readByte(TCS34725_CONTROL);
}

uint8_t getChipID() {
    return readByte(TCS34725_ID);
}

void Enable_sensor()
{
    uint8_t enable_data = TCS34725_ENABLE_PON;
    writeByte(TCS34725_ENABLE, enable_data);
    usleep(3000);
    enable_data |= TCS34725_ENABLE_AEN;
    writeByte(TCS34725_ENABLE, enable_data);
}

void disable_sensor() 
{
    uint8_t enable_reg = readByte(TCS34725_ENABLE);
    enable_reg &= ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
    writeByte(TCS34725_ENABLE, enable_reg);
}

uint8_t get_enable_reg() {
    return readByte(TCS34725_ENABLE);
}

uint16_t read_color(uint8_t reg) {
    uint8_t buf[2];
    buf[0] = reg | TCS34725_COMMAND_BIT;
    if (write(file, buf, 1) != 1) {
        printf("Failed to write to the sensor. \n");
        return -1;
    }
    if (read(file, buf, 2) != 2) {
        printf("Failed to read from the sensor. \n");
        return -1;
    }
    uint16_t value = buf[1] << 8 | buf[0];
    return value;
}

uint16_t read_clear_channel() {
    uint16_t clear_channel = read_word(TCS34725_CDATAL);
    clear_channel |= read_color(TCS34725_CDATAH) << 8;
    return clear_channel;
}

uint16_t read_green_channel() {
    uint16_t green_channel = read_color(TCS34725_GDATAL);
    green_channel |= read_color(TCS34725_GDATAH) << 8;
    return green_channel;
}

uint16_t read_red_channel() {
    uint16_t red_channel = read_color(TCS34725_RDATAL);
    red_channel |= read_color(TCS34725_RDATAH) << 8;
    return red_channel;
}

uint16_t read_blue_channel() {
    uint16_t blue_channel = read_color(TCS34725_BDATAL);
    blue_channel |= read_color(TCS34725_BDATAH) << 8;
    return blue_channel;
}

int calculate_lux(uint16_t r, uint16_t g, uint16_t b) {
    
    int lux = (-0.32466 * r) + (1.57837 * g) + (-0.73191 * b);
    
    if (lux < 0) 
	{
        lux = 0;
    }
    
    return lux;
}
