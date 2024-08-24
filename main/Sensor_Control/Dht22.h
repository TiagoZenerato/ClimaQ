/**
 * @file Dht22.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef DHT22_H
#define DHT22_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"

// Definição dos pinos e tipos
#define DHT22_GPIO        GPIO_NUM_5
#define DHT22_TIMEOUT     1000  // Timeout para leitura de bits

#define DHT22_LONG_TIMEOUT   2000
#define DHT22_SHORT_TIMEOUT  500

#define DHT22_PULLUP_DELAY  40  // Delay após pull-up em microssegundos
#define DHT22_LOW_PULSE      80  // Tempo do pulso baixo em microssegundos
#define DHT22_HIGH_PULSE     80  // Tempo do pulso alto em microssegundos
#define DHT22_BIT0           26  // Tempo para bit 0 em microssegundos
#define DHT22_BIT1           73  // Tempo para bit 1 em microssegundos
#define DHT22_TIMEOUT        1000 // Timeout em microssegundos

typedef struct {
    float temperature;
    float humidity;
} dht22_data_t;

// Prototipos das funções
esp_err_t dht22_init(gpio_num_t pin);
esp_err_t dht22_read(dht22_data_t *data);

#endif