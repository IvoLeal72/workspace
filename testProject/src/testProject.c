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
#include "UART.h"
#include "led.h"
#include "wait.h"

int main(void) {
	printf("UART Test\n");
	WAIT_Init();
	LED_Init(true);
	UART_Initialize(2, 0, 115200);
	char* testStr="O ivo teve dias a volta de um erro parvo\n";
	while(*testStr) {
		LED_On();
		UART_WriteChar(2,*testStr++);
		LED_Off();
		printf("%c", UART_ReadChar(2));
	}
	return 0;
}
