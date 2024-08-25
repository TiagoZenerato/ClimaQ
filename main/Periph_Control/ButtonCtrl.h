/**
 * @file ButtonCtrl.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef BUTTON_CTRL_H
#define BUTTON_CTRL_H

// language libraries (C).
#include <stdio.h>
#include <stdbool.h>

// Framework Libraries (ESP-IDF).
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_attr.h" 

#define BUTTON_GPIO 0           // Defina o número do pino do botão
#define ESP_INTR_FLAG_DEFAULT 0 // Defina o nível de interrupção (Rising ou Falling Edge)

extern bool btState; 

extern void init_button(void);
extern void button_ctrl_app(void *pvParameters);

#endif