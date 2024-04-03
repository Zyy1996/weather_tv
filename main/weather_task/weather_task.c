/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "nvs_flash.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "esp_crt_bundle.h"
#include "esp_system.h"
// #include "esp_tls.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_http_client.h"
#include <ctype.h>

#define MAX_HTTP_OUTPUT_BUFFER 512
static const char *TAG = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    ESP_LOGD(TAG, "%d", evt->event_id);
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        // Clean the buffer in case of a new request
        if (output_len == 0 && evt->user_data) {
            // we are just starting to copy the output data into the use
            memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
        }
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary
         * data. However, event handler can also be used in case chunked encoding is used.
         */
        // if (!esp_http_client_is_chunked_response(evt->client)) {
        // If user_data buffer is configured, copy the response into the buffer
        int copy_len = 0;
        if (evt->user_data) {
            // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
            copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
            if (copy_len) {
                memcpy(evt->user_data + output_len, evt->data, copy_len);
            }
        } else {
            int content_len = esp_http_client_get_content_length(evt->client);
            if (output_buffer == NULL) {
                // We initialize output_buffer with 0 because it is used by strlen() and similar functions therefore
                // should be null terminated.
                output_buffer = (char *)calloc(content_len + 1, sizeof(char));
                output_len = 0;
                if (output_buffer == NULL) {
                    ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                    return ESP_FAIL;
                }
            }
            copy_len = MIN(evt->data_len, (content_len - output_len));
            if (copy_len) {
                memcpy(output_buffer + output_len, evt->data, copy_len);
            }
        }
        output_len += copy_len;
        // }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL) {
            // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0) {
            ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
        }
        if (output_buffer != NULL) {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        esp_http_client_set_header(evt->client, "From", "user@example.com");
        esp_http_client_set_header(evt->client, "Accept", "text/html");
        esp_http_client_set_redirection(evt->client);
        break;
    }
    return ESP_OK;
}

static int check_ip(char *ip) {
    int num, dots = 0;
    char *ptr;

    if (ip == NULL)
        return 0;

    ptr = strtok(ip, ".");
    if (ptr == NULL)
        return 0;

    while (ptr) {
        if (!isdigit((unsigned int)(*ptr)))
            return 0;

        num = atoi(ptr);
        if (num < 0 || num > 255)
            return 0;

        dots++;
        ptr = strtok(NULL, ".");
    }

    return (dots == 3);
}

static int https_get_public_ip(char *ip_buf, int len) {
    int rc = 0;
    // Declare local_response_buffer with size (MAX_HTTP_OUTPUT_BUFFER + 1) to prevent out of bound access when
    // it is used by functions like strlen(). The buffer should only be used upto size MAX_HTTP_OUTPUT_BUFFER
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
    /**
     * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path
     * parameters. If host and path parameters are not set, query parameter will be ignored. In such cases, query
     * parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    esp_http_client_config_t config = {
        .url = "https://api.ipify.org",
        .method = HTTP_METHOD_GET,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %" PRId64 " user_data:%s",
                 esp_http_client_get_status_code(client), esp_http_client_get_content_length(client),
                 local_response_buffer);
        if (strlen(local_response_buffer) > 0) {
            snprintf(ip_buf, len, "%s", local_response_buffer);
            rc = 0;
        } else {
            rc = -1;
        }
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    return rc;
}

static void http_get_weather(char *ip_buf) {
    if (!ip_buf) {
        return;
    }
    // Declare local_response_buffer with size (MAX_HTTP_OUTPUT_BUFFER + 1) to prevent out of bound access when
    // it is used by functions like strlen(). The buffer should only be used upto size MAX_HTTP_OUTPUT_BUFFER
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
    /**
     * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path
     * parameters. If host and path parameters are not set, query parameter will be ignored. In such cases, query
     * parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    char url_buf[64] = {0};

    snprintf(url_buf, sizeof(url_buf), "http://139.196.230.94:8080/api/weather/get?ip=%s", ip_buf);
    ESP_LOGI(TAG, "url:%s", url_buf);
    esp_http_client_config_t config = {
        .url = url_buf,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer, // Pass address of local buffer to get response
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %" PRId64 " user_data:%s",
                 esp_http_client_get_status_code(client), esp_http_client_get_content_length(client),
                 local_response_buffer);
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

void http_weather_task(void *pvParameters) {
    char ip_buf[32] = {0};
    if (https_get_public_ip(ip_buf, sizeof(ip_buf)) == 0) {
        http_get_weather(ip_buf);
    }
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
