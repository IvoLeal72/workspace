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
#include "Utils.h"

#define UART_CTT 4
#define UART0_PWR (1<<3)
#define UART1_PWR (1<<4)
#define UART2_PWR (1<<24)
#define UART3_PWR (1<<25)
#define UART_CCLK_DIVIDER 8
#define UART_FCR_FIFO_EN 1
#define UART_FCR_RX_RS 2
#define UART_FCR_TX_RS 4
#define UART_LSR_RDR 1
#define UART_LSR_THRE (1<<5)
#define UART_TER_TXEN (1<<7)
#define UART_LCR_DLAB_EN (1<<7)
#define UART_LCR_BREAK_EN (1<<6)
#define UART_LCR_BITMASK 0xff
#define UART_LCR_WLEN8 3
#define UART_IIR_PENDING 1
#define UART_IIR_INTID_MASK (0xE)
#define UART_IIR_INTID_RLS 6
#define UART_IIR_INTID_RDA 4
#define UART_IIR_INTID_CTI 0xC
#define UART_IIR_INTID_THRE 2
#define UART_FCR_TRG_0 0
#define UART_FCR_TRG_1 (1<<6)
#define UART_FCR_TRG_2 (2<<6)
#define UART_FCR_TRG_3 (3<<6)
#define UART_IER_RBR 1
#define UART_IER_RLS 4
#define UART_IER_THRE 2
#define UART_IER_MASK 0x307

#define UART_RBUFSIZE (1 << 11)
#define RBUF_MASK (UART_RBUFSIZE-1)
#define RBUF_IS_FULL(wr, rd) ((rd&RBUF_MASK)==((wr+1)&RBUF_MASK))
#define RBUF_WILL_FULL(wr, rd) ((rd&RBUF_MASK)==((wr+2)&RBUF_MASK))
#define RBUF_IS_EMPTY(wr, rd) ((wr&RBUF_MASK)==(rd&RBUF_MASK))
#define RBUF_RESET(bufidx) (bufidx=0)
#define RBUF_INCR(bufidx) (bufidx=(bufidx+1)&RBUF_MASK)


struct UART_controller{
	LPC_UART_TypeDef* perif;
	bool txStopped;
	__IO uint32_t txWrite;
	__IO uint32_t txRead;
	__IO uint32_t rxWrite;
	__IO uint32_t rxRead;
	__IO uint8_t tx[UART_RBUFSIZE];
	__IO uint8_t rx[UART_RBUFSIZE];
};

static struct UART_controller UART_ctrl_arr[UART_CTT]={{(LPC_UART_TypeDef*)LPC_UART0}, {(LPC_UART_TypeDef*)LPC_UART1}, {LPC_UART2}, {LPC_UART3}};

void UARTn_IRQHandler(int id){
	uint32_t iir, lsr;
	struct UART_controller* UARTx=&UART_ctrl_arr[id];
	while(((iir=UARTx->perif->IIR) & UART_IIR_PENDING)==0){
		switch(iir & UART_IIR_INTID_MASK){
			case UART_IIR_INTID_RLS:
				lsr=UARTx->perif->LSR;
				//for(;;);
				(void) lsr;
				break;
			case UART_IIR_INTID_RDA:
			case UART_IIR_INTID_CTI:
				while ((UARTx->perif->LSR & UART_LSR_RDR)!=0 && !RBUF_IS_FULL(UARTx->rxWrite, UARTx->rxRead)){
					UARTx->rx[UARTx->rxWrite]=UARTx->perif->RBR;
					RBUF_INCR(UARTx->rxWrite);
				}
				break;
			case UART_IIR_INTID_THRE:
				if(RBUF_IS_EMPTY(UARTx->txWrite, UARTx->txRead) && (UARTx->perif->LSR & UART_LSR_THRE)){
					UARTx->txStopped=true;
					UARTx->perif->IER = UARTx->perif->IER & ~UART_IER_THRE & UART_IER_MASK;
				}
				else{
					while (!RBUF_IS_EMPTY(UARTx->txWrite, UARTx->txRead) && ((UARTx->perif->LSR & UART_LSR_THRE) != 0)) {
						UARTx->perif->THR=UARTx->tx[UARTx->txRead];
						RBUF_INCR(UARTx->txRead);
						UARTx->txStopped=false;
					}
				}
				break;
		}
	}
}

void UART0_IRQHandler(void){
	UARTn_IRQHandler(0);
}

void UART1_IRQHandler(void){
	UARTn_IRQHandler(1);
}

void UART2_IRQHandler(void){
	UARTn_IRQHandler(2);
}

void UART3_IRQHandler(void){
	UARTn_IRQHandler(3);
}

uint32_t UART_WriteBuffer(int id, uint8_t *buffer, uint32_t len){
	uint32_t bytes=0;
	struct UART_controller* UARTx=&UART_ctrl_arr[id];
	while(!RBUF_IS_FULL(UARTx->txWrite, UARTx->txRead) && bytes<len){
		UARTx->tx[UARTx->txWrite]=buffer[bytes++];
		RBUF_INCR(UARTx->txWrite);
	}
	if(UARTx->txStopped){
		UARTx->perif->THR=UARTx->tx[UARTx->txRead];
		RBUF_INCR(UARTx->txRead);
		UARTx->perif->IER|=UART_IER_THRE;
	}
	return bytes;
}

uint32_t UART_ReadBuffer(int id, uint8_t *buffer, uint32_t len){
	uint32_t bytes=0;
	struct UART_controller* UARTx=&UART_ctrl_arr[id];
	while(!RBUF_IS_EMPTY(UARTx->rxWrite, UARTx->rxRead) && bytes<len){
		buffer[bytes]=UARTx->rx[UARTx->rxRead];
		RBUF_INCR(UARTx->rxRead);
		bytes++;
	}
	return bytes;
}

bool UART_Initialize(int id, int options, unsigned int baud){
	switch(id){
		case 0:
			LPC_SC->PCONP|=UART0_PWR;
			set_PCLK(UART_CCLK_DIVIDER, 0, 6);
			if(options!=0) return false;
			pin_Config(0, 2, 1, PULLUP, NORMAL);
			pin_Config(0, 3, 1, PULLUP, NORMAL);
			break;
		case 1:
			LPC_SC->PCONP|=UART1_PWR;
			set_PCLK(UART_CCLK_DIVIDER, 0, 8);
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
			LPC_SC->PCONP|=UART2_PWR;
			set_PCLK(UART_CCLK_DIVIDER, 1, 16);
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
			LPC_SC->PCONP|=UART3_PWR;
			set_PCLK(UART_CCLK_DIVIDER, 1, 18);
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
	UART_ctrl_arr[id].perif->LCR&=(~UART_LCR_DLAB_EN)&UART_LCR_BITMASK;

	UART_ctrl_arr[id].perif->FCR = ( UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS);
	UART_ctrl_arr[id].perif->FCR = 0;

	while (UART_ctrl_arr[id].perif->LSR & UART_LSR_RDR) {// Dummy reading - empty rx!
		tmp = UART_ctrl_arr[id].perif->RBR;
	}
	UART_ctrl_arr[id].perif->TER = UART_TER_TXEN;
	while (!(UART_ctrl_arr[id].perif->LSR & UART_LSR_THRE));

	UART_ctrl_arr[id].perif->TER = 0;   // Disable Tx
	UART_ctrl_arr[id].perif->IER = 0;	// Disable interrupt
	UART_ctrl_arr[id].perif->LCR = 0;	// Set LCR to default state
	UART_ctrl_arr[id].perif->ACR = 0;

	if(id==1){
		LPC_UART1->MCR=0;
		LPC_UART1->RS485CTRL=0;
		LPC_UART1->RS485DLY=0;
		LPC_UART1->ADRMATCH=0;
		tmp=LPC_UART1->MSR;
	}

	tmp = UART_ctrl_arr[id].perif->LSR;

	double k=(double)SystemCoreClock/UART_CCLK_DIVIDER;
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
	UART_ctrl_arr[id].perif->LCR|=UART_LCR_DLAB_EN;
	UART_ctrl_arr[id].perif->DLM=UnDL>>8;
	UART_ctrl_arr[id].perif->DLL=UnDL&0xff;
	UART_ctrl_arr[id].perif->FDR=best_add|best_mul<<4;
	UART_ctrl_arr[id].perif->LCR&=(~UART_LCR_DLAB_EN)&UART_LCR_BITMASK;

	UART_ctrl_arr[id].perif->LCR = (uint8_t)UART_LCR_WLEN8;
	UART_ctrl_arr[id].perif->TER = UART_TER_TXEN;
	//UART_ctrl_arr[id].perif->FCR=UART_FCR_FIFO_EN | UART_FCR_TRG_0;
	UART_ctrl_arr[id].perif->FCR=0;
	UART_ctrl_arr[id].txStopped=true;
	RBUF_RESET(UART_ctrl_arr[id].rxWrite);
	RBUF_RESET(UART_ctrl_arr[id].rxRead);
	RBUF_RESET(UART_ctrl_arr[id].txWrite);
	RBUF_RESET(UART_ctrl_arr[id].txRead);

	UART_ctrl_arr[id].perif->IER=UART_IER_RBR | UART_IER_RLS;

	uint32_t iir;
	iir=UART_ctrl_arr[id].perif->IIR & UART_IIR_INTID_MASK;
	while(1){
		if((iir == UART_IIR_INTID_RDA) || (iir == UART_IIR_INTID_CTI))
			tmp=UART_ctrl_arr[id].perif->RBR;
		else if(iir==UART_IIR_INTID_RLS)
			tmp=UART_ctrl_arr[id].perif->LSR;
		else break;
		iir=UART_ctrl_arr[id].perif->IIR & UART_IIR_INTID_MASK;
	}

	(void) tmp;

	NVIC_SetPriority(UART0_IRQn+id, 0x9);
	NVIC_EnableIRQ(UART0_IRQn+id);
	return true;
}
