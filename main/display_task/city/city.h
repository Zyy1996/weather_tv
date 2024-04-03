#ifndef _CITY_CODE_
#define _CITY_CODE_

typedef struct _city_code
{
    char * city_str;
    int ad_code;
    int city_code;
} city_code_t;

char * get_city_str(int adcode);

#endif