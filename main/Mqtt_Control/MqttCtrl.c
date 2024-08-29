/**
 * @file MqttCtrl.c
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

#include "MqttCtrl.h"

const int mqtt_port = 1883;                    /**< Porta utilizada para a conexão MQTT. */
static const char *TAG = "MQTT_LIB";           /**< Tag de log para a biblioteca MQTT. */
const char *mqtt_server = "broker.hivemq.com"; /**< URL do broker MQTT. */
static esp_mqtt_client_handle_t client = NULL; /**< Handle do cliente MQTT. */

/**
 * @brief Callback para eventos MQTT.
 *
 * @param client Cliente MQTT.
 * @param event Evento MQTT.
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    if (event == NULL)
    {
        ESP_LOGE(TAG, "Null event received");
        return;
    }

    switch (event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA received");
        // Comparando o tópico
        if (strncmp(event->topic, "climaQ/TrocarCor", event->topic_len) == 0)
        {
            // Comparando os dados
            if (strncmp(event->data, "1", event->data_len) == 0)
            {
                led_ctrl_toggle_color();
                mqtt_publish("climaQ/report", "Mensagem recebida!", strlen("Mensagem recebida"));
            }
            else if (strncmp(event->data, "0", event->data_len) == 0)
            {
                led_ctrl_set_state(OFF);
                mqtt_publish("climaQ/report", "Mensagem recebida!", strlen("Mensagem recebida"));
            }
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id: %li", event_id);
        break;
    }
}

/**
 * @brief Inicializa o cliente MQTT e conecta ao broker.
 *
 * Esta função deve ser chamada antes de qualquer outra operação MQTT.
 *
 * @param uri URI do broker MQTT.
 * @return esp_err_t Retorna ESP_OK se a inicialização for bem-sucedida.
 */
esp_err_t mqtt_init(const char *uri, uint16_t port_use_connect)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = uri,
        .broker.address.port = port_use_connect,
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
        .session.keepalive = 60,
        .network.disable_auto_reconnect = true,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    if (client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return ESP_FAIL;
    }

    esp_err_t err2 = esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, mqtt_event_handler, NULL);
    if (err2 != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to register event handler, err=%d", err2);
        return err2;
    }

    esp_err_t err = esp_mqtt_client_start(client);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start MQTT client");
        return err;
    }

    return ESP_OK;
}

/**
 * @brief Publica uma mensagem em um tópico específico.
 *
 * @param topic Tópico onde a mensagem será publicada.
 * @param payload Dados a serem enviados.
 * @param len Comprimento dos dados a serem enviados.
 * @return esp_err_t Retorna ESP_OK se a publicação for bem-sucedida.
 */
esp_err_t mqtt_publish(const char *topic, const char *payload, int len)
{
    if (client == NULL)
    {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    int msg_id = esp_mqtt_client_publish(client, topic, payload, len, 2, 0);
    if (msg_id == -1)
    {
        ESP_LOGE(TAG, "Failed to publish message");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Inscreve-se em um tópico específico.
 *
 * @param topic Tópico ao qual se inscrever.
 * @return esp_err_t Retorna ESP_OK se a inscrição for bem-sucedida.
 */
esp_err_t mqtt_subscribe(const char *topic)
{
    if (client == NULL)
    {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    int msg_id = esp_mqtt_client_subscribe(client, (char *)topic, 0);
    if (msg_id == -1)
    {
        ESP_LOGE(TAG, "Failed to subscribe to topic");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Desinscreve-se de um tópico específico.
 *
 * @param topic Tópico ao qual se desinscrever.
 * @return esp_err_t Retorna ESP_OK se a desinscrição for bem-sucedida.
 */
esp_err_t mqtt_unsubscribe(const char *topic)
{
    if (client == NULL)
    {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    int msg_id = esp_mqtt_client_unsubscribe(client, topic);
    if (msg_id == -1)
    {
        ESP_LOGE(TAG, "Failed to unsubscribe from topic");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Desconecta o cliente MQTT e libera os recursos.
 */
void mqtt_deinit(void)
{
    if (client != NULL)
    {
        esp_mqtt_client_stop(client);
        esp_mqtt_client_destroy(client);
        client = NULL;
    }
}
