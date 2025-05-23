#include "tl_utils.h"
#include "drivers/FanController.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/pulse_cnt.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

//=============================================================================
// Private Defines
//=============================================================================

//defines for the fan pwm via PWM
#define FAN_PWM_PIN 18
#define FAN_PCNT_PIN 2
#define PWM_FREQ_HZ 25000
#define PWM_RESOLUTION LEDC_TIMER_8_BIT
#define PWM_CHANNEL LEDC_CHANNEL_0
//helper macro to convert duty cycle percentage to 8-bit value
#define DUTY_CYCLE_U8(x) ((x) * 255 / 100) // Scale 0-100% to 0-255
//defines for the fan tachometer via PCNT
#define PCNT_UNIT PCNT_UNIT_1
#define TACH_PULSES_PER_REV 2 // Number of pulses per revolution (depends on fan type)
#define FAN_TACH_CALC_INTERVAL_MS 1000 // Interval for vTask calculating RPM in milliseconds


//=============================================================================
// Private Variable Declarations
//=============================================================================

static const char *TAG = "FanController";
//fan duties are volatile to allow for interrupt access. May need to consider adding mutex for thread safety when using the web server
volatile uint8_t temp_duty = 0;
volatile uint8_t current_duty = 0;
//measured speed is volatile to allow for VTask usage
//volatile int32_t measured_fan_speed_rpm = 0;
//variables for pulse counter
static bool is_fan_controller_initialized = false;
//static pcnt_unit_handle_t pcnt_unit = NULL; // Handle for the PCNT unit


//=============================================================================
// Private Function Declarations
//=============================================================================

/**
 * @brief Private method to calculate the fan speed in RPM within a vTask.
 */
//void fan_controller_calc_speed_rpm(void *pvParameters);


//=============================================================================
// Public Function Implementations
//=============================================================================

esp_err_t fan_controller_init()
{
    if (is_fan_controller_initialized)
    {
        ESP_LOGW(TAG, "Fan controller already initialized");
        return ESP_FAIL;
    }
    //set starting duty - for this fan MAX is off
    temp_duty = FAN_DUTY_MAX; 
    current_duty = FAN_DUTY_MAX;
    
    // Initialize fan speed control via PWM signal
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_channel_config_t channel_config = {
        .gpio_num = FAN_PWM_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CHANNEL,
        .timer_sel = LEDC_TIMER_0,
        .duty = (DUTY_CYCLE_U8(current_duty)), // Set initial duty cycle to 0%
        .hpoint = 0
    };
    ledc_timer_config(&timer_config);
    ledc_channel_config(&channel_config);
    

    //// Initialize the fan speed monitoring via PCNT
    //pcnt_unit_config_t pcnt_config = {
    //    .high_limit = 1000, // Max count value (imposes theoretical limit for fan speed of 30k RPM)
    //    .low_limit = -1000, // No need for a negative limit
    //    .flags.accum_count = false, // Reset count every read
    //};

    //ESP_ERROR_CHECK(pcnt_new_unit(&pcnt_config, &pcnt_unit));

    //pcnt_chan_config_t pcnt_chan_config = {
    //    .edge_gpio_num = FAN_PCNT_PIN, // Fan tach signal
    //    .level_gpio_num = -1, // Not using a level control signal
    //    .flags.invert_edge_input = false, // Normal counting
    //};

    //pcnt_channel_handle_t pcnt_chan = NULL;
    //ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &pcnt_chan_config, &pcnt_chan));
    //ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    
    //ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    //ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    //ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    // Start a FreeRTOS task to calculate the fan speed in RPM
    //BaseType_t xReturned;
    //xReturned = xTaskCreate(fan_controller_calc_speed_rpm, "FanMeasSpeedRPM", 2048, NULL, 5, NULL);
    //if (xReturned != pdPASS)
    //{
    //    ESP_LOGE(TAG, "Failed to create fan speed calculation task");
    //    return ESP_FAIL;
    //}

    // Set init flag and return
    ESP_LOGI(TAG, "Fan Controller initialized");
    is_fan_controller_initialized = true;
    return ESP_OK;
}

void fan_controller_set_duty(uint8_t duty)
{
    // Set the fan duty (e.g., 0 to 100%)
    // Debounce: ignore repeated calls within a short interval
    static uint32_t last_call_time = 0;
    uint32_t now = xTaskGetTickCount();
    const uint32_t debounce_ticks = pdMS_TO_TICKS(100); // 10 ms debounce

    if (now - last_call_time < debounce_ticks) {
        return;
    }
    last_call_time = now;

    if (duty > FAN_DUTY_MAX) {
        ESP_LOGW(TAG, "Invalid duty %d. Duty must be between %d and %d.", duty, FAN_DUTY_MIN, FAN_DUTY_MAX);
        return;
    }
    current_duty = duty;
    ESP_LOGI(TAG, "Setting fan duty to %d %%", DUTY_CYCLE_U8(current_duty));
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, DUTY_CYCLE_U8(current_duty));  // Scale 0-255
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);
}

uint8_t fan_controller_get_duty()
{
    return current_duty;
}

void fan_controller_increment_duty(void)
{
    temp_duty += FAN_DUTY_STEP;
    if (temp_duty > FAN_DUTY_MAX) 
    {
        ESP_LOGI(TAG, "Capped out");
        temp_duty = FAN_DUTY_MAX;
    }
    fan_controller_set_duty(temp_duty);
}

void fan_controller_decrement_duty(void)
{
    if (current_duty > FAN_DUTY_STEP)
    {
        temp_duty -= FAN_DUTY_STEP;
    }
    else
    {
        ESP_LOGI(TAG, "bottomed out");
        temp_duty = FAN_DUTY_MIN;
    }
    fan_controller_set_duty(temp_duty);
}

void fan_controller_reset_duty(void)
{
    temp_duty = current_duty;
        ESP_LOGI(TAG, "duty reset to 30");
    fan_controller_set_duty(30); //todo - replace with stored default value from flash
}

//int32_t fan_controller_get_rpm(void)
//{
//    return measured_fan_speed_rpm;
//}


//=============================================================================
// Private Function Implementations
//=============================================================================

//void fan_controller_calc_speed_rpm(void* pvParameters)
//{
//    int num_pulses = 0;
//    uint64_t last_time_us = esp_timer_get_time();
//    uint64_t current_time_us;
//    float elapsed_time_sec;
//
//    while (true)
//    {
//        // Calculate elapsed time since last read
//        current_time_us = esp_timer_get_time();
//        elapsed_time_sec = MICROSECONDS_TO_SECONDS(current_time_us - last_time_us);
//
//        if (elapsed_time_sec > 0)
//        {
//            // Read pulse count and reset counter
//            ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &num_pulses));
//            ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
//
//            // Calculate fan speed in RPM
//            measured_fan_speed_rpm = ((float)num_pulses / elapsed_time_sec) * (SECONDS_PER_MINUTE / TACH_PULSES_PER_REV);
//            last_time_us = current_time_us;
//        }
//
//        // Sleep until the next interval
//        vTaskDelay(pdMS_TO_TICKS(FAN_TACH_CALC_INTERVAL_MS));
//    }
//}