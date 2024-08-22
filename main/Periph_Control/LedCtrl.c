/**
 * @file LedCtrl.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "LedCtrl.h"

static const char *TAG = "LedCtrl";
static led_ctrl_config_t led_config = {0};

// Função para inicializar o controle do LED
esp_err_t led_ctrl_init(led_ctrl_config_t config) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Configuração inválida");
        return ESP_ERR_INVALID_ARG;
    }

    led_config = config;

    ESP_LOGI(TAG, "Inicializando o temporizador PWM...");
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = config->resolution,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = config->frequency,
        .clk_cfg = LEDC_AUTO_CLK
    };

    esp_err_t err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao configurar o temporizador PWM: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Temporizador PWM configurado com sucesso.");

    ESP_LOGI(TAG, "Configurando o canal PWM...");
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = config->channel,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = config->gpio_num,
        .duty = 0, // Começa com o LED desligado
        .hpoint = 0
    };

    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao configurar o canal PWM: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Canal PWM configurado com sucesso.");
    return ESP_OK;
}

// Função para definir a cor do LED (em formato RGB)
esp_err_t led_ctrl_set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    if (led_config == NULL)
    {
        ESP_LOGE(TAG, "Configuração não inicializada");
        return ESP_ERR_INVALID_STATE;
    }

    // Definir o valor do duty cycle para cada cor
    esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (red * 1023) / 255);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao definir o duty cycle (R): %s", esp_err_to_name(err));
        return err;
    }

    err = ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao atualizar o duty cycle (R): %s", esp_err_to_name(err));
        return err;
    }

    // Repetir para green e blue, mudando o canal PWM ou multiplexando o mesmo canal

    return ESP_OK;
}

// Função para desligar o LED
esp_err_t led_ctrl_off(void)
{
    if (led_config == NULL)
    {
        ESP_LOGE(TAG, "Configuração não inicializada");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE, led_config->channel, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao definir o duty cycle: %s", esp_err_to_name(err));
        return err;
    }

    err = ledc_update_duty(LEDC_LOW_SPEED_MODE, led_config->channel);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao atualizar o duty cycle: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}