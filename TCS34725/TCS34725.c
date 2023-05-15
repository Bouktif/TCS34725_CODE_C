#include "TCS34725.h"

int main() 
{
    char *filename = "/dev/i2c-1";
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open the i2c bus %s\n", filename);
        exit(1);
    }
    if (ioctl(file, I2C_SLAVE, I2C_ADDRESS) < 0) {
        printf("Unable to get bus access to talk to slave.\n");
        exit(1);
    }
    
    // Enable Sensor
    Enable_sensor();
    
    //Check the enable register value
    uint8_t reg_data = get_enable_reg(TCS34725_ENABLE);
    printf("The Enable register value: 0x%02X\n", reg_data);
    
    //Get chip_id
    uint8_t chip_id = getChipID();
    printf("The chip_id of TCS34725: 0x%02X\n", chip_id);

    // Set integration time
    setIntegrationTime(0xEB);
    
    // Get Integration_time
    uint8_t time = get_Integration_time();
    float t = (256 - time)*2.4;
    int tt = round(t);
    printf("The current integration time: %dms\n", tt);

    // Set gain
    setGain(TCS34725_GAIN_1X);

    //Get gain
    uint8_t gain = getGain();

    // Check the gain value
    uint8_t data = gain & 0x03;

    switch (data) {
        case 0x00:
            printf("The RGBC GAIN VALUE = 1x gain\n");
            break;
        case 0x01:
            printf("The RGBC GAIN VALUE = 4x gain\n");
            break;
        case 0x02:
            printf("The RGBC GAIN VALUE = 16x gain\n");
            break;
        case 0x03:
            printf("The RGBC GAIN VALUE = 60x gain\n");
            break;
    }
    
    uint16_t clear_channel = read_clear_channel();
    uint16_t red_channel = read_red_channel();
    uint16_t green_channel = read_green_channel();
    uint16_t blue_channel = read_blue_channel();
    printf("Clear channel: %d \n", clear_channel);
    printf("Red channel:   %d \n", red_channel);
    printf("Green channel: %d \n", green_channel);
    printf("Blue channel:  %d \n", blue_channel);
    
    int lux = calculate_lux(red_channel, green_channel, blue_channel);
    printf("lux channel:   %d \n", lux);

    return 0;
}
