#include "bmp280.h"

extern int _bmp280_write(bmp280_handle_t, uint8_t, uint8_t*, int);
extern int _bmp280_read(bmp280_handle_t, uint8_t, uint8_t*, int);
extern int32_t _bmp280_compensate_temperature(bmp280_params_t*, int32_t);
extern uint32_t _bmp280_compensate_pressure(bmp280_params_t*, int32_t, int32_t);

int _bmp280_read_adc(bmp280_handle_t device, int32_t* p_adc, int32_t* t_adc) {
    uint8_t buffer[6];

    if (_bmp280_read(device, BMP280_REG_PRESS_MSB, buffer, 6) != BMP280_OK)
        return BMP280_ERROR;

    (*p_adc) = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);
    (*t_adc) = (buffer[3] << 12) | (buffer[4] << 4) | (buffer[5] >> 4);

    return BMP280_OK;
}

int bmp280_get_temperature_degC_x100_int(bmp280_handle_t device, int32_t* temperature) {
    int32_t p_adc, t_adc;

    if (_bmp280_read_adc(device, &p_adc, &t_adc) != BMP280_OK)
        return BMP280_ERROR;

    (*temperature) = _bmp280_compensate_temperature(&device->params, t_adc);

    return BMP280_OK;
}

int bmp280_get_pressure_Pa_x1_int(bmp280_handle_t device, uint32_t* pressure) {
    int32_t p_adc, t_adc;

    if (_bmp280_read_adc(device, &p_adc, &t_adc) != BMP280_OK)
        return BMP280_ERROR;

    (*pressure) = _bmp280_compensate_pressure(&device->params, p_adc, t_adc);

    return BMP280_OK;
}
