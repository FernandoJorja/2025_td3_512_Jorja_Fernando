#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"    // Libreria para el Led
#include "FreeRTOS.h"
#include "task.h"

/**
 * Declararacion de Tareas
 */

void Vtask_Led_On(void *params);
void Vtask_Led_Off(void *params);
void VTaskDelete(void *params);






/**
 * Tarea de blinky de LED. Una tarea prende el Led y la otra lo apaga
 */
void Vtask_Led_On(void *params) {
    
    while(1) {

         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
         printf("Prendo led\n");
        // Demora de ticks equivalentes a 1000 ms (500 + 500)
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}


void Vtask_Led_Off(void *params) {
    
    while(1) {

        // Demora de ticks equivalentes a 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        printf("Apago led\n");
        // Demora de ticks equivalentes a 500 ms
        // Total de demora 1000
        vTaskDelay(pdMS_TO_TICKS(500));

    }
}


/**
 *  Programa principal
 */
int main(void) {
    stdio_init_all();

 if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    
// Creacion de tareas

xTaskCreate(Vtask_Led_On, "Vtask_Led_On", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
xTaskCreate(Vtask_Led_Off, "Vtask_Led_Off", configMINIMAL_STACK_SIZE, NULL, 1, NULL);




    // Arranca el scheduler
    vTaskStartScheduler();

    
    while(1);
}

