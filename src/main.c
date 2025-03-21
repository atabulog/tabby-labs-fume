#include "esp_log.h"
#include "CommandService.h"
#include "drivers/HardwareAbstractionLayer.h"
#include "drivers/FanController.h"
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Main";

void setup()
{
    //exit program if command service not initialized correctly
    if(command_service_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize command service");
        assert(false);
    }

    //register fan controller commands
    if(!register_command(CMD_FAN_INCREMENT, fan_controller_increment_speed) ||
       !register_command(CMD_FAN_DECREMENT, fan_controller_decrement_speed) ||
       !register_command(CMD_FAN_DEFAULT, fan_controller_reset_speed))
    {
        ESP_LOGE(TAG, "Failed to register command");
        assert(false);
    }

    tl_hal_init();
}

void app_main()
{
    setup();
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // 100ms task delay
        ESP_LOGI(TAG, "fan speed: %d", fan_controller_get_speed());
    }
}