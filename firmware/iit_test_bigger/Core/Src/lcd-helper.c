#include <stdbool.h>

#include "lcd-helper.h"
#include "i2clcd.h"
#include "stm32h7xx_hal.h"

bool trigger_main_screen = false;

char status_string[16] = "";

I2C_LCD_HandleTypeDef lcd;

void render_main_menu() {
    lcd_clear(&lcd);
    //lcd_gotoxy(&lcd, 0, 0);
    //lcd_puts(&lcd, "mpe32board");
    lcd_gotoxy(&lcd, 0, 1);
    lcd_puts(&lcd, "o-  o+  mcro cfg");

}

void render_btns() {
    //lcd_puts(&lcd, "");
}

void debug() {

}

UIState state = SPLASH_BOOT;
UIState current_state = SPLASH_BOOT;
uint32_t ptick = 0;
void ui_service_tick() {

    // status text

    if (status_string[0] != '\0') {
        ptick = HAL_GetTick() + 2000;

        lcd_gotoxy(&lcd, 0, 0);
        lcd_puts(&lcd, "                ");
        lcd_gotoxy(&lcd, 0, 0);
        lcd_puts(&lcd, status_string);

        status_string[0] = '\0';
    }

    if (ptick != 0) {
        if (HAL_GetTick() > ptick) {
            lcd_gotoxy(&lcd, 0, 0);
            lcd_puts(&lcd, "                ");

            ptick = 0;
        }
    }

    // state switching 

    if (current_state == state) {
        return;
    }

    switch (state) {
        case SPLASH_BOOT:

            break;
        case MAIN_MENU:
            render_main_menu();
            break;
        case CONFIG:

            break;
    }

    current_state = state;
}