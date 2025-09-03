#include "bmp280.h"

#include "driver/i2c_master.h"

void bmp280_init(bmp280_t* bmp280, bmp280_config_t* config, i2c_master_bus_handle_t i2c_master) {
    i2c_device_config_t i2c_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP280_I2C_ADDRESS,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master, &i2c_config, &(bmp280->i2c_handle)));
}

