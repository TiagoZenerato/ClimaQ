/**
 * @file MqttCtrl.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "MqttCtrl.h"

#define WIFI_SSID "ZENA2007"
#define WIFI_PASS "m130856z"

#define BROKER_URL "broker.hivemq.com"
#define TOPIC_PUBLISH "testtopic/1"
#define TOPIC_SUBSCRIBE "testtopic/#sub"

static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip_id;

static void event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

static void wifi_init_sta(void) 
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip_id));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI("MQTT", "Connected to broker");
            ESP_ERROR_CHECK(esp_mqtt_client_subscribe(event->client, TOPIC_SUBSCRIBE, 0));
            break;
        case MQTT_EVENT_DATA:
            printf("Received message: %.*s\n", event->data_len, event->data);
            break;
        default:
            break;
    }
}

static void mqtt_app_start(void) {

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URL, 
        .broker.address.port = 8884,
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));

    // Publish a message
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "key", "value");
    char *json_str = cJSON_Print(json);
    ESP_ERROR_CHECK(esp_mqtt_client_publish(client, TOPIC_PUBLISH, json_str, 0, 1, 0));
    free(json_str);
    cJSON_Delete(json);
}

static void event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI("WIFI", "Disconnected from Wi-Fi, reconnecting...");
        esp_wifi_connect();
    } else if (base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI("WIFI", "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id);
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip_id);
        mqtt_app_start();
    }
}

void MqttCtrlAllInit(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_sta();
}