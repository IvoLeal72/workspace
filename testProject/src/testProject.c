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
#include "pUART.h"
#include "led.h"
#include "wait.h"
#include "lcd.h"

int main(void) {
	WAIT_Init();
	LED_Init(true);
	LCDText_Init();
	LCDText_Printf("UART Test\n");
	pUART_Initialize(2, 0, 115200);
	char* testStr="Frase de teste";
	char strRec[strlen(testStr)+2];
	LED_Off();
	pUART_WriteString(2, testStr);
	pUART_ReadString(2, strRec, strlen(testStr));
	LED_On();
	LCDText_Printf("%s\n", strRec);
	return 0;
}
