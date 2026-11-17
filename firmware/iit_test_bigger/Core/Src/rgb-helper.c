#include "stm32h7xx_hal.h"
#include "main.h"

#include "neopixel32.h"
#include <stdint.h>

bool animate_led_startup = false;

NP32_Instance_t neopixel_instance_internal;
NP32_Instance_t neopixel_instance_keys;

NP32_RGB_t color_black = {0, 0, 0};
NP32_RGB_t color_red = {10, 0, 0};
NP32_RGB_t color_blue = {5, 70, 80};

//keys
int8_t Start_LED_DMA_KEYS(uint16_t *buf, uint16_t len) {
  HAL_StatusTypeDef status = HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_3, (uint32_t *)buf, len);
  return status == HAL_OK ? 0 : -1;
}

//ty https://github.com/ElisaCastellari/prova-cps-progettob
int8_t Start_LED_DMA(uint16_t *buf, uint16_t len) {
  HAL_StatusTypeDef status = HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_4, (uint32_t *)buf, len);
  return status == HAL_OK ? 0 : -1;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM8) {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
      // Avvisa la libreria che il buffer è libero
      NP32_DMAComplete_Callback(&neopixel_instance_keys);
      // Ferma il PWM
      HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_3);
    } else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
      // Avvisa la libreria che il buffer è libero
      NP32_DMAComplete_Callback(&neopixel_instance_internal);
      // Ferma il PWM
      HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_4);
    }
  }
}

NP32_RGB_t ModifyColorBrightness(NP32_RGB_t color, int8_t value) { // 0 to 100
  uint8_t R = (uint8_t) (((uint16_t) color.R * value + 50) / 100);
  uint8_t G = (uint8_t) (((uint16_t) color.G * value + 50) / 100);
  uint8_t B = (uint8_t) (((uint16_t) color.B * value + 50) / 100);
  
  NP32_RGB_t new_color = {R, G, B};
  return new_color;
}

int led_value[24];
bool led_pulse_up[24];
bool led_pulse_down[24];

void tick_internal_leds(int i, int value) {
  if ((i % 4) == 0) {
    NP32_SetLED_RGB(&neopixel_instance_internal, 3+2, ModifyColorBrightness(color_blue, value));
  } else if ((i % 4) == 1) {
    NP32_SetLED_RGB(&neopixel_instance_internal, 2+2, ModifyColorBrightness(color_blue, value));
  } else if ((i % 4) == 2) {
    NP32_SetLED_RGB(&neopixel_instance_internal, 1+2, ModifyColorBrightness(color_blue, value));
  } else {
    NP32_SetLED_RGB(&neopixel_instance_internal, 0+2, ModifyColorBrightness(color_blue, value));
  }
}

void tick_led_animation() {
  //for each led
  //pulse them
  for (int i = 0; i < 24; i++) {
    //skip since its already pulsed
    if (led_pulse_up[i] && led_pulse_down[i]) {
      continue;
    }
    if (!led_pulse_up[i]) {
      led_value[i]++;
      NP32_SetLED_RGB(&neopixel_instance_keys, i, ModifyColorBrightness(color_blue, led_value[i]));
      tick_internal_leds(i, led_value[i]);

      if (led_value[i] > 50) {
        led_pulse_up[i] = true;
      }
      return;
    } else if (!led_pulse_down[i]) {
      led_value[i]--;
      NP32_SetLED_RGB(&neopixel_instance_keys, i, ModifyColorBrightness(color_blue, led_value[i]));
      tick_internal_leds(i, led_value[i]);

      if (led_value[i] < 1) {
        led_pulse_down[i] = true;
      }
      return;
    }
  }

  //reset animation
  for (int i = 0; i < 24; i++) {
    led_pulse_up[i] = false;
    led_pulse_down[i] = false;
    led_value[i] = 0;
  }
}

void stop_led_animation() {
  if (animate_led_startup) {
    animate_led_startup = false;
    NP32_SetAllLEDs_RGB(&neopixel_instance_keys, color_black);
    NP32_SetAllLEDs_RGB(&neopixel_instance_internal, color_black);
  }
}