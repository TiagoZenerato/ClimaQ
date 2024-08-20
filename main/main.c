/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.h"

void app_main()
{
    init_button();
    
    while (true)
    {
        printf("\nHello Teste");
        if(btState == true){
            printf("\nButton...");
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay para evitar uso excessivo da CPU
    }
}
