#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "driver/i2c_master.h"

#include "bmp280.h"

#define GPIO_I2C_SCL 3
#define GPIO_I2C_SDA 5

void app_main(void) {
    i2c_master_bus_config_t i2c_master_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = GPIO_I2C_SCL,
        .sda_io_num = GPIO_I2C_SDA,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t i2c_master;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_config, &i2c_master));

    bmp280_config_t bmp280_config = {
        .power_mode = BMP280_POWER_MODE_NORMAL,
        .pressure_oversampling = BMP280_OVERSAMPLING_X1,
        .temperature_oversampling = BMP280_OVERSAMPLING_X1,
    };

    bmp280_t bmp280;

    bmp280_init(&bmp280, &bmp280_config, i2c_master);

    // i2c_device_config_t i2c_device_config = {
    //     .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    //     .device_address = BMP280_I2C_ADDRESS,
    //     .scl_speed_hz = 100000,
    // };
    //
    // ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master, &i2c_device_config, &i2c_device));
    //
    // bmp280_reset();
    //
    // bmp280_set_power_mode(BMP280_POWER_MODE_NORMAL);
    //
    // bmp280_set_temperature_oversampling(BMP280_TEMPERATURE_OVERSAMPLING_X1);
    //
    // bmp280_set_pressure_oversampling(BMP280_PRESSURE_OVERSAMPLING_X1);
    //
    // while (1) {
    //     bmp280_get_temperature();
    //     bmp280_get_pressure();
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
    //
    // printf("Finished setup!\n");
}

