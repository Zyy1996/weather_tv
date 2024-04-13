#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "mqtt_handle.h"

void msg_task(void *param)
{
    mqtt_app_start();
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
