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
static const char *TAG_REPORT = "[SYS-INFO]";

// Conectar-se a uma rede Wi-Fi
const char *ssid = "ZENA2007";
const char *password = "m130856z";

// Detalhes do broker MQTT
const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 8884;                         // Porta fornecida
const char *mqtt_client_id = "clientId-TXAQggAfwO"; // ClientID fornecido

dht22_data_t sensor_data;

void status_report_app(void *pvParameters)
{
    uint8_t cont_report_send = 0;
    while (true)
    {
        printf("\e[1;1H\e[2J"); // limpar todo o terminal
        ESP_LOGI(TAG_REPORT, "(REPORT_NUM: %i) ---> |ClimaQ| ----> |VER: %i| -----> |P2G-\u2665| ", cont_report_send++, VERSION_CTRL_FW);
        vTaskDelay(pdMS_TO_TICKS(3000)); // A cada 3s
    }
}

/**
 * @brief Set the AllTasksCore 0 object
 *
 */
void setAllTasksCore_0(void)
{
    // xTaskCreatePinnedToCore(led_ctrl_app, "TaskLedRGB", 4096, NULL, 1, NULL, 0); // Led RGB (NeoPixel) service
    xTaskCreatePinnedToCore(status_report_app, "TaskStatusReport", 2048, NULL, 1, NULL, 0); //
}

/**
 * @brief Set the AllTasksCore 1 object
 *
 */
void setAllTasksCore_1(void)
{
    xTaskCreatePinnedToCore(led_ctrl_app, "TaskLedRGB", 4096, NULL, 1, NULL, 0); // Led RGB (NeoPixel) service
    // xTaskCreatePinnedToCore(serviceRGB, "TaskLedRGB", 2048, NULL, 1, NULL, 1); // Led RGB (NeoPixel) service
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

    // Inscreva-se em um tópico
    if (mqtt_subscribe("test/topic") != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to subscribe to topic");
        return ESP_FAIL;
    }

    // Publique uma mensagem
    if (mqtt_publish("test/topic", "Hello, world!", strlen("Hello, world!")) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to publish message");
        return ESP_FAIL;
    }

    // Lembre-se de desalocar recursos ao final
    mqtt_deinit();

    return ESP_OK;
}

int get_random_number(int min, int max)
{
    // Return a random number in the range [min, max)
    return min + (esp_random() % (max - min));
}

void dht_sensor_app(void)
{
    gpio_num_t pin = GPIO_NUM_5;
    esp_err_t result = dht_read_float_data(DHT_TYPE_AM2301, pin, &sensor_data.humidity, &sensor_data.temperature);
    if (result == ESP_OK)
    {
        ESP_LOGI("DHT22", "Temperature: %.1f C, Humidity: %.1f %%", sensor_data.temperature, sensor_data.humidity);
    }
    else
    {
        ESP_LOGE("DHT22", "Failed to read from DHT22 sensor");
    }
    vTaskDelay(pdMS_TO_TICKS(300));
}

void app_main(void)
{
    init_button();

    if (led_ctrl_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "A configuracao do led RGB falhou..");
    }

    setAllTasksCore_0();
    setAllTasksCore_1();

    if (mqtt_start_all() != ESP_OK)
    {
        ESP_LOGE(TAG, "A inicializacao do serviço de mqtt/wifi falhou...");
        led_ctrl_set_mode(ERRO);
    }

    while (true)
    {
        if (btState)
        {
            led_ctrl_set_mode(RECEIVED_COMMAND);
            vTaskDelay(pdMS_TO_TICKS(20));
            led_ctrl_set_state(get_random_number(0, 5));
            btState = false;
        }

        dht_sensor_app();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}