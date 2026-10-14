#ifndef HELPER_H
#define HELPER_H


#include "TMAG5273.h"

#include "stm32h7xx_hal.h"
#include <stdint.h>

#include "i2c-mux.h"

/**
 * @brief Scans hi2c4 for all i2c device addresses for the i2c lcd screen
 * @return Returns the address fouund. (uint16_t)(0<<1) if not found
 */
int16_t probe_i2c_addr(I2C_HandleTypeDef *hi2c);

/**
 * @brief Sends a message to tusb cdc serial and lpuart
 * @param msg: msg as a char array
 */
void debugtalk(const char *msg);

/**
 * @brief Maps the time (hal_tick) to a midi key velocity
 * @param t: time duration
 */
int8_t map_velocity_log(uint32_t t);

/**
 * @brief Map a range of float to a int16 range
 * @param x: input float
 * @param in_min: input float min
 * @param in_max: input float max
 * @param out_min: output int16 min
 * @param out_max: output int16 max
 */
int16_t map_float_to_int16(float x, float in_min, float in_max, int16_t out_min, int16_t out_max);

/**
 * @brief Map a range of float to a uint8 range
 * @param x: input float
 * @param in_min: input float min
 * @param in_max: input float max
 * @param out_min: output uint8 min
 * @param out_max: output uint8 max
 */
uint8_t map_float_to_uint8(float x, float in_min, float in_max, uint8_t out_min, uint8_t out_max);

extern i2c_mux_t tca_mux_1;
extern i2c_mux_t tca_mux_2;
extern i2c_mux_t tca_mux_3;

extern char buffer[100];

#endif