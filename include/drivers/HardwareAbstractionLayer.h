#ifndef HARDWARE_ABSTRACTION_LAYER_H
#define HARDWARE_ABSTRACTION_LAYER_H

/**
 * @file HardwareAbstractionLayer.h
 * @author Austin Tabulog
 * @brief The Hardware abstraction layer is responsible for separating the hardware specific code from the application code.
 *        The focus of the HAL is the off-chip sensors, and the chip's setup to enable these systems.
 * @version 0.1
 * @date 2025-03-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/**
 * @brief Initializes the hardware abstraction layer (hal_init already taken by ESP-IDF)
 */
void tl_hal_init(void);




#endif //HARDWARE_ABSTRACTION_LAYER_H