/**
 * @file ButtonCtrl.c
 * @author Tiago Zenerato (dr.zenerato@gmail.com)
 * @brief Controle de botão com debounce e detecção de pressionamento longo.
 *
 * Este arquivo implementa as funcionalidades necessárias para o controle de um botão,
 * incluindo debounce, detecção de bordas, e temporização para diferenciar entre um
 * pressionamento curto e um pressionamento longo.
 *
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ButtonCtrl.h"

bool bt_state = false;                              /**< Estado atual do botão. */
bool bt_pressed = false;                            /**< Indica se o botão foi pressionado. */
uint32_t debounce_delay = 50;                       /**< Tempo de debounce em milissegundos. */
uint32_t button_press_time = 0;                     /**< Tempo de pressionamento do botão em milissegundos. */
uint32_t last_debounce_time = 0;                    /**< Último tempo de debounce em milissegundos. */
static bool timer_started = false;                  /**< Indica se o temporizador foi iniciado. */
uint32_t long_press_threshold = 3000;               /**< Threshold para considerar um pressionamento longo (em milissegundos). */
static esp_timer_handle_t press_timer;              /**< Handle do temporizador para controle de pressionamento do botão. */
button_press_mode_t press_mode = BUTTON_PRESS_NONE; /**< Modo de pressionamento do botão (curto, longo, etc.). */

/**
 * @brief Configuração do pino do botão. 
 */
gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_ANYEDGE,         /**< Para detectar uma borda de descida */
    .mode = GPIO_MODE_INPUT,                /**< Configura o pino como entrada */
    .pin_bit_mask = (1ULL << BUTTON_GPIO),  /**< Máscara de bits para o pino do botão */
    .pull_up_en = GPIO_PULLUP_ENABLE,       /**< Habilitar pull-up interno */
    .pull_down_en = GPIO_PULLDOWN_DISABLE}; /**< Desabilitar pull-down interno */

/**
 * @brief Manipulador de interrupção para o botão.
 *
 * Esta função é chamada quando uma interrupção é gerada pelo botão,
 * detectando tanto a borda de subida quanto a de descida para determinar
 * se o botão foi pressionado ou solto.
 *
 * @param arg Argumento opcional passado para a função (não utilizado).
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

/**
 * @brief Callback do temporizador de pressionamento do botão.
 *
 * Esta função é chamada quando o temporizador configurado para detectar
 * um pressionamento longo expira.
 *
 * @param arg Argumento opcional passado para a função (não utilizado).
 */
void IRAM_ATTR timer_callback(void *arg)
{
    // Esta função pode ser usada para lógica adicional se necessário
}

/**
 * @brief Inicializa o botão.
 *
 * Configura o pino do botão, o temporizador e instala o serviço de interrupção.
 */
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

/**
 * @brief Função principal de controle do botão.
 *
 * Esta função pode ser utilizada em uma task FreeRTOS para monitorar o estado
 * do botão e tomar ações apropriadas.
 *
 * @param pvParameters Parâmetro opcional passado para a função (não utilizado).
 */
void button_ctrl_app(void *pvParameters)
{
    while (true)
    {
        if (press_mode == BUTTON_PRESS_SHORT)
        {
            led_ctrl_toggle_blink_time();
            press_mode = BUTTON_PRESS_NONE;
        }
        else if (press_mode == BUTTON_PRESS_LONG)
        {
            led_ctrl_toggle_color();
            press_mode = BUTTON_PRESS_NONE;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // A cada 10ms
    }
}