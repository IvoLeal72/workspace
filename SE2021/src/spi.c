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

	LPC_PINCON->PINSEL0 |= 0b11<<30;
	LPC_PINCON->PINSEL1 |= 0x3f;

	LPC_SPI->SPCCR |= (1<<2 | 1<<5);
	LPC_SPI->SPCCR &= ~(3<<6);
}
