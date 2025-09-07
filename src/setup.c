#include "bmp280.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "driver/i2c_master.h"

extern int _bmp280_write(bmp280_handle_t, uint8_t, uint8_t*, int);
extern int _bmp280_read(bmp280_handle_t, uint8_t, uint8_t*, int);

int _bmp280_verify_id(bmp280_handle_t device) {
    uint8_t buffer[1];

    if (_bmp280_read(device, BMP280_REG_ID, buffer, 1) != BMP280_OK)
        return BMP280_ERROR;

    if (buffer[0] != BMP280_VAL_ID)
        return BMP280_ERROR;

    return BMP280_OK;
}

int _bmp280_read_params(bmp280_handle_t device, bmp280_params_t* params) {
    uint8_t buffer[26];

    if (_bmp280_read(device, BMP280_REG_CALIB, buffer, 26) != BMP280_OK)
        return BMP280_ERROR;

    params->t1 = (buffer[1] << 8) | buffer[0];
    params->t2 = (buffer[3] << 8) | buffer[2];
    params->t3 = (buffer[5] << 8) | buffer[4];

    params->p1 = (buffer[7] << 8) | buffer[6];
    params->p2 = (buffer[9] << 8) | buffer[8];
    params->p3 = (buffer[11] << 8) | buffer[10];
    params->p4 = (buffer[13] << 8) | buffer[12];
    params->p5 = (buffer[15] << 8) | buffer[14];
    params->p6 = (buffer[17] << 8) | buffer[16];
    params->p7 = (buffer[19] << 8) | buffer[18];
    params->p8 = (buffer[21] << 8) | buffer[20];
    params->p9 = (buffer[23] << 8) | buffer[22];

    return BMP280_OK;
}

int bmp280_init(bmp280_handle_t device, bmp280_config_t* config, i2c_master_bus_handle_t i2c_bus) {
    if (i2c_master_probe(i2c_bus, BMP280_I2C_ADDRESS, 1000) != ESP_OK)
        return BMP280_ERROR;

    i2c_device_config_t i2c_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP280_I2C_ADDRESS,
        .scl_speed_hz = 100000,
    };

    if (i2c_master_bus_add_device(i2c_bus, &i2c_config, &(device->i2c)) != ESP_OK)
        return BMP280_ERROR;

    if (_bmp280_verify_id(device) != BMP280_OK)
        return BMP280_ERROR;

    if (bmp280_reset(device) != BMP280_OK)
        return BMP280_ERROR;

    vTaskDelay(100 / portTICK_PERIOD_MS);

    if (_bmp280_read_params(device, &device->params) != BMP280_OK)
        return BMP280_ERROR;

    if (bmp280_set_config(device, config) != BMP280_OK)
        return BMP280_ERROR;

    return BMP280_OK;
}

int bmp280_reset(bmp280_handle_t device) {
    uint8_t buffer[1];

    buffer[0] = BMP280_VAL_RESET;

    if (_bmp280_write(device, BMP280_REG_RESET, buffer, 1) != BMP280_OK)
        return BMP280_ERROR;

    return BMP280_OK;
}

int bmp280_set_config(bmp280_handle_t device, bmp280_config_t* config) {
    uint8_t buffer[1];

    buffer[0] = (config->temperature_oversampling << 5) | (config->pressure_oversampling << 2) | config->power_mode;

    if (_bmp280_write(device, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK)
        return BMP280_ERROR;

    return BMP280_OK;
}
