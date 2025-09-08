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

// BMP280 Datasheet p. 14
typedef enum {
    BMP280_FILTER_OFF = 0b000,
    BMP280_FILTER_2 = 0b001,
    BMP280_FILTER_4 = 0b010,
    BMP280_FILTER_8 = 0b011,
    BMP280_FILTER_16 = 0b100,
} bmp280_filter_t;

// BMP280 Datasheet p. 17
typedef enum {
    BMP280_STANDBY_1_MS = 0b000,
    BMP280_STANDBY_63_MS = 0b001,
    BMP280_STANDBY_125_MS = 0b010,
    BMP280_STANDBY_250_MS = 0b011,
    BMP280_STANDBY_500_MS = 0b100,
    BMP280_STANDBY_1000_MS = 0b101,
    BMP280_STANDBY_2000_MS = 0b110,
    BMP280_STANDBY_4000_MS = 0b111,
} bmp280_standby_t;

// BMP280 Datasheet p. 21
typedef struct {
    uint16_t t1;
    int16_t t2;
    int16_t t3;
    uint16_t p1;
    int16_t p2;
    int16_t p3;
    int16_t p4;
    int16_t p5;
    int16_t p6;
    int16_t p7;
    int16_t p8;
    int16_t p9;
} bmp280_params_t;

typedef struct {
    bmp280_standby_t standby_time;
    bmp280_oversampling_t temperature_oversampling;
    bmp280_oversampling_t pressure_oversampling;
    bmp280_filter_t filter;
} bmp280_config_t;

typedef struct {
    i2c_master_dev_handle_t i2c;
    bmp280_params_t params;
} bmp280_device_t;

typedef bmp280_device_t* bmp280_handle_t;

int bmp280_init(bmp280_handle_t, bmp280_config_t*, i2c_master_bus_handle_t);

int bmp280_reset(bmp280_handle_t);
int bmp280_set_config(bmp280_handle_t, bmp280_config_t*);
int bmp280_set_power_mode(bmp280_handle_t, bmp280_power_mode_t);

int bmp280_get_temperature_degC_x100_int(bmp280_handle_t, int32_t*);
int bmp280_get_pressure_Pa_x1_int(bmp280_handle_t, uint32_t*);
