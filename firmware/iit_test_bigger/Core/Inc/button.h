#ifndef BUTTON_H
#define BUTTON_H

/**
 * @brief Handles button presses on board for debugging purposes
 */
void button_debug_handle();

/**
 * @brief Handles button presses on board
 */
void button_handle();


extern bool button_state[4];


#endif