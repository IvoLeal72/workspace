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
#include "esp8266.h"

int main(void) {
	//UART_Initialize(0,0,0);
	WAIT_Init();
	LED_Init(false);
	ESP_Init();
	LED_On();
	return 0;
}
