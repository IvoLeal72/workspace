/*
 * RTOS_LCD.c
 *
 *  Created on: 25/06/2021
 *      Author: ivogc
 */

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "lcd.h"
#include "wait.h"
#include "RTOS_LCD.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

typedef enum{WriteChar, Locate, WriteString, Clear, CreateChar, SetCursor} CMD;

static SemaphoreHandle_t mutex;
static QueueHandle_t cmds;

typedef struct{
	CMD cmd;
	union{
		char ch;
		int row;
		char* str;
		unsigned char location;
		bool state;
	};
	union{
		int column;
		unsigned char charmap[8];
	};
}LCD_CMD;

static void lcdHandler(void* params){
	mutex=xSemaphoreCreateMutex();
	if(mutex==NULL)
		for(;;)
			WAIT_Milliseconds(10000);
	cmds=xQueueCreate(10, sizeof(LCD_CMD));
	if(cmds==NULL)
		for(;;)
			WAIT_Milliseconds(10000);
	LCDText_Init();
	LCD_CMD command;
	while(1){
		xQueueReceive(cmds, &command, portMAX_DELAY);
		switch(command.cmd){
			case WriteChar: LCDText_WriteChar(command.ch); break;
			case Locate: LCDText_Locate(command.row, command.column); break;
			case WriteString: LCDText_WriteString(command.str); break; //needs free
			case Clear: LCDText_Clear(); break;
			case CreateChar: LCDText_CreateChar(command.location, command.charmap); break;
			case SetCursor: LCDText_SetCursor(command.state); break;
		}
	}
}

bool RTOS_LCD_Init(){
	return xTaskCreate(lcdHandler, "lcdHandler", configMINIMAL_STACK_SIZE, NULL, 2, NULL)==pdPASS;
}

static void RTOS_LCD_Lock(){
	while(mutex==NULL || cmds==NULL);
	xSemaphoreTake(mutex, portMAX_DELAY);
}

static void RTOS_LCD_Unlock(){
	xSemaphoreGive(mutex);
}

static void sendCMD(LCD_CMD* ptr){
	xQueueSend(cmds, ptr, portMAX_DELAY);
}

static void RTOS_LCD_Clear(){
	LCD_CMD command;
	command.cmd=Clear;
	sendCMD(&command);
}

static void RTOS_LCD_Locate(int row, int column){
	LCD_CMD command;
	command.cmd=Locate;
	command.row=row;
	command.column=column;
	sendCMD(&command);
}

static void RTOS_LCD_WriteChar(char ch){
	LCD_CMD command;
	command.cmd=WriteChar;
	command.ch=ch;
	sendCMD(&command);
}

static void RTOS_LCD_WriteString(char* str){
	LCD_CMD command;
	command.cmd=WriteString;
	command.str=pvPortMalloc(strlen(str)+1);
	if(command.str==NULL)
		for(;;)
			WAIT_Milliseconds(10000);
	strcpy(command.str, str);
	sendCMD(&command);
}

void RTOS_LCD_CreateChar(unsigned char location, unsigned char charmap[8]){
	RTOS_LCD_Lock();
	LCD_CMD command;
	command.cmd=CreateChar;
	command.location=location;
	memcpy(command.charmap, charmap, 8);
	RTOS_LCD_Unlock();
}

void RTOS_LCD_SetCursor(bool state){
	RTOS_LCD_Lock();
	LCD_CMD command;
	command.cmd=SetCursor;
	command.state=state;
	sendCMD(&command);
	RTOS_LCD_Unlock();
}



void RTOS_LCD_LocatedChar(int row, int column, char ch, bool clear){
	RTOS_LCD_Lock();
	if(clear){
		RTOS_LCD_Clear();
	}

	RTOS_LCD_Locate(row, column);

	RTOS_LCD_WriteChar(ch);

	RTOS_LCD_Unlock();
}

void RTOS_LCD_LocatedPrint(int row, int column, char* str, bool clear){
	RTOS_LCD_Lock();
	if(clear){
		RTOS_LCD_Clear();
	}

	RTOS_LCD_Locate(row, column);

	RTOS_LCD_WriteString(str);

	RTOS_LCD_Unlock();
}

void RTOS_LCD_Printf(int row, int column, bool clear, char *fmt, ...){
	char buffer[33];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 33, fmt, args);
	va_end(args);
	RTOS_LCD_LocatedPrint(row, column, buffer, clear);
}
