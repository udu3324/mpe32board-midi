#ifndef LCD_HELPER_H
#define LCD_HELPER_H

#include "i2clcd.h"

typedef enum {
    SPLASH_BOOT,
    MAIN_MENU,
    CONFIG
} UIState;

extern I2C_LCD_HandleTypeDef lcd;

extern UIState state;

void ui_service_tick();

#endif