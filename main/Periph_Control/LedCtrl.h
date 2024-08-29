/**
 * @file LedCtrl.h
 * @author Tiago Zenerato (dr.zenerato@gmail.com)
 * @brief Controle de LEDs endereçáveis WS2812B.
 * 
 * Este arquivo implementa as funções para controle de LEDs endereçáveis WS2812B,
 * incluindo configuração, controle de cores e modos de operação do LED.
 * 
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef LED_CTRL_H
#define LED_CTRL_H

// Bibliotecas da linguagem (C).
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// Bibliotecas do RTOS (FreeRTOS).
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Bibliotecas da Estrutura (ESP-IDF).
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "led_strip.h"
#include "led_strip_interface.h"

#define BLINK_TIME_1 1000                       /**< Tempo de piscar 1 em milissegundos. */
#define BLINK_TIME_2 300                        /**< Tempo de piscar 2 em milissegundos. */
#define LED_STRIP_BLINK_GPIO 48                 /**< GPIO assignment. */
#define LED_STRIP_LED_NUMBERS 1                 /**< Number of LEDs in the strip. */
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000) /**< 10MHz resolution, 1 tick = 0.1us (LED strip needs a high resolution). */

/**
 * @brief Estados possíveis para o LED RGB.
 */
typedef enum
{
    OFF,    /**< LED desligado. */
    RED,    /**< LED vermelho. */
    GREEN,  /**< LED verde. */
    BLUE,   /**< LED azul. */
    YELLOW, /**< LED amarelo. */
} LED_RGB_STATE;

/**
 * @brief Modos de operação para o LED RGB.
 */
typedef enum
{
    WAITING_CONNECT,  /**< Esperando conexão. */
    CONNECTED,        /**< Conectado. */
    ERRO,             /**< Erro. */
    NEW_DATA_SEND,    /**< Novo dado enviado. */
    RECEIVED_COMMAND, /**< Comando recebido. */
    BLINK_NOW_STATE,  /**< Piscar no estado atual. */
    CLEAR_MODE        /**< Limpar modo. */
} LED_RGB_MODE;

// Funções de uso externo.
esp_err_t led_ctrl_init(void);
uint8_t led_ctrl_get_mode(void);
uint8_t led_ctrl_get_state(void);
void led_ctrl_toggle_color(void);
void led_ctrl_set_mode(uint8_t mode);
void led_ctrl_toggle_blink_time(void);
void led_ctrl_app(void *pvParameters);
void led_ctrl_set_state(uint8_t state);
uint32_t led_ctrl_get_time_use_blink(void);

#endif
