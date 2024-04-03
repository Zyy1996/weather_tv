/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "littlefs_filesystem.h"
#include "lvgl.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "ui_task.h"
#include "weather_task.h"
#include "wifi_sta.h"
#include <stdio.h>

TaskHandle_t ui_Handle = NULL;
TaskHandle_t weather_Handle = NULL;

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    wifi_init_sta();
    filesystem_init();
    // http_weather_task();
    xTaskCreate(http_weather_task, "weather_task", 8192, NULL, 5, &weather_Handle);
    ESP_ERROR_CHECK(ret);
    xTaskCreate(ui_task, "ui_task", 8192, NULL, 5, &ui_Handle);
}
