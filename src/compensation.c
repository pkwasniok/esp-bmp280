#include "bmp280.h"

int32_t _bmp280_compensate_temperature(bmp280_params_t* params, int32_t t_adc) {
    int32_t t_var1, t_var2, t;
    t_var1 = ((((t_adc>>3) - ((int32_t)(params->t1) << 1))) * ((int32_t)(params->t2))) >> 11;
    t_var2 = (((((t_adc>>4) - ((int32_t)(params->t1))) * ((t_adc>>4) - ((int32_t)(params->t1)))) >> 12) * ((int32_t)(params->t3))) >> 14;
    t = t_var1 + t_var2;
    t = ((t * 5 + 128) >> 8);

    return t;
}

uint32_t _bmp280_compensate_pressure(bmp280_params_t* params, int32_t p_adc, int32_t t_adc) {
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
        p = 0;
    } else {
        p = 1048576 - p_adc;
        p = (((p << 31) - p_var2) * 3125) / p_var1;
        p_var1 = (((int64_t)(params->p9)) * (p >> 13) * (p >> 13)) >> 25;
        p_var2 = (((int64_t)(params->p8)) * p) >> 19;
        p = ((p + p_var1 + p_var2) >> 8) + (((int64_t)(params->p7)) << 4);
        p = p / 256;
    }

    return (uint32_t)p;
}
