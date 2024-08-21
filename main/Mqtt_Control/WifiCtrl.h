/**
 * @file WifiCtrl.h
 * @author Tiago Zenerato(dr.zenerato@gmail.com)
 * @brief Biblioteca para gerenciamento de conexão Wi-Fi utilizando ESP-IDF.
 *
 * @details
 * Esta biblioteca oferece funcionalidades para inicialização da conexão Wi-Fi,
 * conexão a uma rede Wi-Fi, desconexão e verificação do estado de conexão.
 *
 * @version 0.1
 * @date 2024-08-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// language libraries (C).
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Framework Libraries (ESP-IDF).
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

// rtos libraries (FreeRTOS).
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define MAXIMUM_RETRY 5         // Definição da quantidade de tentativas de conexão
#define WIFI_CONNECTED_BIT BIT0 // Definição do estado de conectado
#define WIFI_FAIL_BIT BIT1      // Definição do estado de desconectado

// Funções para uso externo
esp_err_t wifi_manager_init(void);
bool wifi_manager_is_connected(void);
esp_err_t wifi_manager_disconnect(void);
esp_err_t wifi_manager_connect(const char *ssid, const char *password);

#endif // WIFI_MANAGER_H
