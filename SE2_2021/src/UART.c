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

#define UART_CTT 4
#define UART_CCLK_DIVIDER 8

struct UART_controller{
	LPC_UART_TypeDef* perif;
};

static struct UART_controller UART_ctrl_arr[UART_CTT]={{LPC_UART0}, {LPC_UART1}, {LPC_UART2}, {LPC_UART3}};

bool UART_GetChar(int id, unsigned char *ch){
	if ((UART_ctrl_arr[id].perif->LSR & UART_LSR_RDR) == 0)
		return false;
	*ch = UART_ctrl_arr[id].perif->RBR;
	return true;
}

bool UART_IsChar(int id){
	return (UART_ctrl_arr[id].perif->LSR & UART_LSR_RDR) != 0;
}

unsigned char UART_ReadChar(int id){
	while ((UART_ctrl_arr[id].perif->LSR & UART_LSR_RDR) == 0);
	return UART_ctrl_arr[id].perif->RBR;
}

void UART_WriteChar(int id, unsigned char ch){
	while ((UART_ctrl_arr[id].perif->LSR & UART_LSR_THRE) == 0);
	UART_ctrl_arr[id].perif->THR = ch;
}

void UART_WriteString(int id, char *str){
	while (*str) {
		UART_WriteChar(id, *str++);
	}
}

void UART_WriteBuffer(int id, unsigned char *buffer, int len){
	int i;
	for (i = 0; i < len; i++){
		UART_WriteChar(id, *buffer++);
	}
}

bool UART_Initialize(int id, int options, unsigned int baud){
	switch(id){
		case 0:
			set_PCLK(UART_CCLK_DIVIDER, 0, 6);
			pinConfig(0, 2, 1, NEITHER, NORMAL);
			pinConfig(0, 3, 1, NEITHER, NORMAL);
			break;
		case 1:
			set_PCLK(UART_CCLK_DIVIDER, 0, 8);
			break;
		case 2:
			set_PCLK(UART_CCLK_DIVIDER, 1, 16);
			break;
		case 3:
			set_PCLK(UART_CCLK_DIVIDER, 1, 18);
			break;
		default:
			return false;
	}
}
