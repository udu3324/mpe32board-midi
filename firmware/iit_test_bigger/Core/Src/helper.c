#include "helper.h"

#include "stm32h7xx_hal.h"
#include "main.h"

#include "class/cdc/cdc_device.h"

void debugtalk(char *msg) {
  uint16_t len = (uint16_t)strlen(msg);
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)(msg), len, HAL_MAX_DELAY);

  if (tud_cdc_connected()) {
    tud_cdc_write((uint8_t *)(msg), len);
    tud_cdc_write_flush();
  }
}