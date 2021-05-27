/*
===============================================================================
 Name        : testProject.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "led.h"
#include "wait.h"
#include "lcd.h"

int main(void) {
	WAIT_Init();
	LED_Init(true);
	LCDText_Init();
	LCDText_Printf("UART Test\n");
	UART_Initialize(2, 0, 115200);
	char* testStr="Frase de teste";
	char strRec[strlen(testStr)+1];
	strRec[strlen(testStr)]=0;
	LED_Off();
	UART_WriteBuffer(2, (uint8_t*)testStr, strlen(testStr));
	for(uint32_t read=0;read<strlen(testStr);){
		read+=UART_ReadBuffer(2, (uint8_t*)strRec+read, strlen(testStr)-read);
	}
	LED_On();
	LCDText_Printf("%s\n", strRec);
	WAIT_Milliseconds(2000);
	LCDText_Clear();
	char* testStr2="Um teste como antes, mas maior!";
	char strRec2[strlen(testStr2)+1];
	strRec2[strlen(testStr2)]=0;
	LED_Off();
	UART_WriteBuffer(2, (uint8_t*)testStr2, strlen(testStr2));
	for(uint32_t read=0;read<strlen(testStr2);){
		read+=UART_ReadBuffer(2, (uint8_t*)strRec2+read, strlen(testStr2)-read);
	}
	LED_On();
	LCDText_Printf("%s\n", strRec2);
	return 0;
}
