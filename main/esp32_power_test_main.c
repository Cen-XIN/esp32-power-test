#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_err.h"
#include "esp_sleep.h"

#define TEST_ERROR -1
#define ESP_ONE_SEC 1000000

// TEST CONST
#define TEST_TIME 60 // seconds
#define SWITCH_FREQ 0.5 // Hz

// test_time in second
void DelayBeforeTest();
void ActiveTest(int test_time); // no specific work just in idle
void LightSleepTest(int test_time);
void SwitchTest(int test_time, float switch_freq); // switch between active mode and light-sleep mode, each mode for 1 sec

void app_main()
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    /* BEGIN OF TEST */
    // ActiveTest(test_time);
    // LightSleepTest(test_time);
    SwitchTest(TEST_TIME, SWITCH_FREQ);
    printf("Finished!\n");
    /* END OF TEST */

    /* Restart after test */
    for (int i = 5; i > 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

void DelayBeforeTest()
{
    for (int i = 5; i > 0; i--) {
        printf("TEST will start in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void ActiveTest(int test_time)
{
    printf("[ESP32] Active Mode Test\n");
    DelayBeforeTest();

    for (int i = 0; i < test_time; i++) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void LightSleepTest(int test_time)
{
    esp_err_t sleep_ret;
    printf("[ESP32] Light-sleep Mode Test\n");
    DelayBeforeTest();

    if (esp_sleep_enable_timer_wakeup(test_time * ESP_ONE_SEC) != ESP_OK) {
        printf("Sleep timer config error!\n");
        return;
    }
    sleep_ret = esp_light_sleep_start(); // return ESP_OK if successfully waking up
    if (sleep_ret == ESP_OK) {
        printf("Good morning :-)\n");
    } else {
        printf("Ooops, something went wrong...\n");
    }
}

void SwitchTest(int test_time, float switch_freq)
{
    esp_err_t sleep_ret;
    printf("[ESP32] Mode-switching Test\n");
    DelayBeforeTest();

    if (esp_sleep_enable_timer_wakeup((int)(ESP_ONE_SEC / switch_freq)) != ESP_OK) {
        printf("Sleep timer config error!\n");
        return;
    }
    for (int i = 0; i < (int)(test_time / (2 / switch_freq)); i++) {
        printf("Good night .zzZ\n");
        vTaskDelay((int)((1000 / portTICK_PERIOD_MS) / switch_freq));
        sleep_ret = esp_light_sleep_start(); // return ESP_OK if successfully waking up
        if (sleep_ret == ESP_OK) {
            printf("Good morning :-)\n");
        } else {
        printf("Ooops, something went wrong...\n");
        }
    }
}
