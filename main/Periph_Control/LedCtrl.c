/**
 * @file LedCtrl.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "LedCtrl.h"

// Função para inicializar o canal RMT
void init_rmt() {
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(LED_PIN, RMT_CHANNEL);
    config.clk_div = 2;  // Configuração do clock para alta precisão
    rmt_config(&config);
    rmt_driver_install(config.channel, 0, 0);
}

// Função para converter cor RGB para o formato WS2812
void rgb_to_ws2812(uint8_t r, uint8_t g, uint8_t b, rmt_item32_t* item) {
    uint32_t color = (g << 16) | (r << 8) | b;
    for (int i = 23; i >= 0; i--) {
        if (color & (1 << i)) {
            item->duration0 = 9;  // T0H: 900 ns
            item->level0 = 1;
            item->duration1 = 3;  // T0L: 300 ns
            item->level1 = 0;
        } else {
            item->duration0 = 3;  // T1H: 300 ns
            item->level0 = 1;
            item->duration1 = 9;  // T1L: 900 ns
            item->level1 = 0;
        }
        item++;
    }
}

// Função para enviar as cores para os LEDs
void send_colors(rmt_item32_t* items, size_t len) {
    rmt_write_items(RMT_CHANNEL, items, len, true);
    rmt_wait_tx_done(RMT_CHANNEL, portMAX_DELAY);
}

void app_main() {
    init_rmt();
    
    rmt_item32_t led_data[LED_STRIP_LEN * 24];  // Buffer para 8 LEDs
    
    while (true) {
        // Configurar cor vermelha no primeiro LED
        rgb_to_ws2812(255, 0, 0, led_data);
        // Configurar cor verde no segundo LED
        rgb_to_ws2812(0, 255, 0, led_data + 24);
        // Configurar cor azul no terceiro LED
        rgb_to_ws2812(0, 0, 255, led_data + 48);

        // Enviar os dados para os LEDs
        send_colors(led_data, LED_STRIP_LEN * 24);

        vTaskDelay(pdMS_TO_TICKS(1000));  // Espera 1 segundo

        // Apagar os LEDs
        memset(led_data, 0, sizeof(led_data));
        send_colors(led_data, LED_STRIP_LEN * 24);
        
        vTaskDelay(pdMS_TO_TICKS(1000));  // Espera 1 segundo
    }
}