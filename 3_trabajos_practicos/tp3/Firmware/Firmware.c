#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "helper.h"

// GPIOs a usar como entrada y salida
#define APP_IN  15
#define APP_OUT 14
#define FRECUENCIA 3000

// Maximo valor de cuenta para el semaforo
#define MAX_COUNT   10000

// Semaforo counting
SemaphoreHandle_t semphr_counting;

/**
 * @brief Callback para interrupcion por GPIO
 */
void irq_callback(uint gpio, uint32_t event_mask) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if ((event_mask & GPIO_IRQ_EDGE_FALL) && gpio == APP_IN) {
        xSemaphoreGiveFromISR(semphr_counting, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}



/**
 * @brief Tarea de inicializacion
 */
void task_init(void *params) {
    // Inicializacion de GPIO
    gpio_init(APP_IN);
    gpio_init(APP_OUT);
    gpio_set_dir(APP_IN, false);
    //gpio_set_dir(APP_OUT, true);
    // Agrego interrupcion por flanco descendente
    gpio_set_irq_enabled_with_callback(APP_IN, GPIO_IRQ_EDGE_FALL, true, irq_callback);
    // Creo semaforo
    semphr_counting = xSemaphoreCreateCounting(MAX_COUNT, 0);
        // Inicializo el PWM
    pwm_user_init(APP_OUT, FRECUENCIA);
    // Elimino tarea para liberar recursos
    vTaskDelete(NULL);
}

/**
 * @brief Tarea que manda la cuenta final por consola y 
 * limpia el semaforo
 */
void task_clear(void *params) {

    while(1) {
        // Mando por consola y limpio semaforo
        printf("FRECUENCIA %d \n", uxSemaphoreGetCount(semphr_counting));
        xQueueReset(semphr_counting);
        // Demoro por un segundo para contar
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(task_clear, "Clr", 2 * configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    // Arranca el sistema operativo
    vTaskStartScheduler();
    while(1);
}
