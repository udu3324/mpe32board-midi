#ifndef TMAG_HELPER_H
#define TMAG_HELPER_H


/**
 * @brief initializes all the sensors that are plugged into the 24x connectors
 */
void Init_TMAGS();

/**
 * @brief prints all the sensors that were sucessfully initialized
 */
void PrintEnabledTmags();

/**
 * @brief reads all actively connected sensors to save to data arrays
 */
void sensor_handle();

extern bool enabled_tmags[25];

extern bool storedInitialData;

extern float tmag_pos[25];
extern float tmag_rot[25];

extern float tmag_pos_init[25];
extern float tmag_rot_init[25];

#endif