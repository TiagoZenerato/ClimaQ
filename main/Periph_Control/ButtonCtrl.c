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
bool bt_state = false;
bool bt_pressed = false;
static bool timer_started = false;
uint32_t last_debounce_time = 0;
uint32_t debounce_delay = 50; // Tempo de debounce em milissegundos
uint32_t button_press_time = 0;
uint32_t long_press_threshold = 3000; // Threshold para considerar um pressionamento longo

// Variável global para armazenar o estado do botão
button_press_mode_t press_mode = BUTTON_PRESS_NONE;
static esp_timer_handle_t press_timer;

// Configuração do pino do botão
gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_ANYEDGE, // Para detectar uma borda de descida
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = (1ULL << BUTTON_GPIO),
    .pull_up_en = GPIO_PULLUP_ENABLE, // Habilitar pull-up interno
    .pull_down_en = GPIO_PULLDOWN_DISABLE};

/**
 * @brief
 *
 * @param arg
 */
void IRAM_ATTR button_isr_handler(void *arg)
{
    uint32_t current_time = esp_timer_get_time() / 1000; // Tempo atual em milissegundos

    // Verificar debounce
    static uint32_t last_debounce_time = 0;
    if ((current_time - last_debounce_time) < debounce_delay)
    {
        return;
    }
    last_debounce_time = current_time;

    bool current_button_state = gpio_get_level(BUTTON_GPIO); // Troque BUTTON_GPIO pelo número da GPIO do botão

    if (current_button_state == 0) // Botão pressionado (borda de descida)
    {
        if (!bt_pressed) // O botão estava solto
        {
            bt_pressed = true;
            button_press_time = current_time; // Armazena o tempo de início da pressão
            if (!timer_started)
            {
                esp_timer_start_once(press_timer, long_press_threshold * 1000); // Inicia o temporizador
                timer_started = true;
            }
        }
    }
    else // Botão solto (borda de subida)
    {
        if (bt_pressed) // O botão estava pressionado
        {
            bt_pressed = false;
            esp_timer_stop(press_timer); // Para o temporizador
            timer_started = false;

            uint32_t press_duration = current_time - button_press_time;

            if (press_duration >= long_press_threshold)
            {
                press_mode = BUTTON_PRESS_LONG; // Atualizar para pressão longa
            }
            else
            {
                press_mode = BUTTON_PRESS_SHORT; // Atualizar para pressão curta
            }
        }
    }
}

// Função de callback do temporizador
void IRAM_ATTR timer_callback(void *arg)
{
    // Esta função pode ser usada para lógica adicional se necessário
}

//
void init_button(void)
{

    gpio_config(&io_conf);

    // Configura o temporizador
    esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "press_timer"};
    esp_timer_create(&timer_args, &press_timer);

    // Instala o ISR
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);
}

void button_ctrl_app(void *pvParameters)
{
    while (true)
    {
        if (press_mode == BUTTON_PRESS_SHORT)
        {
            printf("\nshort");
            led_ctrl_toggle_blink_time();
            press_mode = BUTTON_PRESS_NONE;
        }
        else if (press_mode == BUTTON_PRESS_LONG)
        {
            led_ctrl_random_color();
            press_mode = BUTTON_PRESS_NONE;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // A cada 10ms
    }
}