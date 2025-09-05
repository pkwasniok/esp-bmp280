#pragma once

#include "driver/i2c_master.h"

// BMP280 Datasheet p. 28
#define BMP280_I2C_ADDRESS 0x76

// BMP280 Datasheet p. 24
#define BMP280_REG_CALIB      0x88
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
#define BMP280_REG_TEMP_XLSB  0xFC

// BMP280 Datasheet p. 24
#define BMP280_VAL_ID    0x58
#define BMP280_VAL_RESET 0xB6

#define BMP280_OK    0
#define BMP280_ERROR 1

// BMP280 Datasheet p. 15
typedef enum {
    BMP280_POWER_MODE_SLEEP = 0b00,
    BMP280_POWER_MODE_FORCED = 0b01,
    BMP280_POWER_MODE_NORMAL = 0b11,
} bmp280_power_mode_t;

// BMP280 Datasheet p. 12, 13
typedef enum {
    BMP280_OVERSAMPLING_SKIPPED = 0b000,
    BMP280_OVERSAMPLING_X1 = 0b001,
    BMP280_OVERSAMPLING_X2 = 0b010,
    BMP280_OVERSAMPLING_X4 = 0b011,
    BMP280_OVERSAMPLING_X8 = 0b100,
    BMP280_OVERSAMPLING_X16 = 0b101,
} bmp280_oversampling_t;

typedef struct {
    i2c_master_dev_handle_t i2c;
    uint8_t params[26];
} bmp280_device_t;

typedef bmp280_device_t* bmp280_handle_t;

int bmp280_init(bmp280_handle_t, i2c_master_bus_handle_t);

int bmp280_reset(bmp280_handle_t);

int bmp280_set_power_mode(bmp280_handle_t, bmp280_power_mode_t);
int bmp280_set_temperature_oversampling(bmp280_handle_t, bmp280_oversampling_t);
int bmp280_set_pressure_oversampling(bmp280_handle_t, bmp280_oversampling_t);

int bmp280_get_temperature_degC_x100_int(bmp280_handle_t, int32_t*);
int bmp280_get_pressure_Pa_x1_int(bmp280_handle_t, uint32_t*);
