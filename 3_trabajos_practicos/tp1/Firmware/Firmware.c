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

#define LED_ON_TIME 1000 
#define LED_OFF_TIME 1500 




/**
 * Tarea de blinky de LED. Una tarea prende el Led y la otra lo apaga
 */
void Vtask_Led_On(void *params) {
    
    while(1) {

         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
         printf("Prendo led\n");
        // Demora de ticks equivalentes a un ciclo completo (1000 + 1500)
        vTaskDelay(pdMS_TO_TICKS(LED_ON_TIME + LED_OFF_TIME));

    }
}


void Vtask_Led_Off(void *params) {
    
    while(1) {

        // Demora de ticks equivalentes a tiempo encendido en ms
        vTaskDelay(pdMS_TO_TICKS(LED_ON_TIME));
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        printf("Apago led\n");
        // Demora de ticks equivalentes a tiempo apagado en ms
        // Total del ciclo es LED_ON_TIME mas LED_OFF_TIME
        vTaskDelay(pdMS_TO_TICKS(LED_OFF_TIME));

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

xTaskCreate(Vtask_Led_On, "Vtask_Led_On", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
xTaskCreate(Vtask_Led_Off, "Vtask_Led_Off", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);




    // Arranca el scheduler
    vTaskStartScheduler();

    
    while(1);
}

