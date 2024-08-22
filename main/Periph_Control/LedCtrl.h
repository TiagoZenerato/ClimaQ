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
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_err.h"

// Estrutura para armazenar as configurações do LED
typedef struct
{
    int gpio_num;   // GPIO onde o LED está conectado
    int channel;    // Canal PWM utilizado
    int resolution; // Resolução do PWM
    int frequency;  // Frequência do PWM
} led_ctrl_config_t;

extern led_ctrl_config_t led_config;
esp_err_t led_ctrl_off(void);                                           // Função para desligar o LED
esp_err_t led_ctrl_init(led_ctrl_config_t *config);                     // Função para inicializar o controle do LED
esp_err_t led_ctrl_set_color(uint8_t red, uint8_t green, uint8_t blue); // Função para definir a cor do LED (em formato RGB)

#endif
