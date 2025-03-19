#include "drivers/FanController.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//defines for the fan pwm hardware
#define FAN_PWM_PIN 18
#define PWM_FREQ_HZ 88
#define PWM_RESOLUTION LEDC_TIMER_8_BIT
#define PWM_CHANNEL LEDC_CHANNEL_0

#define DUTY_CYCLE_U8(x) ((x) * 255 / 100) // Scale 0-100% to 0-255

static const char *TAG = "FanController";
static bool is_fan_controller_initialized = false;
static uint8_t current_speed = 0;
static uint8_t temp_speed = 0;

esp_err_t fan_controller_init()
{
    if (is_fan_controller_initialized)
    {
        ESP_LOGW(TAG, "Fan controller already initialized");
        return ESP_FAIL;
    }
    
    // Initialize the fan controller
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .gpio_num = FAN_PWM_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CHANNEL,
        .timer_sel = LEDC_TIMER_0,
        .duty = (DUTY_CYCLE_U8(0)), // Set initial duty cycle to 0%
        .hpoint = 0
    };
    ledc_channel_config(&channel_config);

    is_fan_controller_initialized = true;
    ESP_LOGI(TAG, "Fan PWM initialized");
    return ESP_OK;
}

void fan_controller_set_speed(uint8_t speed)
{
    // Set the fan speed (e.g., 0 to 100%)
    if (speed < FAN_SPEED_MIN || speed > FAN_SPEED_MAX) {
        ESP_LOGW(TAG, "Invalid speed %d. Speed must be between %d and %d.", speed, FAN_SPEED_MIN, FAN_SPEED_MAX);
        return;
    }
    current_speed = speed;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, DUTY_CYCLE_U8(current_speed));  // Scale 0-255
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);
}

uint8_t fan_controller_get_speed()
{
    return current_speed;
}

void fan_controller_increment_speed(void)
{
    if (temp_speed < FAN_SPEED_MAX) {
        temp_speed += FAN_SPEED_STEP;
        if (temp_speed > FAN_SPEED_MAX) {
            temp_speed = FAN_SPEED_MAX;
        }
        fan_controller_set_speed(temp_speed);
    }
}

void fan_controller_decrement_speed(void)
{
    if (temp_speed > FAN_SPEED_MIN)
    {
        temp_speed -= FAN_SPEED_STEP;
        //catch under value and overflow case
        if (temp_speed > FAN_SPEED_MAX)
        {
            temp_speed = FAN_SPEED_MIN;
        }
        fan_controller_set_speed(temp_speed);
    }
}