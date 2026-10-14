#include "helper.h"

#include "stm32h7xx_hal.h"
#include "main.h"
#include <stdio.h>
#include "i2c-mux.h"

#include <string.h>

#include "class/cdc/cdc_device.h"
#include "TMAG5273.h"

char buffer[100] = {0};

i2c_mux_t tca_mux_1 = {
  .hi2c = &hi2c1,
	.rst_port = GPIOI,
	.rst_pin = GPIO_PIN_3,
	.addr_offset = 0
};

i2c_mux_t tca_mux_2 = {
  .hi2c = &hi2c1,
	.rst_port = GPIOI,
	.rst_pin = GPIO_PIN_3,
	.addr_offset = 1
};

i2c_mux_t tca_mux_3 = {
  .hi2c = &hi2c1,
	.rst_port = GPIOI,
	.rst_pin = GPIO_PIN_3,
	.addr_offset = 2
};

void debugtalk(const char *msg) {
  uint16_t len = (uint16_t)strlen(msg);
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)(msg), len, HAL_MAX_DELAY);

  if (tud_cdc_connected()) {
    tud_cdc_write((uint8_t *)(msg), len);
    tud_cdc_write_flush();
  }
}

//ty https://deepbluembedded.com/stm32-i2c-scanner-hal-code-example/
int16_t probe_i2c_addr(I2C_HandleTypeDef *hi2c) {

  char Buffer[25] = {0};
  uint8_t i = 0;
  HAL_StatusTypeDef ret;

  debugtalk("i2c scan: ");

  for (i=1; i < 128; i++) {
    // remove tca muxes as they show on the list
    if (i == 0x70 || i == 0x71 || i == 0x72) {
      continue;
    }

    ret = HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(i<<1), 3, 5);

    if (ret != HAL_OK) { /* No ACK Received At That Address */
      //debugtalk(" - ");
    } else if(ret == HAL_OK) {
      debugtalk("--> ");
      sprintf(Buffer, "0x%X", i);
      debugtalk(Buffer);
      debugtalk(" <--\r\n");
      return (uint16_t)(i<<1);
    }
  }


  debugtalk("not found!\r\n");


  return (uint16_t)(0<<1); //todo missing address handle
}

uint8_t map_float_to_uint8(float x, float in_min, float in_max, uint8_t out_min,
		uint8_t out_max) {
	if (x < in_min)
		x = in_min;
	if (x > in_max)
		x = in_max;
	return (uint8_t) (
			((x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min);
}

int16_t map_float_to_int16(float x, float in_min, float in_max,
		int16_t out_min, int16_t out_max) {
	if (x < in_min)
		x = in_min;
	if (x > in_max)
		x = in_max;
	return (int16_t) (((x - in_min) * (out_max - out_min)) / (in_max - in_min)
			+ out_min);
}

uint8_t map_uint32_to_uint8(uint32_t input, uint32_t in_min, uint32_t in_max,
		uint8_t out_min, uint8_t out_max) {
	if (in_max <= in_min)
		return out_min; // Avoid division by zero or invalid range

	// Clamp input to in_min and in_max
	if (input < in_min)
		input = in_min;
	if (input > in_max)
		input = in_max;

	// Perform the mapping
	uint32_t scaled = (input - in_min) * (uint32_t) (out_max - out_min);
	uint32_t result = scaled / (in_max - in_min) + out_min;

	return (uint8_t) result;
}

int8_t map_velocity_log(uint32_t t) {
	if (t < 110) {
		return 127;
	} else if (t < 350) {
		return map_uint32_to_uint8(t, 110, 350, 81, 126);
	} else if (t < 800) {
		return map_uint32_to_uint8(t, 350, 800, 51, 81);
	} else {
		return map_uint32_to_uint8(t, 800, 10000, 30, 51);
	}
}