/**
 * @file Dht22.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Dht22.h"

static const char *TAG = "DHT22";
static gpio_num_t dht22_pin;

esp_err_t dht22_init(gpio_num_t pin) {
   
    ESP_ERROR_CHECK();
    return ESP_OK;
}

