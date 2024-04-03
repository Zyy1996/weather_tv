#include "cJSON.h"
#include "weather_json_parse.h"
#include <stdio.h>
#include <string.h>

int gaud_live_weather_info_get(gaud_live_weather_info *weather_info,
                               char *msg) {
    if (!weather_info || !msg) {
        return -1;
    }
    char *tmp_str;
    cJSON *p = cJSON_Parse(msg);
    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(p, "status"));
    if (tmp_str) {
        sscanf(tmp_str, "%d", &weather_info->status);
    } else {
        goto RET_ERR;
    }

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(p, "count"));
    if (tmp_str) {
        sscanf(tmp_str, "%d", &weather_info->count);
    } else {
        goto RET_ERR;
    }

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(p, "info"));
    if (tmp_str) {
        snprintf(weather_info->info, sizeof(weather_info->info), "%s", tmp_str);
    } else {
        goto RET_ERR;
    }

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(p, "infocode"));
    if (tmp_str) {
        sscanf(tmp_str, "%d", &weather_info->infocode);
    } else {
        goto RET_ERR;
    }
    cJSON *city_p = cJSON_GetArrayItem(cJSON_GetObjectItem(p, "lives"), 0);

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(city_p, "province"));
    if (tmp_str) {
        snprintf(weather_info->province, sizeof(weather_info->province), "%s",
                 tmp_str);
    } else {
        goto RET_ERR;
    }

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(city_p, "city"));
    if (tmp_str) {
        snprintf(weather_info->city, sizeof(weather_info->city), "%s", tmp_str);
    } else {
        goto RET_ERR;
    }

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(city_p, "adcode"));
    if (tmp_str) {
        sscanf(tmp_str, "%d", &weather_info->adcode);
    } else {
        goto RET_ERR;
    }
    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(city_p, "weather"));
    if (tmp_str) {
        snprintf(weather_info->weather, sizeof(weather_info->weather), "%s",
                 tmp_str);
    } else {
        goto RET_ERR;
    }

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(city_p, "temperature"));
    if (tmp_str) {
        sscanf(tmp_str, "%d", &weather_info->temperature);
    } else {
        goto RET_ERR;
    }

    tmp_str =
        cJSON_GetStringValue(cJSON_GetObjectItem(city_p, "winddirection"));
    if (tmp_str) {
        snprintf(weather_info->winddirection,
                 sizeof(weather_info->winddirection), "%s", tmp_str);
    } else {
        goto RET_ERR;
    }

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(city_p, "windpower"));
    if (tmp_str) {
        snprintf(weather_info->windpower, sizeof(weather_info->windpower), "%s",
                 tmp_str);
    } else {
        goto RET_ERR;
    }

    tmp_str = cJSON_GetStringValue(cJSON_GetObjectItem(city_p, "humidity"));
    if (tmp_str) {
        sscanf(tmp_str, "%d", &weather_info->humidity);
    } else {
        goto RET_ERR;
    }

    cJSON_Delete(p);
    return 0;
RET_ERR:
    cJSON_Delete(p);
    return -1;
}