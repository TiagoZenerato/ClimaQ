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

// Detalhes do broker MQTT
const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 8884;                         // Porta fornecida
const char *mqtt_client_id = "clientId-TXAQggAfwO"; // ClientID fornecido

#define LED_STRIP_RMT_CHANNEL    RMT_CHANNEL_0
#define LED_STRIP_GPIO           48  // Pino GPIO onde o WS2812 está conectado
#define LED_STRIP_NUM_PIXELS     1   // Número de LEDs



/**
 * @brief Set the AllTasksCore 0 object
 *
 */
void setAllTasksCore_0(void)
{
    // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 4096, NULL, 1, NULL, 0); // Led RGB (NeoPixel) service
    // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 2048, NULL, 1, NULL, 0); // Led RGB (NeoPixel) service
}

/**
 * @brief Set the AllTasksCore 1 object
 *
 */
void setAllTasksCore_1(void)
{
    // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 2048, NULL, 1, NULL, 1); // Led RGB (NeoPixel) service
    // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 2048, NULL, 1, NULL, 1); // Led RGB (NeoPixel) service
}

/**
 * @brief
 *
 * @return true
 * @return false
 */
bool wifi_start_all(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    // Inicializar o Wi-Fi manager
    if (wifi_manager_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi manager");
        return false;
    }

    ESP_LOGI(TAG, "Attempting to connect to SSID: %s", ssid);
    if (wifi_manager_connect(ssid, password) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
        return false;
    }

    // Verificar se está conectado
    if (wifi_manager_is_connected())
    {
        ESP_LOGI(TAG, "Successfully connected to Wi-Fi");
        return true;
    }
    else
    {
        ESP_LOGI(TAG, "Not connected to Wi-Fi");
        return false;
    }
}

/**
 * @brief
 *
 * @return true
 * @return false
 */
bool mqtt_start_all(void)
{
    // Inicialize o cliente MQTT
    if (mqtt_init(mqtt_server, mqtt_port) != ESP_OK)
    {
        ESP_LOGE("MAIN", "Failed to initialize MQTT");
        return false;
    }

    // Inscreva-se em um tópico
    if (mqtt_subscribe("test/topic") != ESP_OK)
    {
        ESP_LOGE("MAIN", "Failed to subscribe to topic");
        return false;
    }

    // Publique uma mensagem
    if (mqtt_publish("test/topic", "Hello, world!", strlen("Hello, world!")) != ESP_OK)
    {
        ESP_LOGE("MAIN", "Failed to publish message");
        return false;
    }

    // Lembre-se de desalocar recursos ao final
    mqtt_deinit();

    return true;
}

bool led_onBoard_set_and_init(void)
{
    led_strip_handle_t led_strip = configure_led();

    return true;
}


void app_main(void)
{
    
    bool led_on_off = false;

    ESP_LOGI(TAG, "Start blinking LED strip");
    wifi_start_all();
    mqtt_start_all();
    while (1) {
        if (led_on_off) {
            /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
            for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++) {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 30, 0, 0));
            }
            /* Refresh the strip to send data */
            ESP_ERROR_CHECK(led_strip_refresh(led_strip));
            ESP_LOGI(TAG, "LED ON!");
        } else {
            /* Set all LED off to clear all pixels */
            ESP_ERROR_CHECK(led_strip_clear(led_strip));
            ESP_LOGI(TAG, "LED OFF!");
        }

        led_on_off = !led_on_off;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}