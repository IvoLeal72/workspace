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

bool UART_GetChar(unsigned char *ch){
	if ((UARTx->LSR & UART_LSR_RDR) == 0)
		return false;
	*ch = UARTx->RBR;
	return true;
}

bool UART_IsChar(void){
	return (UARTx->LSR & UART_LSR_RDR) != 0;
}

unsigned char UART_ReadChar(void){
	while ((UARTx->LSR & UART_LSR_RDR) == 0);
	return UARTx->RBR;
}

void UART_WriteChar(unsigned char ch){
	while ((UARTx->LSR & UART_LSR_THRE) == 0);
	UARTx->THR = ch;
}

void UART_WriteString(char *str){
	while (*str) {
		UART_WriteChar(*str++);
	}
}

void UART_WriteBuffer(unsigned char *buffer, int len){
	int i;
	for (i = 0; i < len; i++){
		UART_WriteChar(*buffer++);
	}
}
