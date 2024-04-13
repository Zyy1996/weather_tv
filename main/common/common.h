#ifndef _COMMON_H_
#define _COMMON_H_

typedef struct dev_info {
    char net_status; // 0:网络没有连接,1:连接成功
    char dev_id[32];
    char dev_secret[32];
    char product_id[32];
    char wifi_ap_name[16];
    char wifi_ap_passwd[16];
} dev_info_t;

int esp_db_init();
int dev_info_init();
dev_info_t *get_dev_info();


#endif