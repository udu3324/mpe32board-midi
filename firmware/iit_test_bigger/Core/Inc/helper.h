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

extern i2c_mux_t tca_mux_1;
extern i2c_mux_t tca_mux_2;
extern i2c_mux_t tca_mux_3;

extern char buffer[100];

#endif