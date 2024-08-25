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

typedef struct
{
    float temperature;
    float humidity;
} dht22_data_t;

// Prototipos das funções
esp_err_t dht22_init(gpio_num_t pin);

#endif