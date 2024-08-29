/**
 * @file main.c
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

#include "main.h"

static const char *TAG = "MAIN";              /**< Tag de log para mensagens gerais. */
static const char *TAG_REPORT = "[SYS-INFO]"; /**< Tag de log para mensagens de relatório. */

dht22_data_t sensor_data; /**< Estrutura para armazenar os dados do sensor DHT22. */

/**
 * @brief Tarefa responsável por gerar relatórios de status e publicar via MQTT.
 * 
 * Esta função cria um relatório JSON contendo informações sobre o estado do LED,
 * temperatura, umidade e outros dados relevantes, e o publica em um tópico MQTT.
 * 
 * @param pvParameters Ponteiro para parâmetros de tarefa (não utilizado).
 */
void status_report_app(void *pvParameters)
{
    const char *Led_Color = "";
    const char *Led_State = "";
    uint32_t timer_for_blink = 0;
    uint32_t cont_report_send = 0;

    while (true)
    {
        printf("\e[1;1H\e[2J"); // limpar todo o terminal
        cont_report_send = (cont_report_send >= 0x7FFFFFFE) ? 0 : cont_report_send + 1;
        ESP_LOGI(TAG_REPORT, "(REPORT_NUM: %li)--> |ClimaQ|--> |VER: %i|--> |P2G-\u2665|", cont_report_send, VERSION_CTRL_FW);
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
        }
        else
        {
            ESP_LOGI(TAG_REPORT, "Led color: %s, Led state: %s", Led_Color, Led_State);
        }

        // Criando o objeto JSON
        cJSON *report_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(report_json, "report_number", cont_report_send);
        cJSON_AddNumberToObject(report_json, "version", VERSION_CTRL_FW);
        cJSON_AddNumberToObject(report_json, "temperature", sensor_data.temperature);
        cJSON_AddNumberToObject(report_json, "humidity", sensor_data.humidity);
        cJSON_AddStringToObject(report_json, "led_color", Led_Color);
        cJSON_AddStringToObject(report_json, "led_state", Led_State);

        // Convertendo o JSON para string
        char *json_string = cJSON_Print(report_json);

        if (mqtt_publish("climaQ/report", json_string, strlen(json_string)) != ESP_OK)
        {
            ESP_LOGE(TAG_REPORT, "Failed to publish the JSON report.");
        }

        cJSON_Delete(report_json);
        free(json_string);

        vTaskDelay(pdMS_TO_TICKS(3000)); // A cada 3s
    }
}

/**
 * @brief Tarefa responsável por ler dados do sensor DHT22.
 * 
 * Esta função lê os dados de temperatura e umidade do sensor DHT22 e atualiza a estrutura
 * `sensor_data` com os valores lidos.
 * 
 * @param pvParameters Ponteiro para parâmetros de tarefa (não utilizado).
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
 * @brief Cria e configura tarefas no núcleo 0.
 * 
 * Esta função cria as tarefas que serão executadas no núcleo 0 do ESP32, incluindo o controle
 * de interrupção do botão e o relatório de status da aplicação.
 */
void setAllTasksCore_0(void)
{
    // Atividades do core 0
    xTaskCreatePinnedToCore(button_ctrl_app, "Task-Button-App", 4096, NULL, 1, NULL, 0);          // serviço de controle para o interrupção do botão
    xTaskCreatePinnedToCore(status_report_app, "Task-Status-Report-App", 4096, NULL, 2, NULL, 0); // serviço de relatório do status da aplicação
}

/**
 * @brief Cria e configura tarefas no núcleo 1.
 * 
 * Esta função cria as tarefas que serão executadas no núcleo 1 do ESP32, incluindo o controle
 * do LED RGB (NeoPixel) e a leitura do sensor DHT22.
 */
void setAllTasksCore_1(void)
{
    // Atividades do core 1
    xTaskCreatePinnedToCore(led_ctrl_app, "Task-Led-RGB-App", 4096, NULL, 1, NULL, 1); // Led RGB (NeoPixel) service
    xTaskCreatePinnedToCore(dht_sensor_app, "Task-Dht22-App", 4096, NULL, 2, NULL, 1); // Led RGB (NeoPixel) service
}

/**
 * @brief Inicializa e configura o Wi-Fi.
 * 
 * Esta função inicializa o gerenciador de Wi-Fi, tenta conectar à rede especificada e
 * verifica se a conexão foi bem-sucedida.
 * 
 * @return ESP_OK se a conexão for bem-sucedida, ESP_FAIL caso contrário.
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
 * @brief Inicializa e configura o cliente MQTT.
 * 
 * Esta função inicializa o cliente MQTT, conecta ao servidor e publica uma mensagem inicial.
 * Além disso, se inscreve em um tópico para receber comandos.
 * 
 * @return ESP_OK se a inicialização for bem-sucedida, ESP_FAIL caso contrário.
 */
esp_err_t mqtt_start_all(void)
{
    char initial_message[128];
    uint8_t initial_message_size = 0;

    initial_message_size = sprintf(initial_message, "|ClimaQ| --> |VER: %i| --> |P2G-\u2665|", VERSION_CTRL_FW);

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
    if (mqtt_publish("climaQ/report", initial_message, initial_message_size) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to publish message");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Função principal da aplicação.
 * 
 * Esta função é executada ao iniciar a aplicação. Ela configura o controle de LEDs, inicializa
 * o botão e cria tarefas em diferentes núcleos. Também inicializa o serviço MQTT e configura
 * o modo do LED de acordo com o resultado da inicialização.
 */
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