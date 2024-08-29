/**
 * @file ButtonCtrl.h
 * @author Tiago Zenerato (dr.zenerato@gmail.com)
 * @brief Controle de botão com debounce e detecção de pressionamento longo.
 *
 * Este arquivo implementa as funcionalidades necessárias para o controle de um botão,
 * incluindo debounce, detecção de bordas, e temporização para diferenciar entre um
 * pressionamento curto e um pressionamento longo.
 *
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef BUTTON_CTRL_H
#define BUTTON_CTRL_H

// Bibliotecas da linguagem (C).
#include <stdio.h>
#include <stdbool.h>

// Bibliotecas do RTOS (FreeRTOS).
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Bibliotecas da Estrutura (ESP-IDF).
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_timer.h"

// Bibliotecas do projeto (climaQ).
#include "LedCtrl.h"

#define BUTTON_GPIO 0           /**< Defina o número do pino do botão.*/
#define ESP_INTR_FLAG_DEFAULT 0 /**< Defina o nível de interrupção (Rising ou Falling Edge).*/

/**
 * @brief Definindo a enumeração para os modos de pressão do botão.
 * 
 */
typedef enum
{
    BUTTON_PRESS_NONE,  /**< Sem pressionamento.*/
    BUTTON_PRESS_SHORT, /**< Pressionamento rápido (abaixo de 1 seg). */
    BUTTON_PRESS_LONG   /**< Pressionamento longo (acima de 3 seg).*/
} button_press_mode_t;

// Funções para uso externo.
extern void init_button(void);
extern void button_ctrl_app(void *pvParameters);

#endif