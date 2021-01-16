/*
 * ADXL345.c
 *
 *  Created on: 15/01/2021
 *      Author: ivogc
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "ADXL345.h"

void ADXL345_Init(){
	LPC_GPIO0->FIOSET=1<<21;
	LPC_GPIO0->FIODIR|=1<<21;
	LPC_PINCON->PINSEL1&=~(0b11<10);
}

static int ADXL345_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int length){
	SPI_ConfigTransfer(1000000, 8, 0b11);

	LPC_GPIO0->FIOCLR=1<<21;
	int res=SPI_Transfer(txBuffer, rxBuffer, length);
	LPC_GPIO0->FIOSET=1<<21;

	return res;
}

unsigned char ADXL345_GetId(){
	unsigned short txBuffer[2]={0x80, 0x07};
	unsigned short rxBuffer[2]={0};

	ADXL345_Transfer(txBuffer, rxBuffer, 2);

	return (unsigned char)rxBuffer[1];
}
