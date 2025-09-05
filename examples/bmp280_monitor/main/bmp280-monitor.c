#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "bmp280.h"

#define GPIO_I2C_SCL 3
#define GPIO_I2C_SDA 5

void app_main(void) {
    i2c_master_bus_config_t i2c_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = GPIO_I2C_SCL,
        .sda_io_num = GPIO_I2C_SDA,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t i2c_bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_config, &i2c_bus));

    bmp280_device_t bmp;
    ESP_ERROR_CHECK(bmp280_init(&bmp, i2c_bus));

    bmp280_set_power_mode(&bmp, BMP280_POWER_MODE_NORMAL);
    bmp280_set_temperature_oversampling(&bmp, BMP280_OVERSAMPLING_X1);
    bmp280_set_pressure_oversampling(&bmp, BMP280_OVERSAMPLING_X4);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        int32_t temperature;
        bmp280_get_temperature_degC_x100_int(&bmp, &temperature);

        uint32_t pressure;
        bmp280_get_pressure_Pa_x1_int(&bmp, &pressure);

        printf("Temperature: %.2f degC\n", temperature / 100.0);
        printf("Pressure: %.2f hPa\n", pressure / 100.0);
        printf("\n");
    }
}
