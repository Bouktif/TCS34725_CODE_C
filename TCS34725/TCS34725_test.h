#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

// adapt to your system
#define I2C_DEVICE "/dev/i2c-1" 

#define TCS34725_ADDRESS          (0x29)     /**< Default I2C address for TCS34725 */
#define TCS34725_COMMAND_BIT      (0x80)     /**< Command bit for TCS34725 register access */
#define TCS34725_ENABLE           (0x00)     /**< Enable register */
#define TCS34725_ENABLE_AIEN      (0x10)     /**< RGBC Interrupt Enable */
#define TCS34725_ENABLE_WEN       (0x08)     /**< Wait enable - Writing 1 activates the wait timer */
#define TCS34725_ENABLE_AEN       (0x02)     /**< RGBC Enable - Writing 1 actives the ADC, 0 disables it */
#define TCS34725_ENABLE_PON       (0x01)     /**< Power on - Writing 1 activates the internal oscillator, 0 disables it */
#define TCS34725_ID               (0x12)     /**< Device ID */
#define TCS34725_CDATAL           (0x14)     /**< Clear channel data */
#define TCS34725_RDATAL           (0x16)     /**< Red channel data */
#define TCS34725_GDATAL           (0x18)     /**< Green channel data */
#define TCS34725_BDATAL           (0x1A)     /**< Blue channel data */

// Integration time constants
#define TCS34725_ATIME                   0x01
#define TCS34725_INTEGRATIONTIME_2_4MS   0xFF
#define TCS34725_INTEGRATIONTIME_24MS    0xF6
#define TCS34725_INTEGRATIONTIME_50MS    0xEB
#define TCS34725_INTEGRATIONTIME_101MS   0xD5
#define TCS34725_INTEGRATIONTIME_154MS   0xC0
#define TCS34725_INTEGRATIONTIME_700MS   0x00



#define TCS34725_GAIN               0x1B
#define TCS34725_CONTROL           (0x0F)





char buf[2];
int i2c_fd;

//Get the integration time
uint8_t get_integration_time(int i2c_fd) {
    uint8_t buffer[2];

    // Read the integration time from the register
    buffer[0] = TCS34725_COMMAND_BIT | TCS34725_ATIME;
    if (write(i2c_fd, buffer, 1) != 1) {
        perror("Failed to set read address");
        exit(1);
    }
    if (read(i2c_fd, buffer, 1) != 1) {
        perror("Failed to read integration time");
        exit(1);
    }

    return buffer[0];
}


//Set the integration time
void tcs34725_set_integration_time(uint8_t integration_time) {
    uint8_t reg_data[2];
    reg_data[0] = TCS34725_COMMAND_BIT | TCS34725_ATIME;
    reg_data[1] = integration_time;
    write(i2c_fd, reg_data, 2);
}


// Get the chip ID
uint8_t get_chip_id(){
   
    uint8_t reg_data; 
    reg_data = TCS34725_ID | TCS34725_COMMAND_BIT;
    write(i2c_fd, &reg_data, 1);
    read(i2c_fd, &reg_data, 1);
    
    //printf("chip ID: %02x\n", reg_data);
    
    if (reg_data != 0x44) 
    {
        printf("Error: Invalid chip ID: %02x\n", reg_data);
        exit(1);
    }
    
    return reg_data;
    
}


void tcs34725_init()
{

    uint8_t reg_data;
    
    // Open the I2C device file
    i2c_fd = open(I2C_DEVICE, O_RDWR);
    
    if (i2c_fd < 0) 
    {
        perror("open");
        exit(1);
    }
    
    // Set the slave address to TCS34725
    if (ioctl(i2c_fd, I2C_SLAVE, TCS34725_ADDRESS) < 0) 
    {
        perror("ioctl");
        exit(1);
    }
    
    // Power on the TCS34725
    reg_data = TCS34725_ENABLE_PON;
    write(i2c_fd, &reg_data, 1);
    usleep(30000); // Wait for oscillator to stabilize
    
    // Enable the ADC
    reg_data = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
    write(i2c_fd, &reg_data, 1);

}


// Function to write data to the TCS34725 sensor
void writeData(int file, char *data) {
    if (write(file, data, 1) != 1) {
        printf("Error writing to TCS34725 sensor. \n");
        exit(1);
    }
}

uint8_t readByte(uint8_t reg) {
    uint8_t value;
    if (write(i2c_fd, &reg, 1) != 1) {
        perror("Failed to write to the I2C bus");
        exit(1);
    }
    if (read(i2c_fd, &value, 1) != 1) {
        perror("Failed to read from the I2C bus");
        exit(1);
    }
    return value;
}


// Function to read an 8-bit register
uint8_t read8(uint8_t reg) {
    uint8_t value;
    if (write(i2c_fd, &reg, 1) != 1) {
        perror("write reg");
        exit(1);
    }
    if (read(i2c_fd, &value, 1) != 1) {
        perror("read value");
        exit(1);
    }
    return value;
}

void write_byte(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {TCS34725_COMMAND_BIT | reg, value};
    if (write(i2c_fd, buf, 2) != 2) {
        printf("Error writing to I2C slave\n");
        exit(1);
    }
}

void read_bytes(uint8_t reg, uint8_t *buf, uint8_t len) {
    if (write(i2c_fd, &reg, 1) != 1) {
        printf("Error writing to I2C slave\n");
        exit(1);
    }
    if (read(i2c_fd, buf, len) != len) {
        printf("Error reading from I2C slave\n");
        exit(1);
    }
}

void enable_sensor() {
    uint8_t reg_data = TCS34725_ENABLE_PON;
    write_byte(TCS34725_ENABLE, reg_data);
    usleep(3000);
    reg_data |= TCS34725_ENABLE_AEN;
    write_byte(TCS34725_ENABLE, reg_data);
}

void read_enable_reg() {
    uint8_t reg_data;
    read_bytes(TCS34725_ENABLE | TCS34725_COMMAND_BIT, &reg_data, 1);
    printf("Enable register: 0x%02X\n", reg_data);
    if ((reg_data & TCS34725_ENABLE_AEN) && (reg_data & TCS34725_ENABLE_PON)) {
        printf("Sensor is active\n");
    } else {
        printf("Sensor is not active\n");
    }
}

