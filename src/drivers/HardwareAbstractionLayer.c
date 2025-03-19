#include "drivers/HardwareAbstractionLayer.h"
#include "drivers/FanController.h"
#include "esp_log.h"
#include <stdbool.h>

//tag for logging
static const char* TAG = "HardwareAbstractionLayer";

void tl_hal_init()
{
    if(fan_controller_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize fan controller");
        assert(false); //exit application
    }
}

//esp_err_t hal_init_status_light(void)
//{
//    gpio_config_t io_conf = {
//        .pin_bit_mask = (1ULL << GPIO_NUM_2), // GPIO2 is typically the onboard LED
//        .mode = GPIO_MODE_OUTPUT,
//        .pull_up_en = GPIO_PULLUP_DISABLE,
//        .pull_down_en = GPIO_PULLDOWN_DISABLE,
//        .intr_type = GPIO_INTR_DISABLE
//    };
//
//    esp_err_t ret = gpio_config(&io_conf);
//    if (ret != ESP_OK)
//    {
//        ESP_LOGE(TAG, "Failed to configure GPIO for status light");
//        return ret;
//    }
//
//    // Set initial state of the LED to off
//    gpio_set_level(GPIO_NUM_2, 0);
//
//    ESP_LOGI(TAG, "Status light initialized");
//    return ESP_OK;
//}