/**
 * @file main.h
 * @author Tiago Zenerato (dr.zenerato@gmail.com)
 * @brief Arquivo principal da aplicação ClimaQ.
 * @version 0.1
 * @date 2024-08-18
 * 
 * @copyright Copyright (c) 2024
 * 
 * Este arquivo contém a lógica principal da aplicação ClimaQ, incluindo a configuração de tarefas,
 * leitura de sensores, controle de LEDs e comunicação via MQTT.
 */

#ifndef MAIN_H
#define MAIN_H

#define VERSION_CTRL_FW 117 /**< Número da versão do Firmware*/

// Bibliotecas da linguagem (C).
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <cJSON.h>

// Bibliotecas do RTOS (FreeRTOS).
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Bibliotecas da Estrutura (ESP-IDF).
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "dht.h"

// Bibliotecas do projeto (climaQ).
#include "Mqtt_Control/WifiCtrl.h"
#include "Mqtt_Control/MqttCtrl.h"
#include "Periph_Control/ButtonCtrl.h"
#include "Periph_Control/LedCtrl.h"

/**
 * @brief Estrutura de dados do sensor dht22.
 * 
 */
typedef struct
{
    float temperature; /**< Temperatura lida do sensor.*/
    float humidity;  /**< Umidade lida do sensor.*/
} dht22_data_t;

#endif