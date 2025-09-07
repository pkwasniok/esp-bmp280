#include "bmp280.h"

int _bmp280_write(bmp280_handle_t device, uint8_t address, uint8_t* buffer, int length) {
    i2c_operation_job_t operations[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = &address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = buffer, .total_bytes = length } },
        { .command = I2C_MASTER_CMD_STOP },
    };

    if (i2c_master_execute_defined_operations(device->i2c, operations, sizeof(operations) / sizeof(i2c_operation_job_t), 1000) != ESP_OK)
        return BMP280_ERROR;

    return BMP280_OK;
}

int _bmp280_read(bmp280_handle_t device, uint8_t address, uint8_t* buffer, int length) {
    i2c_operation_job_t operations[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .data = &address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_READ, .read = { .data = buffer, .total_bytes = (length - 1), .ack_value = I2C_ACK_VAL } },
        { .command = I2C_MASTER_CMD_READ, .read = { .data = buffer + (length - 1), .total_bytes = 1, .ack_value = I2C_NACK_VAL } },
        { .command = I2C_MASTER_CMD_STOP },
    };

    if (i2c_master_execute_defined_operations(device->i2c, operations, sizeof(operations) / sizeof(i2c_operation_job_t), 1000) != ESP_OK)
        return BMP280_ERROR;

    return BMP280_OK;
}
