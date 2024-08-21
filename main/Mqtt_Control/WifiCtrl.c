/**
 * @file WifiCtrl.c
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
#include "WifiCtrl.h"

static int s_retry_num = 0;              // Contador de tentativas de reconexão.
static const char *TAG = "WIFI_MANAGER"; // String utilizada para identificar as mensagens de log geradas pelo gerenciador de Wi-Fi.

// Declaração de struct para uso da lib
static EventGroupHandle_t s_wifi_event_group;
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;

/**
 * @brief Manipulador de eventos Wi-Fi.
 *
 * Esta função é responsável por tratar eventos de Wi-Fi, como o início da conexão,
 * desconexões e obtenção de endereço IP.
 *
 * @param arg Argumento passado ao manipulador (não utilizado).
 * @param event_base Base do evento (WIFI_EVENT ou IP_EVENT).
 * @param event_id ID do evento.
 * @param event_data Dados específicos do evento.
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/**
 * @brief Inicializa o gerenciador de Wi-Fi.
 *
 * Esta função inicializa o sistema de Wi-Fi, incluindo a inicialização do NVS,
 * configuração dos eventos de Wi-Fi, e preparação do modo de operação STA.
 *
 * @return esp_err_t Retorna ESP_OK em caso de sucesso ou código de erro.
 */
esp_err_t wifi_manager_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    return ESP_OK;
}

/**
 * @brief Conecta a uma rede Wi-Fi.
 *
 * Esta função configura e conecta o ESP32 a uma rede Wi-Fi específica.
 *
 * @param ssid O SSID da rede Wi-Fi.
 * @param password A senha da rede Wi-Fi.
 *
 * @return esp_err_t Retorna ESP_OK se a conexão for bem-sucedida, ou ESP_FAIL se falhar.
 */
esp_err_t wifi_manager_connect(const char *ssid, const char *password)
{
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s", ssid);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", ssid);
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Desconecta da rede Wi-Fi.
 *
 * Esta função desconecta o ESP32 da rede Wi-Fi atual.
 *
 * @return esp_err_t Retorna ESP_OK em caso de sucesso.
 */
esp_err_t wifi_manager_disconnect(void)
{
    ESP_LOGI(TAG, "Disconnecting from Wi-Fi...");
    ESP_ERROR_CHECK(esp_wifi_stop());
    return ESP_OK;
}

/**
 * @brief Verifica se o ESP32 está conectado a uma rede Wi-Fi.
 *
 * @return true Se o dispositivo estiver conectado a uma rede Wi-Fi.
 * @return false Se o dispositivo não estiver conectado a uma rede Wi-Fi.
 */
bool wifi_manager_is_connected(void)
{
    EventBits_t bits = xEventGroupGetBits(s_wifi_event_group);
    return (bits & WIFI_CONNECTED_BIT) != 0;
}