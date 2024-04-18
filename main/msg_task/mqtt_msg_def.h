#ifndef _MQTT_DEF_H_
#define _MQTT_DEF_H_

//设备主动请求的消息

#define MQTT_TAG "mqtt_ser"

typedef enum {
    MQTT_DEV_GET_TIMESTAMP,
    MQTT_DEV_GET_WEATHER,
    MQTT_DEV_GET_MAX_NUM,
} mqtt_dev_get_msg_id_t;

//服务器主动请求的消息
typedef enum  {
    MQTT_SER_GET_DEV_INFO,
    MQTT_SER_GET_MAX_NUM
}mqtt_ser_get_msg_id_t;

// 设备主动上报的消息，不需要应答
typedef enum  {
    MQTT_DEV_REPORT_TEMP_AND_HUM, //上报温湿度
    MQTT_DEV_REPORT_TOPIC_MAX_NUM
}mqtt_dev_report_msg_id_t;

// 服务器主动上报的消息，不需要应答
typedef enum  {
MQTT_SER_REPORT_TOPIC_MAX_NUM,
}mqtt_ser_report_msg_id_t;

typedef struct mqtt_sub_msg{
    char *sub_msg;
    void (*msg_handle)(char *data,int len);
}mqtt_sub_msg_t;

typedef enum {
    MQTT_SER_RSP,
    MQTT_SER_GET,
    MQTT_TOP_MSG_NUM,
};

typedef struct mqtt_main_msg
{
    int len;
    char *main_msg;
    mqtt_sub_msg_t *mqtt_sub_msg;
}mqtt_main_msg_t;


int get_sys_msg_topic(int key, char *buf, int len);
int get_sys_msg_resp_topic(int key, char *buf, int len);
int get_ser_msg_topic(int key,char *buf,int len);
int get_ser_msg_resp_topic(int key,char *buf,int len);
int get_ser_report_topic(int key,char *buf,int len);
int get_dev_report_topic(int key,char *buf,int len);
int mqtt_msg_handle(char *topic ,int topic_len,char *data,int data_len);
int mqtt_msg_subscribe(void *client);
#endif