/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "sdkconfig.h"
#include <stdio.h>

#define LCD_HOST SPI2_HOST
#define PARALLEL_LINES CONFIG_EXAMPLE_LCD_FLUSH_PARALLEL_LINES
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL 1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_DATA0 7 /*!< for 1-line SPI, this also refereed as MOSI */
#define EXAMPLE_PIN_NUM_PCLK 6
#define EXAMPLE_PIN_NUM_CS 0
#define EXAMPLE_PIN_NUM_DC 19
#define EXAMPLE_PIN_NUM_RST 5
#define EXAMPLE_PIN_NUM_BK_LIGHT 18

#define EXAMPLE_LCD_H_RES 240
#define EXAMPLE_LCD_V_RES 240
#define EXAMPLE_LCD_CMD_BITS 8
#define EXAMPLE_LCD_PARAM_BITS 8

#define BUF_LEN (240 * 12)

static esp_lcd_panel_handle_t panel_handle = NULL;
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_map) {
    esp_lcd_panel_handle_t panel_handle1 = lv_display_get_user_data(disp);
    int len = (area->x2 + 1 - area->x1) * (area->y2 + 1 - area->y1);
    int i = 0;
    lv_draw_sw_rgb565_swap(color_map, len);
    esp_lcd_panel_draw_bitmap(panel_handle1, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);
    lv_display_flush_ready(disp);
}

static void increase_lvgl_tick(void *arg) { lv_tick_inc(2); }

void ui_task(void *param) {
    gpio_config_t bk_gpio_config = {.mode = GPIO_MODE_OUTPUT, .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT};
    // Initialize the GPIO of backlight
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    spi_bus_config_t buscfg = {.sclk_io_num = EXAMPLE_PIN_NUM_PCLK,
                               .mosi_io_num = EXAMPLE_PIN_NUM_DATA0,
                               .miso_io_num = -1,
                               .quadwp_io_num = -1,
                               .quadhd_io_num = -1,
                               .max_transfer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(uint16_t)};
#if CONFIG_EXAMPLE_LCD_SPI_8_LINE_MODE
    buscfg.data1_io_num = EXAMPLE_PIN_NUM_DATA1;
    buscfg.data2_io_num = EXAMPLE_PIN_NUM_DATA2;
    buscfg.data3_io_num = EXAMPLE_PIN_NUM_DATA3;
    buscfg.data4_io_num = EXAMPLE_PIN_NUM_DATA4;
    buscfg.data5_io_num = EXAMPLE_PIN_NUM_DATA5;
    buscfg.data6_io_num = EXAMPLE_PIN_NUM_DATA6;
    buscfg.data7_io_num = EXAMPLE_PIN_NUM_DATA7;
    buscfg.flags = SPICOMMON_BUSFLAG_OCTAL;
#endif
    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    // ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, false));
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL));
    lv_init();
    uint16_t *pdata1 = heap_caps_malloc(BUF_LEN * 2, MALLOC_CAP_DMA);
    uint16_t *pdata = heap_caps_malloc(BUF_LEN * 2, MALLOC_CAP_DMA);
    lv_display_t *ui_disp = lv_display_create(240, 240);
    lv_display_set_flush_cb(ui_disp, lvgl_flush_cb);
    lv_display_set_user_data(ui_disp, panel_handle);
    lv_display_set_buffers(ui_disp, pdata, pdata1, BUF_LEN * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);

    const esp_timer_create_args_t lvgl_tick_timer_args = {.callback = &increase_lvgl_tick, .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 2 * 1000));
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(0, 0, 0), 0);
    lv_obj_t *p = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(p, lv_color_make(0, 255, 0), 0);
    lv_obj_set_width(p, 100);
    lv_obj_set_height(p, 100);
    // lv_obj_center(p);
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
