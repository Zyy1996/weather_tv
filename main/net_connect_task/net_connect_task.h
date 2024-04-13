#ifndef _BIND_TASK_H_
#define _BIND_TASK_H_

#define BIND_TAG "app_bind"

#define BIND_KEY "bind_status"

typedef struct bind_info
{
    char bind_status; //0 没有绑定 1 :绑定
    char *wifi_name;//wifi
    char *wifi_passwd;
}bind_info_t;

void bind_task(void *param);

#endif