/**
 * @file Dht22.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Dht22.h"

static gpio_num_t dht22_pin;

esp_err_t dht22_init(gpio_num_t pin)
{
    dht22_pin = pin;
    gpio_set_direction(dht22_pin, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(dht22_pin, GPIO_PULLUP_ONLY);
    gpio_set_level(dht22_pin, 1); // Inicializar em nível alto
    return ESP_OK;
}

static uint32_t dht22_wait_for_level(int level, uint32_t timeout)
{
    uint32_t time_count = 0;
    while (gpio_get_level(dht22_pin) != level)
    {
        if (time_count++ > timeout)
        {
            return 0;
        }
        esp_rom_delay_us(2);
    }
    return time_count;
}

esp_err_t dht22_read(dht22_data_t *data)
{
    uint8_t bits[5] = {0};
    uint32_t bit_time;

    // Iniciar comunicação
    gpio_set_level(dht22_pin, 0);
    esp_rom_delay_us(20000); // Pull baixo por 20ms
    gpio_set_level(dht22_pin, 1);
    esp_rom_delay_us(40); // Pull alto por 40us

    // Esperar pela resposta do sensor
    if (!dht22_wait_for_level(0, DHT22_LONG_TIMEOUT))
    {
        ESP_LOGE("DHT22", "Timeout esperando nível baixo");
        return ESP_ERR_TIMEOUT;
    }
    if (!dht22_wait_for_level(1, DHT22_LONG_TIMEOUT))
    {
        ESP_LOGE("DHT22", "Timeout esperando nível alto");
        return ESP_ERR_TIMEOUT;
    }
    if (!dht22_wait_for_level(0, DHT22_LONG_TIMEOUT))
    {
        ESP_LOGE("DHT22", "Timeout esperando nível baixo após resposta");
        return ESP_ERR_TIMEOUT;
    }

    // Ler os 40 bits de dados (5 bytes)
    for (int i = 0; i < 40; i++)
    {
        if (!dht22_wait_for_level(1, DHT22_LONG_TIMEOUT))
        {
            ESP_LOGE("DHT22", "Timeout esperando nível alto durante leitura de bits");
            return ESP_ERR_TIMEOUT;
        }
        bit_time = dht22_wait_for_level(0, DHT22_LONG_TIMEOUT);
        if (bit_time == 0)
        {
            ESP_LOGE("DHT22", "Timeout esperando nível baixo durante leitura de bits");
            return ESP_ERR_TIMEOUT;
        }
        bits[i / 8] <<= 1;
        if (bit_time > 40)
        { // A unidade de bit_time é microssegundos
            bits[i / 8] |= 1;
        }
    }

    // Verificar checksum
    if (bits[4] != ((bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF))
    {
        ESP_LOGE("DHT22", "Checksum inválido");
        return ESP_ERR_INVALID_CRC;
    }

    // Converter os dados
    data->humidity = ((bits[0] << 8) + bits[1]) / 10.0;
    data->temperature = (((bits[2] & 0x7F) << 8) + bits[3]) / 10.0;
    if (bits[2] & 0x80)
    {
        data->temperature = -data->temperature;
    }

    return ESP_OK;
}