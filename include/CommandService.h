#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include "esp_err.h"
#include "esp_attr.h"
#include <stdbool.h>

/// @brief Command type for commands stored in service
typedef enum
{
    CMD_FAN_INCREMENT,
    CMD_FAN_DECREMENT,
    CMD_FAN_DEFAULT
}command_type_t;

/// @brief Command structure to hold command type and callback function
typedef struct
{
    command_type_t command;
    void (*callback)(void);
} command_t;


/**
 * @brief Initializes the command service.
 * 
 */
esp_err_t command_service_init(void);

/**
 * @brief Register a command type with a callback function.
 * 
 * @param command command id
 * @param callback function to execute when command is received
 * @return true if registration was successful, else false
 */
bool register_command(command_type_t command, void (*callback)(void));

/**
 * @brief Enqueues a command to be executed.
 * 
 * @param cmd The command to enqueue.
 */
void IRAM_ATTR command_service_enqueue_from_isr(command_type_t command_type);


#endif // COMMAND_SERVICE_H