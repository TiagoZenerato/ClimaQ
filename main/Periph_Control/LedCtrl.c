/**
 * @file LedCtrl.c
 * @author Tiago Zenerato (dr.zenerato@gmail.com)
 * @brief Controle de LEDs endereçáveis WS2812B.
 *
 * Este arquivo implementa as funções para controle de LEDs endereçáveis WS2812B,
 * incluindo configuração, controle de cores e modos de operação do LED.
 *
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "LedCtrl.h"

static uint8_t mode_led_rgb = 0;                                     /**< Variável de controle do modo de operação do led.*/
static uint8_t state_led_rgb = 0;                                    /**< Variável de controle para o estado de operação do led.*/
static const char *TAG = "LedCtrl";                                  /**< String utilizada para identificar as mensagens de log geradas pelo gerenciador de LED. */
static uint8_t last_mode_led_rgb = 0;                                /**< Variável para armazenar o último modo de operação do led.*/
static uint8_t blink_timer_select = 0;                               /**< Variável de controle para selecionar os tempos de blink disponíveis.*/
static uint8_t last_state_led_rgb = 0;                               /**< Variável para armazenar o último estado de operação do led.*/
static uint32_t time_for_blink_mode = 0;                             /**< Variável para armazenar o time selecionado para usar no blink.*/
static uint32_t blink_time_use[] = {BLINK_TIME_1, BLINK_TIME_2};     /**< Vetor de tempos disponíveis para utilizar.*/
int num_values = sizeof(blink_time_use) / sizeof(blink_time_use[0]); /**< Variável para calcular a partir do vetor a quantidade de possíveis tempos.*/

led_strip_handle_t led_strip; /**< Estrutura de controle para usar o led da placa.*/

/**
 * @brief Bloco de separação para funções internas
 *
 */
#ifndef INTERNAL_LIB_FUNCTIONS
/**
 * @brief Desliga todos os LEDs para limpar todos os pixels.
 */
void led_ctrl_off_state(void)
{
    // Set all LED off to clear all pixels */
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

/**
 * @brief Define a cor do LED como vermelho.
 */
void led_ctrl_red_state(void)
{
    // Set the LED color red
    for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 150, 0, 0));
    }

    // Refresh the strip to send data
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

/**
 * @brief Define a cor do LED como verde.
 */
void led_ctrl_green_state(void)
{
    // Set the LED color green
    for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 0, 150, 0));
    }

    // Refresh the strip to send data
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

/**
 * @brief Define a cor do LED como azul.
 */
void led_ctrl_blue_state(void)
{
    // Set the LED color blue
    for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 0, 0, 150));
    }

    // Refresh the strip to send data
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

/**
 * @brief Define a cor do LED como amarelo.
 */
void led_ctrl_yellow_state(void)
{
    // Set the LED color yellow
    for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 150, 80, 0));
    }

    // Refresh the strip to send data
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

/**
 * @brief Modo de espera para conexão, piscando o LED azul.
 */
void led_ctrl_waiting_connect_mode(void)
{
    led_ctrl_blue_state();
    vTaskDelay(pdMS_TO_TICKS(500));
    led_ctrl_off_state();
    vTaskDelay(pdMS_TO_TICKS(500));
}

/**
 * @brief Modo conectado, piscando o LED verde.
 */
void led_ctrl_connected_mode(void)
{
    for (int x = 0; x < 4; x++)
    {
        led_ctrl_green_state();
        vTaskDelay(pdMS_TO_TICKS(120));
        led_ctrl_off_state();
        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

/**
 * @brief Modo de erro, piscando o LED vermelho.
 */
void led_ctrl_erro_mode(void)
{
    static uint8_t contErroLedshow = 0;
    static TickType_t lastWakeTime = 0;
    static int state = 0; // Estado da máquina de estados

    switch (state)
    {
    case 0:
        // Inicia o ciclo piscando o LED vermelho
        led_ctrl_red_state();
        lastWakeTime = xTaskGetTickCount();
        state = 1;
        break;

    case 1:
        // Espera 550 ms com o LED vermelho ligado
        if ((xTaskGetTickCount() - lastWakeTime) >= pdMS_TO_TICKS(550))
        {
            led_ctrl_off_state();
            lastWakeTime = xTaskGetTickCount();
            state = 2;
        }
        break;

    case 2:
        // Espera 250 ms com o LED desligado
        if ((xTaskGetTickCount() - lastWakeTime) >= pdMS_TO_TICKS(250))
        {
            contErroLedshow++;
            if (contErroLedshow < 4)
            {
                state = 0; // Repete o ciclo
            }
            else
            {
                contErroLedshow = 0; // Reseta o contador
                state = 3;           // Fim do ciclo de 4 piscadas
            }
        }
        break;

    case 3:
        // Finaliza o ciclo de piscadas
        state = 0; // Reset para próximo ciclo
        break;
    }
}

/**
 * @brief Modo de envio de novos dados, piscando o LED azul.
 */
void led_ctrl_new_data_send_mode(void)
{
    led_ctrl_blue_state();
    vTaskDelay(pdMS_TO_TICKS(500));
    led_ctrl_off_state();
}

/**
 * @brief Modo de piscar o LED baseado no estado definido.
 */
void led_ctrl_blink_mode(void)
{
    static TickType_t lastWakeTime = 0;
    static int state = 0; // Estado da máquina de estados

    switch (state)
    {
    case 0:
        switch (state_led_rgb)
        {
        case OFF:
            led_ctrl_off_state();
            break;
        case RED:
            led_ctrl_red_state();
            break;
        case GREEN:
            led_ctrl_green_state();
            break;
        case BLUE:
            led_ctrl_blue_state();
            break;
        case YELLOW:
            led_ctrl_yellow_state();
            break;
        default:
            led_ctrl_off_state();
            led_ctrl_set_mode(ERRO);
            ESP_LOGE(TAG, "Status requested incorrect or to be implemented");
            break;
        }
        lastWakeTime = xTaskGetTickCount();
        state = 1;
        break;
    case 1:
        if ((xTaskGetTickCount() - lastWakeTime) >= pdMS_TO_TICKS(time_for_blink_mode))
        {
            led_ctrl_off_state();
            lastWakeTime = xTaskGetTickCount();
            state = 2;
        }
        break;
    case 2:
        // Espera 250 ms com o LED desligado
        if ((xTaskGetTickCount() - lastWakeTime) >= pdMS_TO_TICKS(time_for_blink_mode))
        {
            state = 0;
        }
        break;
    }
}
#endif

/**
 * @brief Bloco de separação para funções internas
 *
 */
#ifndef EXTERNAL_LIB_FUNCTION
/**
 * @brief Inicializa o controle do LED strip, configurando o RMT e o objeto LED strip.
 *
 * @return esp_err_t Retorna `ESP_OK` se a inicialização for bem-sucedida, ou um código de erro em caso contrário.
 */
esp_err_t led_ctrl_init(void)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_BLINK_GPIO,   // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_NUMBERS,        // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
#else
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
#endif
    };

    // LED Strip object handle
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");

    return ESP_OK;
}

/**
 * @brief Define o estado atual do LED RGB.
 *
 * @param state Novo estado do LED RGB.
 */
void led_ctrl_set_state(uint8_t state)
{
    last_state_led_rgb = state_led_rgb;
    state_led_rgb = state;
}

/**
 * @brief Obtém o estado atual do LED RGB.
 *
 * @return uint8_t O estado atual do LED RGB.
 */
uint8_t led_ctrl_get_state(void)
{
    return state_led_rgb;
}

/**
 * @brief Define o modo atual do LED RGB.
 *
 * @param mode Novo modo do LED RGB.
 */
void led_ctrl_set_mode(uint8_t mode)
{
    last_mode_led_rgb = mode_led_rgb;
    mode_led_rgb = mode;
}

/**
 * @brief Obtém o modo atual do LED RGB.
 *
 * @return uint8_t O modo atual do LED RGB.
 */
uint8_t led_ctrl_get_mode(void)
{
    return mode_led_rgb;
}

/**
 * @brief Alterna a cor do LED RGB.
 *
 * Define o modo como `RECEIVED_COMMAND` e alterna entre os estados do LED RGB. Se o estado for `YELLOW`, volta para o estado `1`, caso contrário, incrementa o estado.
 * Se o modo anterior foi `BLINK_NOW_STATE`, restaura o modo anterior.
 */
void led_ctrl_toggle_color(void)
{
    led_ctrl_set_mode(RECEIVED_COMMAND);

    if (state_led_rgb <= YELLOW)
    {
        led_ctrl_set_state(state_led_rgb);
        if (state_led_rgb == YELLOW)
        {
            state_led_rgb = 1;
        }
        else
        {
            state_led_rgb++;
        }
    }

    if (last_mode_led_rgb == BLINK_NOW_STATE)
    {
        led_ctrl_set_mode(last_mode_led_rgb);
    }
}

/**
 * @brief Alterna o tempo de piscar do LED.
 *
 * Define o modo como `BLINK_NOW_STATE` e alterna entre os tempos de piscar disponíveis.
 */
void led_ctrl_toggle_blink_time(void)
{
    mode_led_rgb = BLINK_NOW_STATE;
    blink_timer_select = (blink_timer_select + 1) % num_values;
    time_for_blink_mode = blink_time_use[blink_timer_select];
}

/**
 * @brief Obtém o tempo atual para o modo de piscar do LED.
 *
 * @return uint32_t O tempo atual para o modo de piscar.
 */
uint32_t led_ctrl_get_time_use_blink(void)
{
    return time_for_blink_mode;
}
#endif

/**
 * @brief Função principal para controle do LED RGB.
 *
 * Esta função gerencia o comportamento do LED RGB com base no modo atual (`mode_led_rgb`), 
 * alternando entre diferentes funções de controle de LED. Ela também limpa o modo de operação após a execução de certos modos.
 *
 * @param pvParameters Parâmetros passados para a tarefa (não utilizados neste caso).
 */
void led_ctrl_app(void *pvParameters)
{
    bool clean_mode_request = false;

    ESP_LOGI(TAG, "Started main LED RGB control application");
    while (true)
    {
        switch (mode_led_rgb)
        {
        case WAITING_CONNECT:
            led_ctrl_waiting_connect_mode();
            clean_mode_request = true;
            break;
        case CONNECTED:
            led_ctrl_connected_mode();
            clean_mode_request = true;
            break;
        case ERRO:
            led_ctrl_erro_mode();
            break;
        case NEW_DATA_SEND:
            led_ctrl_new_data_send_mode();
            clean_mode_request = true;
            break;
        case RECEIVED_COMMAND:
            switch (state_led_rgb)
            {
            case OFF:
                led_ctrl_off_state();
                break;
            case RED:
                led_ctrl_red_state();
                break;
            case GREEN:
                led_ctrl_green_state();
                break;
            case BLUE:
                led_ctrl_blue_state();
                break;
            case YELLOW:
                led_ctrl_yellow_state();
                break;
            default:
                led_ctrl_off_state();
                led_ctrl_set_mode(ERRO);
                ESP_LOGE(TAG, "State requested incorrect or to be implemented");
                break;
            }
            break;
        case BLINK_NOW_STATE:
            led_ctrl_blink_mode();
            break;
        case CLEAR_MODE:
            vTaskDelay(pdMS_TO_TICKS(10));
            break;
        default:
            mode_led_rgb = ERRO;
            ESP_LOGE(TAG, "Non-existent operating mode");
            break;
        }

        if (clean_mode_request == true)
        {
            led_ctrl_set_mode(CLEAR_MODE);
            clean_mode_request = false;
        }

        vTaskDelay(pdMS_TO_TICKS(2)); // Verificando a cada 2 ms (500Hz)
    }
}

/**
 * @brief Teste de piscar o LED RGB.
 *
 * Esta função alterna o estado do LED RGB entre ligado e desligado a cada 500 ms,
 * e define a cor do LED como vermelha (30 de intensidade) quando ligado. A função 
 * entra em um loop infinito piscando o LED strip para testes visuais.
 */
void led_blink_rgb_teste(void)
{
    bool led_on_off = false;

    ESP_LOGI(TAG, "Start blinking LED strip");
    while (1)
    {
        if (led_on_off)
        {
            /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
            for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++)
            {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 30, 0, 0));
            }
            /* Refresh the strip to send data */
            ESP_ERROR_CHECK(led_strip_refresh(led_strip));
            ESP_LOGI(TAG, "LED ON!");
        }
        else
        {
            /* Set all LED off to clear all pixels */
            ESP_ERROR_CHECK(led_strip_clear(led_strip));
            ESP_LOGI(TAG, "LED OFF!");
        }

        led_on_off = !led_on_off;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
