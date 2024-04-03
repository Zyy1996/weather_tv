import sys
import pandas as pd
import numpy as np
import os
import re
import json

'''
const_font.xlsx:生成常量字符串的bin文件，并且字符串是打包到c代码的
get_bin.xlsx：生成常量字符串的bin文件，但是常量字符串是打包到c代码的
font_format:生成的字体格式：bin，lvgl
'''

with open('./font_build_config.json', 'r') as f:
    json_data = json.load(f)

HEADER=json_data["out_path"] + "custom_font.h"
SOURCE=json_data["out_path"] + "custom_font.c"

# MiSansW-Demibold.otf
# MiSansW-Heavy.otf
# MiSansW-Light.otf
# MiSansW-Medium.otf
# MiSansW-Regular.otf
# Mitype2018-50.otf

const_list_array = []
excel_c_dic ={}
country_list = []
country_list1 = []

def macro_defintion_name_to_param(macro_name):
    if(str(macro_name).find("FONT_DEMIBOLD_") >=0):
        font_name = "MiSansW-Demibold.otf "
    elif (str(macro_name).find("FONT_HEAVY_") >=0):
        font_name = "MiSansW-Heavy.otf"
    elif (str(macro_name).find("FONT_LIGHT_") >=0):
        font_name ="MiSansW-Light.otf"
    elif (str(macro_name).find("FONT_MEDIUM_") >=0):
        font_name ="MiSansW-Medium.otf" 
    elif (str(macro_name).find("FONT_REGULAR_") >=0):
        font_name ="MiSansW-Regular.otf"
    elif (str(macro_name).find("FONT_MITYPE_") >=0):
        font_name ="Mitype2018-50.otf"
    elif (str(macro_name).find("FONT_NORMAL_") >=0):
        font_name ="MiSansW-Normal.otf"
    elif (str(macro_name).find("FONT_BOLD_") >=0):
        font_name ="MiSansW-Bold.otf"
    elif (str(macro_name).find("FONT_A_P_REGULAR_") >=0):
        font_name ="Alibaba_PuHuiTi_2.0_55_Regular_55_Regular.ttf"
    elif (str(macro_name).find("FONT_A_S_BOLD_") >=0):
        font_name ="AlimamaShuHeiTi-Bold.otf"
    elif (str(macro_name).find("FONT_A_P_SEMIBOLD_") >=0):
        font_name ="Alibaba_PuHuiTi_2.0_75_SemiBold_75_SemiBold.ttf"
    else:
        print("------------err---1-------------------")
        return None
    num_data = re.findall(r"\d+",macro_name)
    return [font_name,num_data[0]]

def get_macro_definition_name(font_name,font_size):
    if(str(font_name).find("MiSansW-Demibold.otf") >=0):
        macro = "FONT_DEMIBOLD_" + str(font_size)
        return macro
    elif (str(font_name).find("MiSansW-Heavy.otf") >=0):
        macro = "FONT_HEAVY_" + str(font_size)
        return macro
    elif (str(font_name).find("MiSansW-Light.otf") >=0):
        macro = "FONT_LIGHT_" + str(font_size)
        return macro
    elif (str(font_name).find("MiSansW-Medium.otf") >=0):
        macro = "FONT_MEDIUM_" + str(font_size)
        return macro
    elif (str(font_name).find("MiSansW-Regular.otf") >=0):
        macro = "FONT_REGULAR_" + str(font_size)
        return macro
    elif (str(font_name).find("MiSansW-Bold.otf") >=0):
        macro = "FONT_BOLD_" + str(font_size)
        return macro
    elif (str(font_name).find("MiSansW-Normal.otf") >=0):
        macro = "FONT_NORMAL_" + str(font_size)
        return macro
    elif (str(font_name).find("Alibaba_PuHuiTi_2.0_55_Regular_55_Regular.ttf") >=0):
        macro = "FONT_A_P_REGULAR_" + str(font_size)
        return macro
    elif (str(font_name).find("AlimamaShuHeiTi-Bold.otf") >=0):
        macro = "FONT_A_S_BOLD_" + str(font_size)
        return macro
    elif (str(font_name).find("Alibaba_PuHuiTi_2.0_75_SemiBold_75_SemiBold.ttf") >=0):
        macro = "FONT_A_P_SEMIBOLD_" + str(font_size)
        return macro
    else:
        print("------------err-----3-----------------")
        return None

def write_country(country):
    enum_country_str = "\nenum { \n"
    for i in range(int(len(country)/2)):
        enum_country_str += "COUNTRY_" + str(country[i*2+1]).upper() + ",\n"
    enum_country_str += "COUNTRY_MAX_NUM\n};\n"
    return enum_country_str


# 写字体的宏定义
def write_font_macro(dic):
    t = dict(dic)
    list_t = []
    for key in t:
        list_t.append(key)
    list.sort(list_t,reverse= 0)
    font_macro = "\nenum {\n"
    for data in list_t:
        font_macro +="    "+ data + ",\n"
    font_macro +=  "    FONT_MAX_NUM\n};\n\n"
    return font_macro

def get_country_list(country):
    global country_list
    for i in range(int(len(country)/2)):
        country_list.append("COUNTRY_" + str(country[i*2+1]).upper())
    print("country list:",country_list)

def get_country_list1(country):
    global country_list1
    for i in range(int(len(country)/2)):
        country_list1.append("COUNTRY_" + str(country[i*2+1]).upper())
    print("country list:",country_list1)

df = pd.read_excel(json_data["excel_const_font"])
df1 = pd.read_excel(json_data["excel_bin_font"])

get_country_list(df.columns.values)
get_country_list1(df1.columns.values)

def strtofont(font_str,const_str,country_id):
    global excel_c_dic
    if pd.isna(font_str):
        return
    elif pd.isna(const_str):
        return
    elif pd.isna(country_id):
        return
    const_str = str(const_str).replace("\n","---")
    const_str = str(const_str).replace("\r","---")
    print("-----1-------",const_str,"-------2------")
    str_list = re.split(r"\s+",font_str)
    for str_l in str_list:
        sing_font = str(str_l).split(",")
        for font_size in sing_font[1:]:
            font_name = get_macro_definition_name(sing_font[0],font_size)
            if font_name != None:
                try:
                    dic_curr_str = excel_c_dic[font_name]
                    try:
                        country_font_str  = dic_curr_str[country_id]
                        country_font_str = str(country_font_str) + const_str
                        excel_c_dic[font_name][country_id] = country_font_str
                    except:
                        excel_c_dic[font_name][country_id] = const_str
                except:
                    excel_c_dic[font_name] = {}
                    excel_c_dic[font_name][country_id] = const_str
            else:
                pass

for i in df.index.values:
    tmp_list = []
    for j in range(int(len(df.columns.values)/2)):
        strtofont(df.values[i,j*2],df.values[i,j*2+1],country_list[j])
        tmp_list.append(df.values[i,j*2+1])
    const_list_array.append(tmp_list)

print(excel_c_dic)

for i in df1.index.values:
    for j in range(int(len(df1.columns.values)/2)):
        print(i,j)
        strtofont(df1.values[i,j*2],df1.values[i,j*2+1],country_list1[j])

print(excel_c_dic)

def listarraytoccode(const_list_array:list):
    c_const_data = "\nchar* font_single_str[] = {\n"
    m = 0
    for index in range(int(len(const_list_array))):
        if m != 0:
            c_const_data += ",\n"
        c_const_data += "[" + str(index) +"] = " +  "{"
        t = 0
        for s in const_list_array[index]:
            if t == 0:
                c_const_data += "\""+ s + "\""
            else:
                c_const_data += "," + "\""+ s + "\""
            t = t+1
        c_const_data +="}"
        m = m+1
    c_const_data +="\n};\n"
    return c_const_data

def write_c_country_name(country):
    country_name_str = "\nconst char *country_name[COUNTRY_MAX_NUM]={\n"
    for i in range(int(len(country)/2)):
       country_name_str +=  "[COUNTRY_" + str(country[i*2+1]).upper() +"] =" + "\""+str(country[i*2+1]).lower() + "\""+ ",\n"
    country_name_str +="};\n"
    return country_name_str

def write_c_bin_path(dic_data):
    country_font_bin_str = "\nconst char* font_bin_load_path[FONT_MAX_NUM] = {\n"
    t = dict(dic_data)
    list_t = []
    for key in t:
        list_t.append(key)
    list.sort(list_t,reverse= 0)
    for font_type in list_t:
            country_font_bin_str += "["+font_type+"]" +" = " + "\"%s"+str(font_type).lower() + "_" + "%s" + ".bin"+"\"" + ",\n"
    country_font_bin_str +="};\n"
    return country_font_bin_str

# 生成头文件
with open(HEADER,"w") as f_h:
    f_h.write("#ifndef _CUSTOM_FONT_H_\n#define _CUSTOM_FONT_H_\n")
    f_h.write(write_country(df.columns.values))
    f_h.write("\n#define country_id_t int\n")
    f_h.write(write_font_macro(excel_c_dic))
    f_h.write("\n#define font_id_t int\n")
    if(json_data["font_format"] == "bin"):
        f_h.write("\ntypedef struct font_single_str {\n    char *single_str[COUNTRY_MAX_NUM];\n} font_single_str_t;\n")
    f_h.write("\n#endif\n")

# 生成c文件
with open(SOURCE,"w") as f_c:
    f_c.write("#include \"custom_font.h\"\n#include \"ui_manage.h\"\n#include <stdio.h>\n")
    f_c.write("\nconst char *en_month[] = {\n    \"Jan\", \"Feb\", \"Mar\", \"Apr\", \"May\", \"Jun\", \"Jul\", \"Aug\", \"Sep\", \"Oct\", \"Nov\", \"Dec\"};\n")
    f_c.write("\nconst char *cn_week[] = {\"日\", \"一\", \"二\", \"三\", \"四\", \"五\", \"六\"};\n")
    f_c.write("\nconst char *en_week[] = {\n    \"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"};\n")
    if(json_data["font_format"] == "bin"):
        f_c.write("\nlv_ft_info_t ui_font[COUNTRY_MAX_NUM][FONT_MAX_NUM] = {0};\n")
    f_c.write(listarraytoccode(const_list_array))
    if json_data["font_format"] == "bin":
        f_c.write(write_c_country_name(df.columns.values))
        f_c.write(write_c_bin_path(excel_c_dic))

os.system("rm ./font_build/font_*.c")
os.system("rm ./font_build/*.bin")

for key,val in excel_c_dic.items():
    font_size_str = macro_defintion_name_to_param(key)
    for key1, value in val.items():
            if(json_data["font_format"] == "bin"):
                bin_name_str =json_data["out_path"]+"/" + str(key).lower() + "_" + str(key1[8:]).lower() + ".bin"
            else:
                bin_name_str =json_data["out_path"]+"/" + str(key).lower() + "_" + str(key1[8:]).lower() + ".c"
            symbol_str = ""
            for sym in value:
                if(pd.isna(sym)):
                    pass
                else:
                    symbol_str += sym
            cmd_str = "env DEBUG=* lv_font_conv --font ./{} --bpp 2 --size {} --format {}  --symbols \"{}\"   -o {}".format(font_size_str[0],font_size_str[1],json_data["font_format"],symbol_str,bin_name_str)
            print(cmd_str)
            ret = os.system(cmd_str)
            if(ret != 0):
                print("源代码和bin文件生成失败")
                sys.exit(-1)
            else:
                print("源代码和bin文件生成成功")