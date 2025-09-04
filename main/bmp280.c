#include "bmp280.h"
#include <stdint.h>

#include "esp_err.h"
#include "driver/i2c_master.h"

int _bmp280_write(bmp280_t* bmp280, uint8_t address, uint8_t* buffer, int length) {
    i2c_operation_job_t operations[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = &address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = buffer, .total_bytes = length } },
        { .command = I2C_MASTER_CMD_STOP },
    };

    if (i2c_master_execute_defined_operations(bmp280->i2c_device, operations, sizeof(operations) / sizeof(i2c_operation_job_t), 1000) != ESP_OK) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int _bmp280_read(bmp280_t* bmp280, uint8_t address, uint8_t* buffer, int length) {
    i2c_operation_job_t operations[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = &address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_READ, .read = { .data = buffer, .total_bytes = (length - 1), .ack_value = I2C_ACK_VAL } },
        { .command = I2C_MASTER_CMD_READ, .read = { .data = buffer + (length - 1), .total_bytes = 1, .ack_value = I2C_NACK_VAL } },
        { .command = I2C_MASTER_CMD_STOP },
    };

    if (i2c_master_execute_defined_operations(bmp280->i2c_device, operations, sizeof(operations) / sizeof(i2c_operation_job_t), 1000) != ESP_OK) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int _bmp280_verify_id(bmp280_t* bmp280) {
    uint8_t buffer[1];

    if (_bmp280_read(bmp280, BMP280_REG_ID, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    if (buffer[0] != BMP280_VAL_ID) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int bmp280_init(bmp280_t* bmp280, i2c_master_bus_handle_t i2c_bus) {
    // Check if device with BMP280 address is connected to bus
    if (i2c_master_probe(i2c_bus, BMP280_I2C_ADDRESS, 1000) != ESP_OK) {
        return BMP280_ERROR;
    }

    // Create I2C device
    i2c_device_config_t i2c_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP280_I2C_ADDRESS,
        .scl_speed_hz = 100000,
    };

    if (i2c_master_bus_add_device(i2c_bus, &i2c_config, &(bmp280->i2c_device)) != ESP_OK) {
        return BMP280_ERROR;
    }

    // Verify BMP280 id
    if (_bmp280_verify_id(bmp280) != BMP280_OK) {
        return BMP280_ERROR;
    }

    // Reset BMP280
    bmp280_reset(bmp280);

    // Read BMP280 calibration parameters
    for (int i = 0; i < 2; i++) {
        if (_bmp280_read(bmp280, 0x88, bmp280->params, 26) != BMP280_OK) {
            return BMP280_ERROR;
        }
    }

    return BMP280_OK;
}

int bmp280_reset(bmp280_t* bmp280) {
    uint8_t buffer[1];

    buffer[0] = BMP280_VAL_RESET;
    return _bmp280_write(bmp280, BMP280_REG_RESET, buffer, 1);
}

int bmp280_set_power_mode(bmp280_t* bmp280, bmp280_power_mode_t power_mode) {
    uint8_t buffer[1];

    if (_bmp280_read(bmp280, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    buffer[0] &= ~(0b00000011);
    buffer[0] |= power_mode;

    if (_bmp280_write(bmp280, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int bmp280_set_temperature_oversampling(bmp280_t* bmp, bmp280_oversampling_t oversampling) {
    uint8_t buffer[1];

    if (_bmp280_read(bmp, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    buffer[0] &= ~(0b11100000);
    buffer[0] |= (oversampling << 5);

    if (_bmp280_write(bmp, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int bmp280_set_pressure_oversampling(bmp280_t* bmp, bmp280_oversampling_t oversampling) {
    uint8_t buffer[1];

    if (_bmp280_read(bmp, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    buffer[0] &= ~(0b00011100);
    buffer[0] |= (oversampling << 2);

    if (_bmp280_write(bmp, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int _bmp280_read_temperature_adc(bmp280_t* bmp, int32_t* t_adc) {
    uint8_t buffer[3];

    if (_bmp280_read(bmp, BMP280_REG_TEMP_MSB, buffer, 3) != BMP280_OK) {
        return BMP280_ERROR;
    }

    (*t_adc) = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);

    return BMP280_OK;
}

void _bmp280_compensate_temperature_int(int32_t t_adc, uint8_t* params, int32_t* t) {
    // Temperature params
    uint16_t dig_t1 = (params[1] << 8) | params[0];
    int16_t dig_t2 = (params[3] << 8) | params[2];
    int16_t dig_t3 = (params[5] << 8) | params[4];

    // Temperature compensation
    int32_t t_var1, t_var2, t_fine;
    t_var1 = ((((t_adc>>3) - ((int32_t)dig_t1<<1))) * ((int32_t)dig_t2)) >> 11;
    t_var2 = (((((t_adc>>4) - ((int32_t)dig_t1)) * ((t_adc>>4) - ((int32_t)dig_t1))) >> 12) * ((int32_t)dig_t3)) >> 14;
    t_fine = t_var1 + t_var2;
    (*t) = (t_fine * 5 + 128) >> 8;
}

int bmp280_get_temperature_int(bmp280_t* bmp, int32_t* temperature) {
    int32_t t_adc;

    if (_bmp280_read_temperature_adc(bmp, &t_adc) != BMP280_OK) {
        return BMP280_ERROR;
    }

    _bmp280_compensate_temperature_int(t_adc, bmp->params, temperature);

    return BMP280_OK;
}
