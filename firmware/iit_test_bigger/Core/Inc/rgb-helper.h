#ifndef RGB_HELPER_H
#define RGB_HELPER_H

int8_t Start_LED_DMA(uint16_t *buf, uint16_t len);


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);

extern NP32_Instance_t neopixel_instance_internal;

extern NP32_RGB_t color_black;
extern NP32_RGB_t color_red;
extern NP32_RGB_t color_blue;

#endif