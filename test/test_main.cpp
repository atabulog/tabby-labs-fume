#include <unity.h>
#include "freertos/FreeRTOS.h"  // Add this include for portTICK_PERIOD_MS

extern "C" {
    #include "drivers/FanController.h"
}

// Optional: setup before each test
void setUp(void) {
    fan_controller_init();  // Ensure system is initialized
}

void tearDown(void) {
    // Clean up, if needed
}

void test_valid_duty_cycle(void) {
    fan_controller_set_duty(50);
    TEST_ASSERT_EQUAL_UINT8(50, fan_controller_get_duty());
}

void test_invalid_duty_cycle_over_max(void) {
    fan_controller_set_duty(150);  // Assuming FAN_DUTY_MAX = 100
    TEST_ASSERT_LESS_OR_EQUAL_UINT8(FAN_DUTY_MAX, fan_controller_get_duty());
}

void test_invalid_duty_cycle_negative(void) {
    fan_controller_set_duty(-1);  // -1 becomes 255 as uint8_t, might trigger warning
    TEST_ASSERT_LESS_OR_EQUAL_UINT8(FAN_DUTY_MAX, fan_controller_get_duty());
}

extern "C" 
{
int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_valid_duty_cycle);
    RUN_TEST(test_invalid_duty_cycle_over_max);
    RUN_TEST(test_invalid_duty_cycle_negative);
    UNITY_END();
    // End the test loop so it doesn't run repeatedly
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();  // reboot to clean up
    return 0;  // Return success
}  
}
