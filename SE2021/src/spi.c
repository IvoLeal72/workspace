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

	LPC_SC->PCLKSEL0 &= ~(0b11<<16);

	LPC_PINCON->PINSEL0 |= 0b11<<30;
	LPC_PINCON->PINSEL1 |= 0x3c;

	LPC_SPI->SPCR |= (1<<2 | 1<<5);
	LPC_SPI->SPCR &= ~(3<<6);
}

void SPI_ConfigTransfer(int frequency, int bitData, int mode){
	LPC_SPI->SPCR &= ~(0xf<<8);
	LPC_SPI->SPCR |= (bitData&0xf)<<8;

	LPC_SPI->SPCR &= ~(3<<3);
	LPC_SPI->SPCR |= (mode & 3)<<3;

	int aux = SystemCoreClock/(4*frequency);
	aux&=~1;
	if(aux<8) aux=8;
	else if(aux>0xfe) aux=0xfe;
	LPC_SPI->SPCCR=aux;
}

int SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int length){
	int aux;
	for(int i=0; i<length; i++){
		LPC_SPI->SPDR=txBuffer[i];
		do{
			aux=LPC_SPI->SPSR & 0xf8;
			if(aux & 0x78) return aux;
		}while(aux==0);
		rxBuffer[i]=LPC_SPI->SPDR;
	}
	return 0;
}

