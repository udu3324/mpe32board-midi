#include "helper.h"

#include "stm32h7xx_hal.h"
#include "main.h"

#include <string.h>

#include "class/cdc/cdc_device.h"
#include "TMAG5273.h"

void debugtalk(const char *msg) {
  uint16_t len = (uint16_t)strlen(msg);
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)(msg), len, HAL_MAX_DELAY);

  if (tud_cdc_connected()) {
    tud_cdc_write((uint8_t *)(msg), len);
    tud_cdc_write_flush();
  }
}

TMAG5273_Handle_t TMAG5273_CreateHandle(I2C_HandleTypeDef *hi2c,
		uint8_t address) {
	TMAG5273_Handle_t tmag = { .pI2c = hi2c, .address = address,
			.magTempcoMode = TMAG5273_NO_MAG_TEMPCO, .convAvgMode =
					TMAG5273_CONV_AVG_32X, .readMode =
					TMAG5273_READ_MODE_STANDARD, .lplnMode = TMAG5273_LOW_NOISE,
			.operatingMode = TMAG5273_OPERATING_MODE_STANDBY, .magXYRange =
					TMAG5273_MAG_RANGE_40MT_133MT, .magZRange =
					TMAG5273_MAG_RANGE_40MT_133MT, .magXYRange =
					TMAG5273_MAG_RANGE_40MT_133MT, .magZRange =
					TMAG5273_MAG_RANGE_80MT_266MT, .tempChEn =
					TMAG5273_TEMP_CH_DISABLED, .angEn = TMAG5273_ANG_X_Z,
			.magChEn = TMAG5276_MAG_Z_X, .crcEna = TMAG5273_CRC_DISABLE,
			.sensor_id = 0, .sleep = TMAG5276_10MS };

	return tmag;
}