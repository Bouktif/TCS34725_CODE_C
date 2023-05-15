#include"TCS34725_test.h"


int main() 
{    
    //Enable TCS34725
    tcs34725_init();

    // Set integration time
    tcs34725_set_integration_time(TCS34725_INTEGRATIONTIME_700MS);
    
    
    // Get the current integration time
    uint8_t integration_time = get_integration_time(i2c_fd);
    
    float t = (256 - integration_time)*2.4;
    
    int tt = round(t);
    
    printf("The current integration time: %dms\n", tt);
    
    //Get chip_id of TCS34725
    uint8_t id = get_chip_id();
    
    printf("The chip_id of TCS34725: %02x\n", id);
    
     // Enable the sensor
    enable_sensor();
    
    
    
    
    // Read the ENABLE register
    uint8_t read_enable_reg = read_bytes(TCS34725_ENABLE | TCS34725_COMMAND_BIT, &reg_data, 1);

    // Check if the sensor is active
    if ((read_enable_reg & 0x03) == 0x03) {
        printf("TCS34725 sensor is active\n");
    } else {
        printf("TCS34725 sensor is not active\n");
    }
    return 0;
}
