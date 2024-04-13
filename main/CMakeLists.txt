file(GLOB UI_SRC  ./display_task/*)
file(GLOB DRV_SRC  ./driver/*)
file(GLOB WEARTHER_HTTP_SRC  ./weather_task/*)
file(GLOB MSG_TASK_SRC  ./msg_task/*)
file(GLOB COMMON_SRC  ./common/*)
# file(GLOB BIND_TASK  ./net_connect_task/*)
set(srcs "main.c")

idf_component_register(SRCS ${srcs} ${UI_SRC} ${DRV_SRC} ${WEARTHER_HTTP_SRC} ${BIND_TASK} ${MSG_TASK_SRC} ${COMMON_SRC}
                    INCLUDE_DIRS "." "display_task" "driver" "weather_task" "net_connect_task" "common" "msg_task")
spiffs_create_partition_image(storage ./res FLASH_IN_PROJECT)

nvs_create_partition_image(nvs ../nvs_data.csv FLASH_IN_PROJECT)
