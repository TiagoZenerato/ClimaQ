/**
 * @file LedCtrl.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "LedCtrl.h"

static const char *TAG = "LedCtrl";
static led_ctrl_config_t *led_config = {0};

led_strip_handle_t configure_led(void)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_BLINK_GPIO,   // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_NUMBERS,        // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
#else
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
#endif
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");

    return led_strip;
}

void led_blink_rgb_teste(void)
{
    bool led_on_off = false;

    ESP_LOGI(TAG, "Start blinking LED strip");
    while (1)
    {
        if (led_on_off)
        {
            /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
            for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++)
            {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 30, 0, 0));
            }
            /* Refresh the strip to send data */
            ESP_ERROR_CHECK(led_strip_refresh(led_strip));
            ESP_LOGI(TAG, "LED ON!");
        }
        else
        {
            /* Set all LED off to clear all pixels */
            ESP_ERROR_CHECK(led_strip_clear(led_strip));
            ESP_LOGI(TAG, "LED OFF!");
        }

        led_on_off = !led_on_off;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}