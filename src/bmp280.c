#include "bmp280.h"

#include "esp_err.h"
#include "driver/i2c_master.h"

int _bmp280_write(bmp280_handle_t bmp, uint8_t address, uint8_t* buffer, int length) {
    i2c_operation_job_t operations[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = &address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = buffer, .total_bytes = length } },
        { .command = I2C_MASTER_CMD_STOP },
    };

    if (i2c_master_execute_defined_operations(bmp->i2c, operations, sizeof(operations) / sizeof(i2c_operation_job_t), 1000) != ESP_OK) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int _bmp280_read(bmp280_handle_t bmp, uint8_t address, uint8_t* buffer, int length) {
    i2c_operation_job_t operations[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = &address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_READ, .read = { .data = buffer, .total_bytes = (length - 1), .ack_value = I2C_ACK_VAL } },
        { .command = I2C_MASTER_CMD_READ, .read = { .data = buffer + (length - 1), .total_bytes = 1, .ack_value = I2C_NACK_VAL } },
        { .command = I2C_MASTER_CMD_STOP },
    };

    if (i2c_master_execute_defined_operations(bmp->i2c, operations, sizeof(operations) / sizeof(i2c_operation_job_t), 1000) != ESP_OK) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int _bmp280_verify_id(bmp280_handle_t bmp) {
    uint8_t buffer[1];

    if (_bmp280_read(bmp, BMP280_REG_ID, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    if (buffer[0] != BMP280_VAL_ID) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int _bmp280_load_params(bmp280_handle_t bmp) {
    uint8_t buffer[26];

    if (_bmp280_read(bmp, BMP280_REG_CALIB, buffer, 26) != BMP280_OK) {
        return BMP280_ERROR;
    }

    bmp->params.t1 = (buffer[1] << 8) | buffer[0];
    bmp->params.t2 = (buffer[3] << 8) | buffer[2];
    bmp->params.t3 = (buffer[5] << 8) | buffer[4];

    bmp->params.p1 = (buffer[7] << 8) | buffer[6];
    bmp->params.p2 = (buffer[9] << 8) | buffer[8];
    bmp->params.p3 = (buffer[11] << 8) | buffer[10];
    bmp->params.p4 = (buffer[13] << 8) | buffer[12];
    bmp->params.p5 = (buffer[15] << 8) | buffer[14];
    bmp->params.p6 = (buffer[17] << 8) | buffer[16];
    bmp->params.p7 = (buffer[19] << 8) | buffer[18];
    bmp->params.p8 = (buffer[21] << 8) | buffer[20];
    bmp->params.p9 = (buffer[23] << 8) | buffer[22];

    return BMP280_OK;
}

int bmp280_init(bmp280_handle_t bmp, i2c_master_bus_handle_t i2c_bus) {
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

    if (i2c_master_bus_add_device(i2c_bus, &i2c_config, &(bmp->i2c)) != ESP_OK) {
        return BMP280_ERROR;
    }

    // Verify BMP280 id
    if (_bmp280_verify_id(bmp) != BMP280_OK) {
        return BMP280_ERROR;
    }

    // Read BMP280 calibration parameters
    _bmp280_load_params(bmp);

    // Reset BMP280
    bmp280_reset(bmp);

    return BMP280_OK;
}

int bmp280_reset(bmp280_handle_t bmp) {
    uint8_t buffer[1];

    buffer[0] = BMP280_VAL_RESET;
    return _bmp280_write(bmp, BMP280_REG_RESET, buffer, 1);
}

int bmp280_set_power_mode(bmp280_handle_t bmp, bmp280_power_mode_t power_mode) {
    uint8_t buffer[1];

    if (_bmp280_read(bmp, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    buffer[0] &= ~(0b00000011);
    buffer[0] |= power_mode;

    if (_bmp280_write(bmp, BMP280_REG_CTRL_MEAS, buffer, 1) != BMP280_OK) {
        return BMP280_ERROR;
    }

    return BMP280_OK;
}

int bmp280_set_temperature_oversampling(bmp280_handle_t bmp, bmp280_oversampling_t oversampling) {
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

int bmp280_set_pressure_oversampling(bmp280_handle_t bmp, bmp280_oversampling_t oversampling) {
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

int _bmp280_read_temperature_adc(bmp280_handle_t bmp, int32_t* t_adc) {
    uint8_t buffer[3];

    if (_bmp280_read(bmp, BMP280_REG_TEMP_MSB, buffer, 3) != BMP280_OK) {
        return BMP280_ERROR;
    }

    (*t_adc) = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);

    return BMP280_OK;
}

void _bmp280_compensate_temperature_int(int32_t t_adc, struct bmp280_params* params, int32_t* t) {
    int32_t t_var1, t_var2, t_fine;
    t_var1 = ((((t_adc>>3) - ((int32_t)(params->t1) << 1))) * ((int32_t)(params->t2))) >> 11;
    t_var2 = (((((t_adc>>4) - ((int32_t)(params->t1))) * ((t_adc>>4) - ((int32_t)(params->t1)))) >> 12) * ((int32_t)(params->t3))) >> 14;
    t_fine = t_var1 + t_var2;
    (*t) = (t_fine * 5 + 128) >> 8;
}

int bmp280_get_temperature_degC_x100_int(bmp280_handle_t bmp, int32_t* temperature) {
    int32_t t_adc;

    if (_bmp280_read_temperature_adc(bmp, &t_adc) != BMP280_OK) {
        return BMP280_ERROR;
    }

    _bmp280_compensate_temperature_int(t_adc, &(bmp->params), temperature);

    return BMP280_OK;
}

int _bmp280_read_pressure_adc(bmp280_handle_t bmp, int32_t* p_adc, int32_t* t_adc) {
    uint8_t buffer[6];

    if (_bmp280_read(bmp, BMP280_REG_PRESS_MSB, buffer, 6) != BMP280_OK) {
        return BMP280_ERROR;
    }

    (*p_adc) = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);
    (*t_adc) = (buffer[3] << 12) | (buffer[4] << 4) | (buffer[5] >> 4);

    return BMP280_OK;
}

void _bmp280_compensate_pressure_int(int32_t p_adc, int32_t t_adc, struct bmp280_params* params, uint32_t* pressure) {
    // Temperature compensation
    int32_t t_var1, t_var2, t_fine;
    t_var1 = ((((t_adc>>3) - ((int32_t)(params->t1) << 1))) * ((int32_t)(params->t2))) >> 11;
    t_var2 = (((((t_adc>>4) - ((int32_t)(params->t1))) * ((t_adc>>4) - ((int32_t)(params->t1)))) >> 12) * ((int32_t)(params->t3))) >> 14;
    t_fine = t_var1 + t_var2;

    // Pressure compensation
    int64_t p_var1, p_var2, p;
    p_var1 = ((int64_t)t_fine) - 128000;
    p_var2 = p_var1 * p_var1 * (int64_t)(params->p6);
    p_var2 = p_var2 + ((p_var1 * (int64_t)(params->p5)) << 17);
    p_var2 = p_var2 + (((int64_t)(params->p4))<<35);
    p_var1 = ((p_var1 * p_var1 * (int64_t)(params->p3))>>8) + ((p_var1 * (int64_t)(params->p2)) << 12);
    p_var1 = (((((int64_t)1) << 47) + p_var1)) * ((int64_t)(params->p1)) >> 33;

    if (p_var1 == 0) {
        (*pressure) = 0;
        return;
    }

    p = 1048576 - p_adc;
    p = (((p << 31) - p_var2) * 3125) / p_var1;
    p_var1 = (((int64_t)(params->p9)) * (p >> 13) * (p >> 13)) >> 25;
    p_var2 = (((int64_t)(params->p8)) * p) >> 19;
    p = ((p + p_var1 + p_var2) >> 8) + (((int64_t)(params->p7)) << 4);

    (*pressure) = (uint32_t)(p / 256);
}

int bmp280_get_pressure_Pa_x1_int(bmp280_handle_t bmp, uint32_t* pressure) {
    int32_t p_adc, t_adc;

    if (_bmp280_read_pressure_adc(bmp, &p_adc, &t_adc) != BMP280_OK) {
        return BMP280_ERROR;
    }

    _bmp280_compensate_pressure_int(p_adc, t_adc, &(bmp->params), pressure);

    return BMP280_OK;
}
