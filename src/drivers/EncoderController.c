#include "drivers/EncoderController.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "CommandService.h"

//defines for the encoder pins
#define ENCODER_PIN_CLK 12 // Encoder Clock
#define ENCODER_PIN_DT 13  // Encoder Data
#define ENCODER_PIN_SW 14  // Encoder Button

/// @brief The last state of the CLK pin set in the isr
volatile int lastCLK = 1;
static const char *TAG = "EncoderController";

/**
 * @brief Interrupt handler for encoder rotation.
 */
void IRAM_ATTR encoder_rotation_isr_handler()
{
    int CLK_State = gpio_get_level(ENCODER_PIN_CLK);
    int DT_State = gpio_get_level(ENCODER_PIN_DT);

    if (CLK_State != lastCLK)
    {
        if (DT_State != CLK_State)
        {
            //clockwise rotation speeds up fan
            command_service_enqueue_from_isr(CMD_FAN_INCREMENT);
        }
        else
        {
            //counter-clockwise rotation slows down fan
            command_service_enqueue_from_isr(CMD_FAN_DECREMENT);
        }
    }
    lastCLK = CLK_State;
}

/**
 * @brief Interrupt handler for encoder button press.
 */
void IRAM_ATTR encoder_press_isr_handler()
{
    //push button pressed, set fan to default speed
    command_service_enqueue_from_isr(CMD_FAN_DEFAULT);
}

esp_err_t encoder_controller_init()
{
    // Initialize the encoder pins
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << ENCODER_PIN_CLK) | (1ULL << ENCODER_PIN_DT) | (1ULL << ENCODER_PIN_SW),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf);

    // Attach interrupt handlers for the encoder
    gpio_install_isr_service(0);
    gpio_isr_handler_add(ENCODER_PIN_CLK, encoder_rotation_isr_handler, (void *)ENCODER_PIN_CLK);
    gpio_isr_handler_add(ENCODER_PIN_SW, encoder_press_isr_handler, (void *)ENCODER_PIN_SW);

    ESP_LOGI(TAG, "Encoder controller initialized");
    return ESP_OK;
}

