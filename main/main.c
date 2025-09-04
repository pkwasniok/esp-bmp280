#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "driver/i2c_master.h"

#include "bmp280.h"

#define GPIO_I2C_SCL 3
#define GPIO_I2C_SDA 5

void app_main(void) {

    // Initialize I2C bus

    i2c_master_bus_config_t i2c_master_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = GPIO_I2C_SCL,
        .sda_io_num = GPIO_I2C_SDA,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t i2c_master;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_config, &i2c_master));

    bmp280_t bmp280;

    if (bmp280_init(&bmp280, i2c_master) != BMP280_OK) {
        printf("BMP280 Error!\n");
    }

    bmp280_set_power_mode(&bmp280, BMP280_POWER_MODE_NORMAL);
    bmp280_set_temperature_oversampling(&bmp280, BMP280_OVERSAMPLING_X1);
    bmp280_set_pressure_oversampling(&bmp280, BMP280_OVERSAMPLING_X4);

    int32_t temperature;
    uint32_t pressure;

    while (1) {
        bmp280_get_temperature_int(&bmp280, &temperature);
        bmp280_get_pressure_int(&bmp280, &pressure);

        printf("Temperature: %.2f degC\n", temperature / 100.0);
        printf("Pressure: %.2f hPa\n", pressure / 100.0);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
