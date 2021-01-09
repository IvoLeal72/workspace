/*
 * spi.c
 *
 *  Created on: 09/01/2021
 *      Author: ivo
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

void SPI_Init(void){
	LPC_SC->PCONP |= 1<<8;

	LPC_SC->PCLKSEL0 |= 0b11<<16;

	LPC_PINCON->PINSEL0 |= 0b11<<30;
	LPC_PINCON->PINSEL1 |= 0x3f;

	LPC_SPI->SPCR |= (1<<2 | 1<<5);
	LPC_SPI->SPCR &= ~(3<<6);
}

void SPI_ConfigTransfer(int frequency, int bitData, int mode){
	LPC_SPI->SPCR &= ~(0xf<<8);
	LPC_SPI->SPCR |= (bitData&0xf)<<8;

	LPC_SPI->SPCR &= ~(3<<3);
	LPC_SPI->SPCR |= (mode & 3)<<3;

	LPC_SPI->SPCCR = SystemCoreClock/(8*frequency);
}

int SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght){
	return 0;
}

