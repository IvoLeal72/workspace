/*
 * UART.c
 *
 *  Created on: 05/05/2021
 *      Author: ivo
 */


#ifdef __USE_CMSIS
	#include "LPC17xx.h"
#endif

#include "pUART.h"
#include "Utils.h"

#define pUART_CTT 4
#define pUART0_PWR (1<<3)
#define pUART1_PWR (1<<4)
#define pUART2_PWR (1<<24)
#define pUART3_PWR (1<<25)
#define pUART_CCLK_DIVIDER 8
#define pUART_FCR_FIFO_EN 1
#define pUART_FCR_RX_RS 2
#define pUART_FCR_TX_RS 4
#define pUART_LSR_RDR 1
#define pUART_LSR_THRE (1<<5)
#define pUART_TER_TXEN (1<<7)
#define pUART_LCR_DLAB_EN (1<<7)
#define pUART_LCR_BREAK_EN (1<<6)
#define pUART_LCR_BITMASK 0xff
#define pUART_LCR_WLEN8 3

#define pUARTx (pUART_ctrl_arr[id].perif)


struct pUART_controller{
	LPC_UART_TypeDef* perif;
};

static struct pUART_controller pUART_ctrl_arr[pUART_CTT]={{(LPC_UART_TypeDef*)LPC_UART0}, {(LPC_UART_TypeDef*)LPC_UART1}, {LPC_UART2}, {LPC_UART3}};

bool pUART_GetChar(int id, unsigned char *ch){
	if ((pUARTx->LSR & pUART_LSR_RDR) == 0)
		return false;
	*ch = pUARTx->RBR;
	return true;
}

bool pUART_IsChar(int id){
	return (pUARTx->LSR & pUART_LSR_RDR) != 0;
}

unsigned char pUART_ReadChar(int id){
	while ((pUARTx->LSR & pUART_LSR_RDR) == 0);
	return pUARTx->RBR;
}

void pUART_ReadBuffer(int id, char* buff, size_t buffSize){
	for(int i=0; i<buffSize; i++) buff[i]=pUART_ReadChar(id);
}

void pUART_ReadString(int id, char* str, size_t strSize){
	char c;
	int i;
	for(i=0; i<strSize; i++){
		c=pUART_ReadChar(id);
		if(c==0) break;
		str[i]=c;
	}
	str[i]=0;
}

void pUART_WriteChar(int id, unsigned char ch){
	while ((pUARTx->LSR & pUART_LSR_THRE) == 0);
	pUARTx->THR = ch;
}

void pUART_WriteString(int id, char *str){
	while (*str) {
		pUART_WriteChar(id, *str++);
	}
	pUART_WriteChar(id, 0);
}

void pUART_WriteBuffer(int id, unsigned char *buffer, int len){
	int i;
	for (i = 0; i < len; i++){
		pUART_WriteChar(id, *buffer++);
	}
}

bool pUART_Initialize(int id, int options, unsigned int baud){
	switch(id){
		case 0:
			LPC_SC->PCONP|=pUART0_PWR;
			set_PCLK(pUART_CCLK_DIVIDER, 0, 6);
			if(options!=0) return false;
			pin_Config(0, 2, 1, PULLUP, NORMAL);
			pin_Config(0, 3, 1, PULLUP, NORMAL);
			break;
		case 1:
			LPC_SC->PCONP|=pUART1_PWR;
			set_PCLK(pUART_CCLK_DIVIDER, 0, 8);
			switch(options){
				case 0:
					pin_Config(0, 15, 1, PULLUP, NORMAL);
					pin_Config(0, 16, 1, PULLUP, NORMAL);
					break;
				case 1:
					pin_Config(2, 0, 2, PULLUP, NORMAL);
					pin_Config(2, 1, 2, PULLUP, NORMAL);
					break;
				default:
					return false;
			}
			break;
		case 2:
			LPC_SC->PCONP|=pUART2_PWR;
			set_PCLK(pUART_CCLK_DIVIDER, 1, 16);
			switch(options){
				case 0:
					pin_Config(0, 10, 1, PULLUP, NORMAL);
					pin_Config(0, 11, 1, PULLUP, NORMAL);
					break;
				case 1:
					pin_Config(2, 8, 2, PULLUP, NORMAL);
					pin_Config(2, 9, 2, PULLUP, NORMAL);
					break;
				default:
					return false;
			}
			break;
		case 3:
			LPC_SC->PCONP|=pUART3_PWR;
			set_PCLK(pUART_CCLK_DIVIDER, 1, 18);
			switch(options){
				case 0:
					pin_Config(0, 0, 2, PULLUP, NORMAL);
					pin_Config(0, 1, 2, PULLUP, NORMAL);
					break;
				case 1:
					pin_Config(0, 25, 3, PULLUP, NORMAL);
					pin_Config(0, 26, 3, PULLUP, NORMAL);
					break;
				case 2:
					pin_Config(4, 28, 3, PULLUP, NORMAL);
					pin_Config(4, 29, 3, PULLUP, NORMAL);
					break;
				default:
					return false;
			}
			break;
		default:
			return false;
	}

	int tmp;
	pUARTx->LCR&=(~pUART_LCR_DLAB_EN)&pUART_LCR_BITMASK;

	pUARTx->FCR = ( pUART_FCR_FIFO_EN | pUART_FCR_RX_RS | pUART_FCR_TX_RS);
	pUARTx->FCR = 0;

	while (pUARTx->LSR & pUART_LSR_RDR) {// Dummy reading - empty rx!
		tmp = pUARTx->RBR;
	}
	pUARTx->TER = pUART_TER_TXEN;
	while (!(pUARTx->LSR & pUART_LSR_THRE));

	pUARTx->TER = 0;   // Disable Tx
	pUARTx->IER = 0;	// Disable interrupt
	pUARTx->LCR = 0;	// Set LCR to default state
	pUARTx->ACR = 0;

	if(id==1){
		LPC_UART1->MCR=0;
		LPC_UART1->RS485CTRL=0;
		LPC_UART1->RS485DLY=0;
		LPC_UART1->ADRMATCH=0;
		tmp=LPC_UART1->MSR;
	}

	tmp = pUARTx->LSR;

	double k=(double)SystemCoreClock/pUART_CCLK_DIVIDER;
	k/=16*baud;
	int UnDL=(int)k;
	if(UnDL>=1<<16) return false;
	double rem=k-UnDL;
	int best_add=0;
	int best_mul=1;
	double best_distance=0;
	for(int mul=1; mul<=15; mul++){
		for(int add=0; add<mul; add++){

			double test_rem=(double)UnDL*(double)add/(double)mul;

			double distance=test_rem-rem;
			if(distance<0) distance=-distance;

			if((mul==1 && add==0) || distance<best_distance){
				best_add=add;
				best_mul=mul;
				best_distance=distance;
			}
		}
	}
	pUARTx->LCR|=pUART_LCR_DLAB_EN;
	pUARTx->DLM=UnDL>>8;
	pUARTx->DLL=UnDL&0xff;
	pUARTx->FDR=best_add|best_mul<<4;
	pUARTx->LCR&=(~pUART_LCR_DLAB_EN)&pUART_LCR_BITMASK;

	pUARTx->LCR = (uint8_t)pUART_LCR_WLEN8;
	pUARTx->TER = pUART_TER_TXEN;
	pUARTx->FCR=pUART_FCR_FIFO_EN;
	return true;
}
