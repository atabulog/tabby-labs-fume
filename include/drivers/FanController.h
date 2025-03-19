#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include "esp_err.h"
#include <stdint.h>

// defines for fan limits
#define FAN_SPEED_MAX 100
#define FAN_SPEED_MIN 0
#define FAN_SPEED_STEP 10

/**
 * @brief Initializes the fan controller.
 */
esp_err_t fan_controller_init(void);

/**
 * @brief Sets the fan speed.
 * @param speed speed value from 0-100
 * 0 = off, 100 = full speed
 */
void fan_controller_set_speed(uint8_t speed);

/**
 * @brief Gets the fan speed.
 * 
 * @return uint8_t speed value from 0-100
 */
uint8_t fan_controller_get_speed(void);

/**
 * @brief Increments the fan speed by 10%.
 * @note The speed will not exceed 100%.
 */
void fan_controller_increment_speed(void);

/**
 * @brief Decrements the fan speed by 10%
 * @note The speed will not drop below 0%.
 */
void fan_controller_decrement_speed(void);


#endif //FAN_CONTROLLER_H