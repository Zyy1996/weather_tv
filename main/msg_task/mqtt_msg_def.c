#include "mqtt_msg_def.h"

char *mqtt_sys_msg_list[] = {[MQTT_GET_TIMESTAMP] = "kp/%s/%s/sys/timestamp"};

char *mqtt_sys_msg_resp_list[] = {[MQTT_GET_TIMESTAMP_RESP] = "kp/%s/%s/sys/timestamp"};

int mqtt_msg_init() { return 0; }

int get_sys_msg_str(int key, char *buf, int len) { return 0; }

int get_sys_msg_resp_str(int key, char *buf, int len) { return 0; }
