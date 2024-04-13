#include "common.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "wifi_handle.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define COMM_TAG "common"

static nvs_handle_t my_handle;
static dev_info_t dev_info;

dev_info_t *get_dev_info() { return &dev_info; }

int esp_db_init() {
    esp_err_t err;
    err = nvs_open_from_partition("nvs", "storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(COMM_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return -1;
    }
    return 0;
}

int dev_info_init() {
    esp_err_t err;
    size_t str_len = sizeof(dev_info.product_id);
    err = nvs_get_str(my_handle, "product_id", dev_info.product_id, &str_len);
    ESP_ERROR_CHECK(err);
    str_len = sizeof(dev_info.dev_id);
    err = nvs_get_str(my_handle, "dev_id", dev_info.dev_id, &str_len);
    ESP_ERROR_CHECK(err);
    str_len = sizeof(dev_info.dev_secret);
    err = nvs_get_str(my_handle, "dev_secret", dev_info.dev_secret, &str_len);
    ESP_ERROR_CHECK(err);
    str_len = sizeof(dev_info.wifi_ap_name);
    err = nvs_get_str(my_handle, "ap_name", dev_info.wifi_ap_name, &str_len);
    ESP_ERROR_CHECK(err);
    str_len = sizeof(dev_info.wifi_ap_passwd);
    err = nvs_get_str(my_handle, "ap_secret", dev_info.wifi_ap_passwd, &str_len);
    ESP_ERROR_CHECK(err);
    ESP_LOGI(COMM_TAG, "product_id:%s,dev_id:%s,dev_secret:%s,ap_name:%s,ap_secret:%s", dev_info.product_id,
             dev_info.dev_id, dev_info.dev_secret, dev_info.wifi_ap_name, dev_info.wifi_ap_passwd);
    return 0;
}