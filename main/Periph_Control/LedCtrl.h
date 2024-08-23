/**
 * @file LedCtrl.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef LED_CTRL_H
#define LED_CTRL_H

// language libraries (C).
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// rtos libraries (FreeRTOS).
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Framework Libraries (ESP-IDF).
#include "esp_log.h"
#include "esp_err.h"
#include "led_strip.h"
#include "led_strip_interface.h"

#define LED_STRIP_BLINK_GPIO  48 // GPIO assignment
#define LED_STRIP_LED_NUMBERS 1 // Numbers of the LED in the strip
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000) // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)

led_strip_handle_t configure_led(void);

#endif
