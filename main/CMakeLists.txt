file(GLOB UI_SRC  ./display_task/*)
file(GLOB DRV_SRC  ./driver/*)
file(GLOB WEARTHER_HTTP_SRC  ./weather_task/*)
set(srcs "main.c")

idf_component_register(SRCS ${srcs} ${UI_SRC} ${DRV_SRC} ${WEARTHER_HTTP_SRC}
                    INCLUDE_DIRS "." "display_task" "driver" "weather_task")
spiffs_create_partition_image(storage ./res FLASH_IN_PROJECT)
