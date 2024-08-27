/**
 * @file MqttCtrl.h
 * @author Tiago Zenerato (dr.zenerato@gmail.com)
 * @brief Implementação da biblioteca para gerenciamento de MQTT com ESP32-S3.
 *
 * @details
 * Esta biblioteca fornece funções para inicializar a conexão MQTT, publicar e se inscrever em tópicos.
 *
 * @version 0.1
 * @date 2024-08-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MQTT_CTRL_H
#define MQTT_CTRL_H

// language libraries (C).
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// rtos libraries (FreeRTOS).
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Framework Libraries (ESP-IDF).
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "cJSON.h"

// Detalhes do broker MQTT
extern const int mqtt_port;                        
extern const char *mqtt_server;     

// Funções para uso externo
void mqtt_deinit(void);
esp_err_t mqtt_subscribe(const char *topic);
esp_err_t mqtt_unsubscribe(const char *topic);
esp_err_t mqtt_init(const char *uri, uint16_t port_use_connect);
esp_err_t mqtt_publish(const char *topic, const char *payload, int len);

#endif