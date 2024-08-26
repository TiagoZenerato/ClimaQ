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
#include "esp_timer.h"
#include "esp_system.h"
#include "led_strip.h"
#include "led_strip_interface.h"
#include "../main.h"

#define LED_STRIP_BLINK_GPIO 48                 // GPIO assignment
#define LED_STRIP_LED_NUMBERS 1                 // Numbers of the LED in the strip
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000) // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)

#define BLINK_TIME_1 1000
#define BLINK_TIME_2 300

// Estados possíveis para o led
typedef enum
{
    OFF,
    RED,
    GREEN,
    BLUE,
    YELLOW,
} LED_RGB_STATE;

// Modos de operação para o led
typedef enum
{
    WAITING_CONNECT,
    CONNECTED,
    ERRO,
    NEW_DATA_SEND,
    RECEIVED_COMMAND,
    BLINK_NOW_STATE
} LED_RGB_MODE;

// funções de uso externo
esp_err_t led_ctrl_init(void);
uint8_t led_ctrl_get_mode(void);
uint8_t led_ctrl_get_state(void);
void led_ctrl_random_color(void);
void led_ctrl_set_mode(uint8_t mode);
void led_ctrl_app(void *pvParameters); 
void led_ctrl_set_state(uint8_t state);
void led_ctrl_toggle_blink_time(void);

#endif
