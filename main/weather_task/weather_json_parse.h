#ifndef _WEATHER_JSON_PARSE_H_
#define _WEATHER_JSON_PARSE_H_

/*
{
    "status": "1",
    "count": "1",
    "info": "OK",
    "infocode": "10000",
    "lives": [
        {
            "province": "澳门",
            "city": "花地玛堂区",
            "adcode": "820001",
            "weather": "晴",
            "temperature": "25",
            "winddirection": "无风向",
            "windpower": "≤3",
            "humidity": "83",
            "reporttime": "2023-12-11 15:36:40",
            "temperature_float": "25.0",
            "humidity_float": "83.0"
        }
    ]
}
*/
typedef struct _gaud_live_weather_info {
    int status;
    int count;
    char info[8];
    int infocode;

    char province[32];
    char city[32];
    int adcode;
    char weather[32];
    int temperature;
    char winddirection[16];
    char windpower[16];
    int humidity;
    char reporttime[32];
} gaud_live_weather_info;


int gaud_live_weather_info_get(gaud_live_weather_info *weather_info,
                               char *msg);

#endif