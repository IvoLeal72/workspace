/*
 * esp8266.c
 *
 *  Created on: 02/06/2021
 *      Author: ivo
 */

#include "LPC17xx.h"
#include "wait.h"
#include "esp8266.h"
#include "UART.h"
#include <string.h>

#define UART_ID 2
#define UART_OPT 0
#define BAUD_RATE 115200
#define ESP_RST_PIN 5

static void ESP_HardRst(){
	LPC_GPIO0->FIOCLR=1<<ESP_RST_PIN;
	WAIT_Milliseconds(1);
	LPC_GPIO0->FIOSET=1<<ESP_RST_PIN;
	WAIT_Milliseconds(10000); //delay para arranque, ajustar
}

static void ESP_SendCommand(char* cmd){
	for(int i=0; i<strlen(cmd);)
		i+=UART_WriteBuffer(UART_ID, (unsigned char*)cmd+i, strlen(cmd)-i);
}

static void ESP_WaitForOk(){
	char last=0, prev=0, tmp;
	while(prev!='O' && last!='K'){
		while(UART_ReadBuffer(UART_ID, (unsigned char*)&tmp, 1)!=1);
		prev=last;
		last=tmp;
	}
	while(prev!='\r' && last!='\n'){
		while(UART_ReadBuffer(UART_ID, (unsigned char*)&tmp, 1)!=1);
		prev=last;
		last=tmp;
	}
}

bool ESP_Init(){
	LPC_GPIO0->FIODIR|=1<<ESP_RST_PIN;
	ESP_HardRst();
	UART_Initialize(UART_ID, UART_OPT, BAUD_RATE);
	ESP_SendCommand("AT\r\n");
	ESP_WaitForOk();
	ESP_SendCommand("AT+CWMODE_CUR=1\r\n");
	ESP_WaitForOk();
	ESP_SendCommand("AT+CIPMUX=0\r\n");
	ESP_WaitForOk();
//	ESP_SendCommand("AT+RESTORE\r\n");
//	ESP_WaitForOk();
	return true;
}
