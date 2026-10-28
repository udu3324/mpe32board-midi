#include <stdbool.h>

#include "lcd-helper.h"
#include "i2clcd.h"

bool trigger_main_screen = false;

I2C_LCD_HandleTypeDef lcd;

void render_main_menu() {
    lcd_clear(&lcd);
    lcd_gotoxy(&lcd, 0, 0);
    lcd_puts(&lcd, "mpe32board");
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
void ui_service_tick() {
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