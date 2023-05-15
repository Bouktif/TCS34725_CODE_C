#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_ADDR 0x29 // adresse I2C du capteur TCS34725
#define TCS34725_COMMAND_BIT 0x80 // bit de commande pour accéder aux registres
#define TCS34725_ENABLE 0x00 // registre d'activation du capteur
#define TCS34725_ENABLE_PON 0x01 // bit d'activation de l'alimentation
#define TCS34725_ENABLE_AEN 0x02 // bit d'activation de la mesure de la couleur

int i2c_fd; // descripteur de fichier I2C

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

int main() {
    char *i2c_device = "/dev/i2c-1";
    if ((i2c_fd = open(i2c_device, O_RDWR)) < 0) {
        printf("Error opening I2C device\n");
        return 1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, I2C_ADDR) < 0) {
        printf("Error setting I2C slave address\n");
        return 1;
    }

    enable_sensor();
    read_enable_reg();

    close(i2c_fd);
    return 0;
}
