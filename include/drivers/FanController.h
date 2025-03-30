#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include "esp_err.h"
#include <stdint.h>

// defines for fan limits
#define FAN_DUTY_MAX 100
#define FAN_DUTY_MIN 0
#define FAN_DUTY_STEP 5

/**
 * @brief Initializes the fan controller.
 */
esp_err_t fan_controller_init(void);

/**
 * @brief Sets the fan duty.
 * @param duty duty value from 0-100
 * 0 = off, 100 = full duty
 */
void fan_controller_set_duty(uint8_t duty);

/**
 * @brief Gets the fan duty.
 * 
 * @return uint8_t duty value from 0-100
 */
uint8_t fan_controller_get_duty(void);

/**
 * @brief Increments the fan speed by 10%.
 * @note The speed will not exceed 100%.
 */
void fan_controller_increment_duty(void);

/**
 * @brief Decrements the fan duty by 10%
 * @note The duty will not drop below 0%.
 */
void fan_controller_decrement_duty(void);

/**
 * @brief Resets the fan duty back to the stored default value
 */
void fan_controller_reset_duty(void);

/**
 * @brief Gets the current RPM of the fan.
 */
int32_t fan_controller_get_rpm(void);

#endif //FAN_CONTROLLER_H