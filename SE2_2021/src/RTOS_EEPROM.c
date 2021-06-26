/*
 * RTOS_EEPROM.c
 *
 *  Created on: 26/06/2021
 *      Author: ivogc
 */

#include "EEPROM.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "wait.h"
#include "RTOS_EEPROM.h"

static QueueHandle_t cmds;

typedef enum{Write, Read} CMD;

typedef struct{
    CMD cmd;
    short addr;
    char* data;
    size_t data_size;
    void (*returnFunc)(int);
}EEPROM_CMD;

static void eepromHandler(void* params){
    cmds=xQueueCreate(10, sizeof(EEPROM_CMD));
    if(cmds==NULL)
        for(;;)
            WAIT_Milliseconds(10000);
    EEPROM_CMD command;
    while(1){
        xQueueReceive(cmds, &command, portMAX_DELAY);
        int ret;
        if(command.cmd==Write)
            ret=EEPROM_Write(command.addr, command.data, command.data_size);
        else
            ret=EEPROM_Read(command.addr, command.data, command.data_size);
        if(command.returnFunc!=NULL)
            command.returnFunc(ret);
    }
}

bool RTOS_EEPROM_Init(){
    return xTaskCreate(eepromHandler, "eepromHandler", configMINIMAL_STACK_SIZE, NULL, 2, NULL)==pdPASS;
}

static bool RTOS_EEPROM_CMD(CMD cmd, short addr, char* data, size_t data_size, void(*returnFunc)(int), uint32_t timeout){
    if(cmds==NULL) return false;
    EEPROM_CMD command={cmd, addr, data, data_size, returnFunc};
    return xQueueSend(cmds, &command, timeout)==pdTRUE;
}

bool RTOS_EEPROM_Write(short addr, char* data, size_t data_size, void(*returnFunc)(int), uint32_t timeout){
    return RTOS_EEPROM_CMD(Write, addr, data, data_size, returnFunc, timeout);
}

bool RTOS_EEPROM_Read(short addr, char* data, size_t data_size, void(*returnFunc)(int), uint32_t timeout){
    return RTOS_EEPROM_CMD(Read, addr, data, data_size, returnFunc, timeout);
}