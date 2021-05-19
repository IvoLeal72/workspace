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
	//printf("UART Test\n");
	//WAIT_Init();
	//LED_Init(true);
	UART_Initialize(2, 0, 115200);
	int tstCh = '0';
	while (1) {
		LED_On();
		UART_WriteChar(2,tstCh);
		if (++tstCh > '9') tstCh = '0';
		LED_Off();
		printf("rx = %c\n", UART_ReadChar(2));
		WAIT_Milliseconds(500);
	}
	return 0;
}
