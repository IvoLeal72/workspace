/*
 * UART.c
 *
 *  Created on: 05/05/2021
 *      Author: ivo
 */


#ifdef __USE_CMSIS
	#include "LPC17xx.h"
#endif

#include "UART.h"

bool UART_Initailize(int id, unsigned int baud){
	return false;
}

uint32_t UART_ReadBuffer(int id, char* buffer, size_t len);
uint32_t UART_WriteBuffer(int id, char* buffer, size_t len);
