#include "stm32h7xx_hal.h"
#include "main.h"

#include "neopixel32.h"

NP32_Instance_t neopixel_instance_internal;

NP32_RGB_t color_black = {0, 0, 0};
NP32_RGB_t color_red = {10, 0, 0};
NP32_RGB_t color_blue = {0, 0, 10};

//ty https://github.com/ElisaCastellari/prova-cps-progettob
int8_t Start_LED_DMA(uint16_t *buf, uint16_t len) {
  HAL_StatusTypeDef status = HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_4, (uint32_t *)buf, len);
  return status == HAL_OK ? 0 : -1;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM8) {
    // Avvisa la libreria che il buffer è libero
    NP32_DMAComplete_Callback(&neopixel_instance_internal);
    // Ferma il PWM
    HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_4);
  }
}