#include "esp_log.h"
#include "drivers/HardwareAbstractionLayer.h"
#include "drivers/FanController.h"
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Main";
static bool windup = true;

void setup()
{
    tl_hal_init();
}

void app_main()
{
    setup();
    while(1)
    {
        uint8_t fan_speed = fan_controller_get_speed();

        if (windup)
        {
            fan_controller_increment_speed();
            if (fan_speed >= FAN_SPEED_MAX)
            {
                ESP_LOGI(TAG, "Winding down");
                windup = false;
            }
        }
        else
        {
            fan_controller_decrement_speed();
            if (fan_speed <= FAN_SPEED_MIN)
            {
                windup = true;
            }
        }
        ESP_LOGI(TAG, "Fan speed %d", fan_controller_get_speed());
        vTaskDelay(pdMS_TO_TICKS(1000)); // 100ms task delay
    }
}