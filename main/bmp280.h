#pragma once

#include "driver/i2c_master.h"

#define BMP280_I2C_ADDRESS 0x76

#define BMP280_REG_ID         0xD0
#define BMP280_REG_RESET      0xE0
#define BMP280_REG_STATUS     0xF3
#define BMP280_REG_CTRL_MEAS  0xF4
#define BMP280_REG_CONFIG     0xF5
#define BMP280_REG_PRESS_MSB  0xF7
#define BMP280_REG_PRESS_LSB  0xF8
#define BMP280_REG_PRESS_XLSB 0xF9
#define BMP280_REG_TEMP_MSB   0xFA
#define BMP280_REG_TEMP_LSB   0xFB
#define BMP280_REF_TEMP_XLSB  0xFC

#define BMP280_VAL_ID    0x58
#define BMP280_VAL_RESET 0xB6

#define BMP280_VAL_POWER_MODE_SLEEP  0b00
#define BMP280_VAL_POWER_MODE_FORCED 0b01
#define BMP280_VAL_POWER_MODE_NORMAL 0b11

#define BMP280_VAL_OVERSAMPLING_SKIPPED 0b000
#define BMP280_VAL_OVERSAMPLING_X1      0b001
#define BMP280_VAL_OVERSAMPLING_X2      0b010
#define BMP280_VAL_OVERSAMPLING_X4      0b011
#define BMP280_VAL_OVERSAMPLING_X8      0b100
#define BMP280_VAL_OVERSAMPLING_X16     0b101

typedef enum {
    BMP280_POWER_MODE_SLEEP,
    BMP280_POWER_MODE_FORCED,
    BMP280_POWER_MODE_NORMAL,
} bmp280_power_mode_t;

typedef enum {
    BMP280_OVERSAMPLING_SKIPPED,
    BMP280_OVERSAMPLING_X1,
    BMP280_OVERSAMPLING_X2,
    BMP280_OVERSAMPLING_X4,
    BMP280_OVERSAMPLING_X8,
    BMP280_OVERSAMPLING_X16,
} bmp280_oversampling_t;

typedef struct {
    bmp280_power_mode_t power_mode;
    bmp280_oversampling_t temperature_oversampling;
    bmp280_oversampling_t pressure_oversampling;
} bmp280_config_t;

typedef struct {
    bmp280_config_t* config;
    i2c_master_dev_handle_t i2c_handle;
} bmp280_t;

void bmp280_init(bmp280_t*, bmp280_config_t*, i2c_master_bus_handle_t);

void bmp280_reset(bmp280_t*);

