#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "hardware/adc.h"

// Cola de datos de temperatura
QueueHandle_t cola_temp;

float tempC=0.0;
uint8_t muestras=20;  // para tomar un promedio de muestras
uint16_t suma=0;
uint16_t promedio=0;

/**
 * @brief Tarea de inicializacion
 */
void task_init(void *params) {

    // Inicializa la cola
    cola_temp = xQueueCreate(1, sizeof(uint32_t));

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    // Elimina la tarea para liberar recursos
    vTaskDelete(NULL);
}

/**
 * @brief Tarea que lee el adc, lo transforma en temp y lo manda
 * por una cola
 */
void task_calc_temp_envia(void *params) {

    while(1) {

    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V 
    const float conversionFactor = 3.3f / (1 << 12);
    suma=0;
    for(uint8_t i = 0; i < muestras; i++) { 
        suma += adc_read();

        }
    
    promedio=suma/muestras;

    float adc_voltage = promedio * conversionFactor;
    tempC = 27.0f - (adc_voltage - 0.706f) / 0.001721f;  //Formula del fabricante

     // Envio a cola
    xQueueSend(cola_temp, &tempC, portMAX_DELAY);
    
    // retardo
    vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/**
 * @brief Tarea que escribe por consola lo que llega
 * de la cola
 */
void task_print(void *params) {

    while(1) {

        xQueueReceive(cola_temp, &tempC, portMAX_DELAY);
        printf("temperatura de placa = %.1f %cC \n", tempC);

    }
}

/**
 * @brief Programa principal
 */
int main(void) {
    stdio_init_all();

    // Creacion de tareas
    xTaskCreate(task_init, "Init", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_calc_temp_envia, "RNG", 4*configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_print, "Print", 2*configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}