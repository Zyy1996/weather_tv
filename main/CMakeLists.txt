file(GLOB UI_SRC  ./display_task/*)
file(GLOB DRV_SRC  ./driver/*)
set(srcs "main.c")

idf_component_register(SRCS ${srcs} ${UI_SRC} ${DRV_SRC}
                    INCLUDE_DIRS "." "display_task" "driver")
spiffs_create_partition_image(storage ./res FLASH_IN_PROJECT)
