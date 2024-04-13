#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>
#include "net_connect_task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "wifi_handle.h"


bind_info_t bind_info = {.wifi_name = "weather_clock", .wifi_passwd = "weather_clock"};

void bind_task(void *param)
{
    esp_err_t err;
    // Open the pre-filled NVS partition called "nvs"
    nvs_handle_t my_handle;
    err = nvs_open_from_partition("nvs", "storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(BIND_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }
    err = nvs_get_u8(my_handle, "bind_status", (uint8_t *)&bind_info.bind_status);
    ESP_ERROR_CHECK(err);
     
    if(bind_info.bind_status == 0) {
        wifi_init_softap();
    } else {

    }

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

}