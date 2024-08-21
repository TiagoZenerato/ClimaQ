/**
 * @file ButtonCtrl.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ButtonCtrl.h"

bool btState = false;

// Configuração do pino do botão
gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_NEGEDGE, // Para detectar uma borda de descida
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = (1ULL << BUTTON_GPIO),
    .pull_up_en = GPIO_PULLUP_ENABLE, // Habilitar pull-up interno
    .pull_down_en = GPIO_PULLDOWN_DISABLE};

// Função de tratamento da interrupção
void IRAM_ATTR button_isr_handler(void *arg)
{
    // Código para lidar com a interrupção, como alternar LEDs ou incrementar contadores
    
    btState = !btState; 
}

// 
void init_button(void)
{

    gpio_config(&io_conf);

    // Instala o ISR
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);
}