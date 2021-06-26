/*
 * RTOS_Buttons.c
 *
 *  Created on: 26/06/2021
 *      Author: ivogc
 */

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "RTOS_Buttons.h"
#include "button.h"
#include "wait.h"

QueueHandle_t buttons;

static void buttonsHandler(void* params){
    BUTTON_Init();
    buttons=xQueueCreate(10, sizeof(int));
    if(buttons==NULL){
        for(;;)
            WAIT_Milliseconds(10000);
    }
    int value;
    while(1){
        while((value=BUTTON_GetButtonsEvents())==0);
        xQueueSend(buttons, &value, portMAX_DELAY);
    }
}

bool RTOS_Buttons_Init(){
    return xTaskCreate(buttonsHandler, "buttonsHandler", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
}

int RTOS_Buttons_GetEvents(uint32_t timeout){
    if(buttons==NULL) return -1;
    int value=0;
    if(xQueueReceive(buttons, &value, pdMS_TO_TICKS(timeout))!=pdTRUE)
        return 0;
    return value;
}
