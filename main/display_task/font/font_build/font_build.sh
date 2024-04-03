#!/bin/bash

rm output.bin
env DEBUG=* lv_font_conv --font ./MiSansW-Demibold.otf --bpp 2 --size 36 --format bin  --symbols 1234567890   -o output.bin
env DEBUG=* lv_font_conv --font ./MiSansW-Heavy.otf --bpp 2 --size 36 --format bin  --symbols 1234567890   -o output1.bin

env DEBUG=* lv_font_conv --font ./MiSansW-Medium.otf --bpp 2 --size 44 --format bin  --symbols 用米家App连接摄像机请先用手机下载\n米家App后连接摄像机在米家APP  -o font_medium_44_cn.bin