#ifndef LCD_HELPER_H
#define LCD_HELPER_H

#include <stdint.h>

/**
 * @brief Scans hi2c4 for all i2c device addresses for the i2c lcd screen
 * @return Returns the address fouund. (uint16_t)(0<<1) if not found
 */
int16_t probe_lcd_i2c_addr();


#endif