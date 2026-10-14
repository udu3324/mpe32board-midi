#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "helper.h"
#include "i2c-mux.h"

TMAG5273_Handle_t tmag_handles[25];
bool enabled_tmags[25];
uint8_t tmag_addr_arr[25] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
		0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22,
		0x23,
		0x24, 0x25, 0x26, 0x27, 0x28 }; 

bool storedInitialData = false;

float tmag_pos[25];
float tmag_rot[25];

float tmag_pos_init[25];
float tmag_rot_init[25];

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

void Init_TMAGS() {
  //mux1
  for (int i = 0; i < 8; i++) {
    snprintf(buffer, sizeof(buffer), "working on mux1:%u\r\n", i);
    debugtalk(buffer);

    if (i2c_mux_select(&tca_mux_1, i) != 0) {
      debugtalk("TMAG mux channel 1 select failed\r\n");
      Error_Handler();
    }

    uint16_t tmag_address = probe_i2c_addr(&hi2c1);
    HAL_StatusTypeDef ret = tmag_address == 0
      ? HAL_ERROR
      : HAL_I2C_IsDeviceReady(&hi2c1, tmag_address, 3, 5);
    
    if (ret != HAL_OK) {
      //skip channel as the sensor may not be plugged in
      snprintf(buffer, sizeof(buffer), "skip mux1:%u\r\n", i);
      debugtalk(buffer);
    } else {
      tmag_handles[i] = TMAG5273_CreateHandle(&hi2c1, (uint8_t)(tmag_address >> 1));
      TMAG5273_Init(&tmag_handles[i]);
      TMAG5273_RewriteI2CAddress(&tmag_handles[i], tmag_addr_arr[i]);

      snprintf(buffer, sizeof(buffer), "init mux1:%u\r\n", i);
      debugtalk(buffer);
    }
    enabled_tmags[i] = (ret == HAL_OK);
  }

  //mux2
  for (int i = 0; i < 8; i++) {
    snprintf(buffer, sizeof(buffer), "working on mux2:%u\r\n", i);
    debugtalk(buffer);

    if (i2c_mux_select(&tca_mux_2, i) != 0) {
      debugtalk("TMAG mux channel 2 select failed\r\n");
      Error_Handler();
    }

    uint16_t tmag_address = probe_i2c_addr(&hi2c1);
    HAL_StatusTypeDef ret = tmag_address == 0
      ? HAL_ERROR
      : HAL_I2C_IsDeviceReady(&hi2c1, tmag_address, 3, 5);
    
    if (ret != HAL_OK) {
      //skip channel as the sensor may not be plugged in
      snprintf(buffer, sizeof(buffer), "skip mux2:%u\r\n", i);
      debugtalk(buffer);
    } else {
      tmag_handles[i + 8] = TMAG5273_CreateHandle(&hi2c1, (uint8_t)(tmag_address >> 1));
      TMAG5273_Init(&tmag_handles[i + 8]);
      TMAG5273_RewriteI2CAddress(&tmag_handles[i + 8], tmag_addr_arr[i + 8]);

      snprintf(buffer, sizeof(buffer), "init mux2:%u\r\n", i);
      debugtalk(buffer);
    }
    enabled_tmags[i + 8] = (ret == HAL_OK);
  }

  //mux3
  for (int i = 0; i < 8; i++) {
    snprintf(buffer, sizeof(buffer), "working on mux3:%u\r\n", i);
    debugtalk(buffer);

    if (i2c_mux_select(&tca_mux_3, i) != 0) {
      debugtalk("TMAG mux channel 3 select failed\r\n");
      Error_Handler();
    }

    uint16_t tmag_address = probe_i2c_addr(&hi2c1);
    HAL_StatusTypeDef ret = tmag_address == 0
      ? HAL_ERROR
      : HAL_I2C_IsDeviceReady(&hi2c1, tmag_address, 3, 5);
    
    if (ret != HAL_OK) {
      //skip channel as the sensor may not be plugged in
      snprintf(buffer, sizeof(buffer), "skip mux3:%u\r\n", i);
      debugtalk(buffer);
    } else {
      tmag_handles[i + 16] = TMAG5273_CreateHandle(&hi2c1, (uint8_t)(tmag_address >> 1));
      TMAG5273_Init(&tmag_handles[i + 16]);
      TMAG5273_RewriteI2CAddress(&tmag_handles[i + 16], tmag_addr_arr[i + 16]);

      snprintf(buffer, sizeof(buffer), "init mux3:%u\r\n", i);
      debugtalk(buffer);
    }
    enabled_tmags[i + 16] = (ret == HAL_OK);
  }
}

void PrintEnabledTmags() {
  for (int i = 0; i < 25; i++) {
    snprintf(buffer, sizeof(buffer), "en_tmags[%d] = %u\r\n", i,
             enabled_tmags[i] ? 1U : 0U);
    debugtalk(buffer);
  }
}

void sensor_handle() {
  for (int d = 0; d < 25; d++) {
    // skip sensors that are not plugged in
    if (!enabled_tmags[d]) {
      continue;
    }

    int ret2;
    
    if (d >= 16) {
      ret2 = i2c_mux_select_multi(&tca_mux_3, 0xFF);
    } else if (d >= 8) {
      ret2 = i2c_mux_select_multi(&tca_mux_2, 0xFF);
    } else {
      ret2 = i2c_mux_select_multi(&tca_mux_1, 0xFF);
    }

    // reading rotation from sensor
	  TMAG5273_Axis_t mag;
	  TMAG5273_Angle_t angle;
	  uint8_t ret = -1;

	  // reading sensor magnet pos. to output led/etc.
	  ret = TMAG5273_ReadMagneticField(&tmag_handles[d], &mag);
	  if (ret == 0) {
	  	//snprintf(buffer, sizeof(buffer), "d=%d, z=%.3fmT\r\n", d, mag.Bz);
      //debugtalk(buffer);
      tmag_pos[d] = fabsf(mag.Bz);
    } else {
      snprintf(buffer, sizeof(buffer), "Sensor Index = %d, RMF..() failed code = %d\r\n", d, ret);
      debugtalk(buffer);
    }

    ret = TMAG5273_ReadAngle(&tmag_handles[d], &angle);
		if (ret == 0) {
			//printf("Angle: %.2f deg, Magnitude: %.2f\r\n", angle.angle, angle.magnitude);
      tmag_rot[d] = fabsf(angle.angle);
		} else {
			snprintf(buffer, sizeof(buffer), "Sensor Index = %d, Failed angle read, code: %d\r\n", d, ret);
      debugtalk(buffer);
		}
  }

  if (!storedInitialData) {
    storedInitialData = true;

    for (int i = 0; i < 25; i++) {
      tmag_pos_init[i] = tmag_pos[i];
      tmag_rot_init[i] = tmag_rot[i];
    }
  }
}