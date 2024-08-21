/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.h"

static const char *TAG = "MAIN";

// Conectar-se a uma rede Wi-Fi
const char *ssid = "ZENA2007";
const char *password = "m130856z";

void setAllTasksCore_0(void)
{
   // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 4096, NULL, 1, NULL, 0); // Led RGB (NeoPixel) service
                                                                               // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 2048, NULL, 1, NULL, 0); // Led RGB (NeoPixel) service
}

void setAllTasksCore_1(void)
{
    // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 2048, NULL, 1, NULL, 1); // Led RGB (NeoPixel) service
    // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 2048, NULL, 1, NULL, 1); // Led RGB (NeoPixel) service
}

void wifi_start_all()
{
   ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    // Inicializar o Wi-Fi manager
    if (wifi_manager_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi manager");
        return;
    }

    ESP_LOGI(TAG, "Attempting to connect to SSID: %s", ssid);
    if (wifi_manager_connect(ssid, password) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
        return;
    }

    // Verificar se está conectado
    if (wifi_manager_is_connected()) {
        ESP_LOGI(TAG, "Successfully connected to Wi-Fi");
    } else {
        ESP_LOGI(TAG, "Not connected to Wi-Fi");
    }
}

void app_main(void)
{
    wifi_start_all();
    
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay para evitar uso excessivo da CPU
    }
    
}