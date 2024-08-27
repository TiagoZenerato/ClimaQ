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

// String utilizada para identificar as mensagens de log.
static const char *TAG = "MAIN";
static const char *TAG_REPORT = "[SYS-INFO]";

dht22_data_t sensor_data;

#ifndef INTERNAL_FUNCTIONS
/**
 * @brief
 *
 * @param pvParameters
 */
void status_report_app(void *pvParameters)
{
    uint8_t cont_report_send = 0;
    const char *Led_Color = "";
    const char *Led_State = "";
    uint32_t timer_for_blink = 0;

    while (true)
    {
        printf("\e[1;1H\e[2J"); // limpar todo o terminal
        cont_report_send = (cont_report_send >= 253) ? 0 : cont_report_send + 1;
        ESP_LOGI(TAG_REPORT, "(REPORT_NUM: %i) --> |ClimaQ| --> |VER: %i| --> |P2G-\u2665|", cont_report_send, VERSION_CTRL_FW);
        ESP_LOGI(TAG_REPORT, "Temperature: %.1f C, Humidity: %.1f %%", sensor_data.temperature, sensor_data.humidity);

        switch (led_ctrl_get_mode())
        {
        case WAITING_CONNECT:
            Led_State = "WAITING_CONNECT";
            break;
        case CONNECTED:
            Led_State = "CONNECTED";
            break;
        case ERRO:
            Led_State = "ERRO";
            break;
        case NEW_DATA_SEND:
            Led_State = "NEW_DATA_SEND";
            break;
        case RECEIVED_COMMAND:
            Led_State = "RECEIVED_COMMAND";
            break;
        case BLINK_NOW_STATE:
            Led_State = "BLINK_NOW_STATE";
            timer_for_blink = led_ctrl_get_time_use_blink();
            break;
        case CLEAR_MODE:
            Led_State = "CLEAR_MODE";
            break;
        default:
            break;
        }

        switch (led_ctrl_get_state())
        {
        case OFF:
            Led_Color = "OFF";
            break;
        case RED:
            Led_Color = "RED";
            break;
        case GREEN:
            Led_Color = "GREEN";
            break;
        case BLUE:
            Led_Color = "BLUE";
            break;
        case YELLOW:
            Led_Color = "YELLOW";
            break;
        default:
            break;
        }

        if (led_ctrl_get_mode() == BLINK_NOW_STATE)
        {
            ESP_LOGI(TAG_REPORT, "Led color: %s, Led state: %s, Interval: %li", Led_Color, Led_State, timer_for_blink);
        }else{
            ESP_LOGI(TAG_REPORT, "Led color: %s, Led state: %s", Led_Color, Led_State);
        }

        vTaskDelay(pdMS_TO_TICKS(3000)); // A cada 3s
    }
}

/**
 * @brief
 *
 */
void dht_sensor_app(void *pvParameters)
{
    gpio_num_t pin = GPIO_NUM_5;

    while (true)
    {
        esp_err_t result = dht_read_float_data(DHT_TYPE_AM2301, pin, &sensor_data.humidity, &sensor_data.temperature);
        if (result == ESP_OK)
        {
            // ESP_LOGI("DHT22", "Temperature: %.1f C, Humidity: %.1f %%", sensor_data.temperature, sensor_data.humidity);
        }
        else
        {
            ESP_LOGE("DHT22", "Failed to read from DHT22 sensor");
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

/**
 * @brief Set the AllTasksCore 0 object
 *
 */
void setAllTasksCore_0(void)
{
    // Atividades do core 0
    xTaskCreatePinnedToCore(button_ctrl_app, "Task-Button-App", 4096, NULL, 1, NULL, 0);          // serviço de controle para o interrupção do botão
    xTaskCreatePinnedToCore(status_report_app, "Task-Status-Report-App", 4096, NULL, 2, NULL, 0); // serviço de relatório do status da aplicação
}

/**
 * @brief Set the AllTasksCore 1 object
 *
 */
void setAllTasksCore_1(void)
{
    // Atividades do core 1
    xTaskCreatePinnedToCore(led_ctrl_app, "Task-Led-RGB-App", 4096, NULL, 1, NULL, 1); // Led RGB (NeoPixel) service
    xTaskCreatePinnedToCore(dht_sensor_app, "Task-Dht22-App", 4096, NULL, 2, NULL, 1); // Led RGB (NeoPixel) service
}

/**
 * @brief
 *
 * @return ESP_OK
 * @return ESP_FAIL
 */
esp_err_t wifi_start_all(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    // Inicializar o Wi-Fi manager
    if (wifi_manager_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi manager");
        led_ctrl_set_mode(ERRO);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Attempting to connect to SSID: %s", ssid);
    if (wifi_manager_connect(ssid, password) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
        return ESP_FAIL;
    }

    // Verificar se está conectado
    if (wifi_manager_is_connected())
    {
        ESP_LOGI(TAG, "Successfully connected to Wi-Fi");
        return ESP_OK;
    }
    else
    {
        ESP_LOGI(TAG, "Not connected to Wi-Fi");
        return ESP_FAIL;
    }
}

/**
 * @brief
 *
 * @return ESP_OK
 * @return ESP_FAIL
 */
esp_err_t mqtt_start_all(void)
{
    // iniciando o wifi.
    if (wifi_start_all())
    {
        ESP_LOGE(TAG, "Failed to initialize WIFI");
        return ESP_FAIL;
    }

    // Inicialize o cliente MQTT
    if (mqtt_init(mqtt_server, mqtt_port) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT");
        return ESP_FAIL;
    }

    // // Inscreva-se em um tópico
    if (mqtt_subscribe("climaQ/TrocarCor") != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to subscribe to topic");
        return ESP_FAIL;
    }

    // Publique uma mensagem
    if (mqtt_publish("climaQ/report", "Hello, world!", strlen("Hello, world!")) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to publish message");
        return ESP_FAIL;
    }

    // Lembre-se de desalocar recursos ao final
    // mqtt_deinit();

    return ESP_OK;
}

#endif

void app_main(void)
{

    if (led_ctrl_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "A configuracao do led RGB falhou..");
    }

    init_button();

    setAllTasksCore_0();
    setAllTasksCore_1();

    if (mqtt_start_all() != ESP_OK)
    {
        ESP_LOGE(TAG, "A inicializacao do serviço de mqtt/wifi falhou...");
        led_ctrl_set_mode(ERRO);
    }
    else
    {
        led_ctrl_set_mode(CONNECTED);
    }
}