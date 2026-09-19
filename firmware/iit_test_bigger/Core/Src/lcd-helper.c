#include "lcd-helper.h"

#include <stdio.h>
#include "stm32h7xx_hal.h"
#include "main.h"
#include "helper.h"

//ty https://deepbluembedded.com/stm32-i2c-scanner-hal-code-example/
int16_t probe_lcd_i2c_addr() {

  char Buffer[25] = {0};
  uint8_t i = 0;
  HAL_StatusTypeDef ret;

  debugtalk("Starting I2C Scanning: \r\n");

  for (i=1; i < 128; i++) {
    ret = HAL_I2C_IsDeviceReady(&hi2c4, (uint16_t)(i<<1), 3, 5);

    if (ret != HAL_OK) { /* No ACK Received At That Address */
      debugtalk(" - ");
    } else if(ret == HAL_OK) {
      sprintf(Buffer, "0x%X", i);
      //debugtalk("found it \r\n");
      debugtalk(Buffer);
      debugtalk("Done! \r\n");
      return (uint16_t)(i<<1);
    }
  }


  debugtalk("Not found! \r\n");


  return (uint16_t)(0<<1); //todo missing address handle
}