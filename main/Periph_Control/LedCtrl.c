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

// Variáveis de controle para o modo de blink.
int current_index = 0;
static uint32_t time_for_blink_mode = 0;
uint32_t blink_time_use[] = {BLINK_TIME_1, BLINK_TIME_2};
int num_values = sizeof(blink_time_use) / sizeof(blink_time_use[0]);

// Variáveis de controle para operação do led.
static int last_random_color;
static uint8_t mode_led_rgb = 0;
static uint8_t state_led_rgb = 0;

led_strip_handle_t led_strip; // estrutura de controle

/**
 * @brief
 *
 */
#ifndef INTERNAL_LIB_FUNCTIONS
/**
 * @brief
 *
 */
void led_ctrl_off_state(void)
{
    // Set all LED off to clear all pixels */
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

/**
 * @brief
 *
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
 * @brief
 *
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
 * @brief
 *
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
 * @brief
 *
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
 * @brief
 *
 * @param funtion
 */
void led_ctrl_waiting_connect_mode(void)
{
    led_ctrl_blue_state();
    vTaskDelay(pdMS_TO_TICKS(500));
    led_ctrl_off_state();
    vTaskDelay(pdMS_TO_TICKS(500));
}

/**
 * @brief
 *
 */
void led_ctrl_connected_mode(void)
{
    led_ctrl_green_state();
    vTaskDelay(pdMS_TO_TICKS(320));
    led_ctrl_off_state();
}

/**
 * @brief  Modo de erro, piscando o LED vermelho
 *
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
 * @brief
 *
 */
void led_ctrl_new_data_send_mode(void)
{
    led_ctrl_blue_state();
    vTaskDelay(pdMS_TO_TICKS(500));
    led_ctrl_off_state();
}

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
 * @brief
 *
 */
#ifndef EXTERNAL_LIB_FUNCTION
/**
 * @brief
 *
 * @return esp_err_t
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
 * @brief
 *
 * @param state
 */
void led_ctrl_set_state(uint8_t state)
{
    state_led_rgb = state;
}

/**
 * @brief
 *
 * @return uint8_t
 */
uint8_t led_ctrl_get_state(void)
{
    return state_led_rgb;
}

/**
 * @brief
 *
 * @param mode
 */
void led_ctrl_set_mode(uint8_t mode)
{
    mode_led_rgb = mode;
}

/**
 * @brief
 *
 * @return uint8_t
 */
uint8_t led_ctrl_get_mode(void)
{
    return mode_led_rgb;
}

/**
 * @brief
 *
 */
void led_ctrl_random_color(void)
{
    led_ctrl_set_mode(RECEIVED_COMMAND);
    vTaskDelay(pdMS_TO_TICKS(5));
    int color_for_led = get_random_number(RED, YELLOW);

    if (color_for_led != last_random_color)
    {
        led_ctrl_set_state(color_for_led);
        last_random_color = color_for_led;
    }
    else
    {
        led_ctrl_set_state(get_random_number(RED, YELLOW));
    }
}

/**
 * @brief 
 * 
 */
void led_ctrl_toggle_blink_time(void)
{
    mode_led_rgb = BLINK_NOW_STATE;
    current_index = (current_index + 1) % num_values;
    time_for_blink_mode = blink_time_use[current_index];
}

#endif

void led_ctrl_app(void *pvParameters)
{
    ESP_LOGI(TAG, "Started main LED RGB control application");
    while (true)
    {
        switch (mode_led_rgb)
        {
        case WAITING_CONNECT:
            led_ctrl_waiting_connect_mode();
            break;
        case CONNECTED:
            led_ctrl_connected_mode();
            break;
        case ERRO:
            led_ctrl_erro_mode();
            break;
        case NEW_DATA_SEND:
            led_ctrl_new_data_send_mode();
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
                ESP_LOGE(TAG, "Status requested incorrect or to be implemented");
                break;
            }
            break;
        case BLINK_NOW_STATE:
            led_ctrl_blink_mode();
            break;
        default:
            mode_led_rgb = ERRO;
            ESP_LOGE(TAG, "Non-existent operating mode");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(2)); // Verificando a cada 2 ms (500Hz)
    }
}

/**
 * @brief
 *
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
