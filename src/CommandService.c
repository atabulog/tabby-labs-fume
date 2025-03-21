#include "CommandService.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include <stdbool.h>
#include <string.h>

#define MAX_COMMAND_REGISTRY_SIZE 10

// log tag
QueueHandle_t command_queue;
command_t command_registry[MAX_COMMAND_REGISTRY_SIZE];
static BaseType_t xTask_create_result = pdFALSE;
// Command queue handle
static const char *TAG = "CommandService";
// Command service ready flag
bool command_service_is_ready = false;

/**
 * @brief task to process commands in the queue. Halts until a command is received.
 * 
 */
void process_commands_task(void)
{
    command_t command;
    while (1)
    {
        // Wait for a command to be received
        if (xQueueReceive(command_queue, &command, portMAX_DELAY) == pdTRUE)
        {
            // Lookup the command in the registry
            for (int i = 0; i < MAX_COMMAND_REGISTRY_SIZE; i++)
            {
                // Compare the command with the registered commands
                if(command_registry[i].command == command.command)
                {
                    // Execute the callback if found
                    if (command_registry[i].callback != NULL)
                    {
                        command_registry[i].callback();
                    }
                    break;
                }
            }
        }
    }
}

/**
 * @brief Initializes the command service.
 * 
 */
esp_err_t command_service_init()
{
    command_queue = xQueueCreate(10, sizeof(command_type_t));
    if (command_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create command queue");
        return ESP_FAIL;
    }

    xTask_create_result = xTaskCreatePinnedToCore(
        process_commands_task,  // Task function
        "CommandProcessor",   // Task name
        2048,                 // Stack size (2 KB)
        NULL,                 // Task parameters
        1,                    // Priority (1 = low)
        NULL,                 // Task handle (not needed)
        1                     // Run on core 1 (separate from WiFi core)
    );
    if(xTask_create_result != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create command processing task: %d", xTask_create_result);
        return ESP_FAIL;
    }

    command_service_is_ready = true;
    return ESP_OK;
}

bool register_command(command_type_t command, void (*callback)(void))
{
    // Check if the command service is initialized
    if(!command_service_is_ready)
    {
        ESP_LOGE(TAG, "Command service not initialized");
        return false;
    }
    // Register the command in an empty slot
    for (int i = 0; i < MAX_COMMAND_REGISTRY_SIZE; i++)
    {
        if (command_registry[i].callback == NULL)
        {
            command_registry[i].command = command;
            command_registry[i].callback = callback;
            return true;
        }
    }
    ESP_LOGE(TAG, "Command registry is full");
    return false;
}

void IRAM_ATTR command_service_enqueue_from_isr(command_type_t command_type)
{
    // Check if the command service is initialized
    if(!command_service_is_ready)
    {
        ESP_LOGE(TAG, "Command service not initialized");
        return;
    }
    // Enqueue the command
    if (xQueueSendFromISR(command_queue, &command_type, NULL) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to enqueue command from ISR");
    }
}