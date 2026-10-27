#include "stm32h7xx_hal.h"

#include "helper.h"
#include "neopixel32.h"
#include "rgb-helper.h"
#include "i2clcd.h"
#include "lcd-helper.h"
#include "tmag-helper.h"


bool runOneshot[4] = { false, false, false, false };

void button_debug_handle() {  
  //PG6_BTN1_INT
  if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6) && runOneshot[0]) {
    runOneshot[0] = false;
    NP32_SetLED_RGB(&neopixel_instance_internal, 3+2, color_blue);
    //Init_TMAGS();
    lcd_clear(&lcd);
    lcd_puts(&lcd, "Hello, LCD 1!");
    debugtalk("1");
  } else if (!HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6) && !runOneshot[0]) {
    runOneshot[0] = true;
    NP32_SetLED_RGB(&neopixel_instance_internal, 3+2, color_black);
  }

  //PG7_BTN2_INT
  if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_7) && runOneshot[1]) {
    runOneshot[1] = false;
    NP32_SetLED_RGB(&neopixel_instance_internal, 2+2, color_blue);

    PrintEnabledTmags();

    lcd_clear(&lcd);
    lcd_puts(&lcd, "aaaaaaaaaaaaaaaaaaaa");
    debugtalk("2");
  } else if (!HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_7) && !runOneshot[1]) {
    runOneshot[1] = true; 
    NP32_SetLED_RGB(&neopixel_instance_internal, 2+2, color_black);
  }

  //PC7_BTN3_INT
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) && runOneshot[2]) {
    runOneshot[2] = false;
    NP32_SetLED_RGB(&neopixel_instance_internal, 1+2, color_blue);
    lcd_clear(&lcd);
    lcd_puts(&lcd, "ok bro ");
    debugtalk("3");
  } else if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) && !runOneshot[2]) {
    runOneshot[2] = true;
    NP32_SetLED_RGB(&neopixel_instance_internal, 1+2, color_black);
  }

  //PA8_BTN4_INT
  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && runOneshot[3]) {
    runOneshot[3] = false;
    NP32_SetLED_RGB(&neopixel_instance_internal, 0+2, color_blue);
    lcd_clear(&lcd);
    lcd_puts(&lcd, "its not fair");
    debugtalk("4");
  } else if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && !runOneshot[3]) {
    runOneshot[3] = true;
    NP32_SetLED_RGB(&neopixel_instance_internal, 0+2, color_black);
  }
}

void button_handle() {
  //PG6_BTN1_INT
  if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6) && runOneshot[0]) {
    runOneshot[0] = false;

  } else if (!HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6) && !runOneshot[0]) {
    runOneshot[0] = true;
  }

  //PG7_BTN2_INT
  if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_7) && runOneshot[1]) {
    runOneshot[1] = false;
  
  } else if (!HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_7) && !runOneshot[1]) {
    runOneshot[1] = true;
  }

  //PC7_BTN3_INT
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) && runOneshot[2]) {
    runOneshot[2] = false;

  } else if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) && !runOneshot[2]) {
    runOneshot[2] = true;
  }

  //PA8_BTN4_INT
  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && runOneshot[3]) {
    runOneshot[3] = false;

  } else if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && !runOneshot[3]) {
    runOneshot[3] = true;
  }
}