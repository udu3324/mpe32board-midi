#ifndef HELPER_H
#define HELPER_H


#include "TMAG5273.h"

/**
 * @brief Sends a message to tusb cdc serial and lpuart
 * @param msg: msg as a char array
 */
void debugtalk(const char *msg);


TMAG5273_Handle_t TMAG5273_CreateHandle(I2C_HandleTypeDef *hi2c,
		uint8_t address);

#endif