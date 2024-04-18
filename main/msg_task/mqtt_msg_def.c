#include "mqtt_msg_def.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

char *mqtt_dev_msg_list[] = {
    [MQTT_DEV_GET_TIMESTAMP] = "kp/%s/%s/dev_get/timestamp",
    [MQTT_DEV_GET_WEATHER] = "kp/%s/%s/dev_get/weather"
};

char *mqtt_dev_msg_resp_list[] = {
    [MQTT_DEV_GET_TIMESTAMP] = "kp/%s/%s/ser_rsp/timestamp",
    [MQTT_DEV_GET_WEATHER] = "kp/%s/%s/ser_rsp/weather"
};

char *mqtt_ser_msg_list[] = {[MQTT_SER_GET_DEV_INFO] = "kp/%s/%s/ser_get/dev_info"};

char *mqtt_ser_msg_resp_list[] = {[MQTT_SER_GET_DEV_INFO] = "kp/%s/%s/dev_rsp/dev_info"};

char *mqtt_dev_msg_report_list[] = {[MQTT_DEV_REPORT_TEMP_AND_HUM] = "kp/%s/%s/dev_report/temp_and_hum"};

char *mqtt_ser_msg_report_list[] = {[MQTT_SER_GET_DEV_INFO] = "kp/%s/%s/ser_report/timestamp"};

mqtt_sub_msg_t mqtt_ser_resp_msg[] = {
    {"timestamp",NULL},
    {"weather",NULL}
};

mqtt_sub_msg_t mqtt_ser_get_msg[] = {
    {"dev_info",NULL},
};

mqtt_main_msg_t mqtt_main_msg[] ={
    [MQTT_SER_RSP] = {
        .main_msg = "ser_rsp",
        .len = sizeof(mqtt_ser_resp_msg)/sizeof(mqtt_ser_resp_msg[0]),
        .mqtt_sub_msg = mqtt_ser_resp_msg,
    },
    [MQTT_SER_GET] = {
        .main_msg = "ser_get",
        .len = sizeof(mqtt_ser_get_msg)/sizeof(mqtt_ser_get_msg[0]),
        .mqtt_sub_msg = mqtt_ser_get_msg,
    }
};

int mqtt_msg_handle(char *topic ,int topic_len,char *data,int data_len) {
    dev_info_t *dev_info = get_dev_info();
    int first_len = 5 + strlen(dev_info->product_id) + strlen(dev_info->dev_id);
    if(first_len < topic_len) {
        for(int i = 0 ; i < MQTT_TOP_MSG_NUM ; i++) {
            if(strstr(topic+first_len,mqtt_main_msg[i].main_msg)) {
                first_len += strlen(mqtt_main_msg[i].main_msg);
                for(int j = 0 ; j< mqtt_main_msg[i].len ; j++) {
                    if(strstr(topic+first_len, mqtt_main_msg[i].mqtt_sub_msg[j].sub_msg)) {
                        if(mqtt_main_msg[i].mqtt_sub_msg[j].msg_handle) {
                            mqtt_main_msg[i].mqtt_sub_msg[j].msg_handle(data,data_len);
                        } else {
                            ESP_LOGE("topic %s not processed",topic);
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    return 0;
}

int mqtt_msg_init() { return 0; }

int get_ser_report_topic(int key,char *buf,int len) {
    dev_info_t *dev_info = get_dev_info();
    switch (key)
    {
    case MQTT_DEV_REPORT_TEMP_AND_HUM:
        snprintf(buf,len,mqtt_ser_msg_report_list[key],dev_info->product_id,dev_info->dev_id);
        break;
    default:
        break;
    }
}

int get_dev_report_topic(int key,char *buf,int len) {
    dev_info_t *dev_info = get_dev_info();
    switch (key)
    {
    case MQTT_DEV_REPORT_TEMP_AND_HUM:
        snprintf(buf,len,mqtt_dev_msg_report_list[key],dev_info->product_id,dev_info->dev_id);
        break;
    default:
        break;
    }
    return 0; 
}

int get_ser_msg_topic(int key,char *buf,int len) {
    dev_info_t *dev_info = get_dev_info();
    switch (key)
    {
    case MQTT_SER_GET_DEV_INFO:
        snprintf(buf,len,mqtt_ser_msg_list[key],dev_info->product_id,dev_info->dev_id);
        break;
    default:
        break;
    }
    return 0; 
}


int get_ser_msg_resp_topic(int key,char *buf,int len) {
    dev_info_t *dev_info = get_dev_info();
    switch (key)
    {
    case MQTT_SER_GET_DEV_INFO:
        snprintf(buf,len,mqtt_ser_msg_resp_list[key],dev_info->product_id,dev_info->dev_id);
        break;
    default:
        break;
    }
    return 0;
}

int get_dev_msg_topic(int key, char *buf, int len)
{
   dev_info_t *dev_info = get_dev_info();
    switch (key)
    {
    case MQTT_DEV_GET_TIMESTAMP:
    case MQTT_DEV_GET_WEATHER:
        snprintf(buf,len,mqtt_dev_msg_list[key],dev_info->product_id,dev_info->dev_id);
        break;
    default:
        break;
    }
    return 0; 
}

int get_dev_msg_resp_topic(int key, char *buf, int len)
{
   dev_info_t *dev_info = get_dev_info();
    switch (key)
    {
    case MQTT_DEV_GET_TIMESTAMP:
    case MQTT_DEV_GET_WEATHER:
        snprintf(buf,len,mqtt_dev_msg_resp_list[key],dev_info->product_id,dev_info->dev_id);
        break;
        break;
    default:
        break;
    }
    return 0; 
}

int mqtt_msg_subscribe(void *client) {
    char topic_buf[256];
    int msg_id;
    get_dev_msg_resp_topic(MQTT_DEV_GET_TIMESTAMP,topic_buf,sizeof(topic_buf));
    msg_id = esp_mqtt_client_subscribe(client, topic_buf, 1);
    get_dev_msg_resp_topic(MQTT_DEV_GET_WEATHER,topic_buf,sizeof(topic_buf));
    msg_id = esp_mqtt_client_subscribe(client, topic_buf, 1);
    get_ser_msg_topic(MQTT_SER_GET_DEV_INFO,topic_buf,sizeof(topic_buf));
    msg_id = esp_mqtt_client_subscribe(client, topic_buf, 1);
    return 0;
}