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
#include "spi.h"

static int ADXL345_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int length){
	SPI_ConfigTransfer(1000000, 8, 0b11);

	LPC_GPIO0->FIOCLR=1<<21;
	int res=SPI_Transfer(txBuffer, rxBuffer, length);
	LPC_GPIO0->FIOSET=1<<21;

	return res;
}

void ADXL345_Init(){
	SPI_Init();
	LPC_GPIO0->FIOSET=1<<21;
	LPC_GPIO0->FIODIR|=1<<21;
	LPC_PINCON->PINSEL1&=~(0b11<10);

	unsigned short txBuffer[2]={0x2D, 0x08};
	unsigned short rxBuffer[2];

	ADXL345_Transfer(txBuffer, rxBuffer, 2);
}


unsigned char ADXL345_GetId(){
	unsigned short txBuffer[2]={0x80, 0x07};
	unsigned short rxBuffer[2]={0};

	ADXL345_Transfer(txBuffer, rxBuffer, 2);

	return (unsigned char)rxBuffer[1];
}

int ADXL345_GetValues(short values[3]){
	unsigned short txBuffer[7]={0x80|0x40|0x32};
	unsigned short rxBuffer[7];
	int res=ADXL345_Transfer(txBuffer, rxBuffer, 7);

	values[0]=rxBuffer[1]|rxBuffer[2]<<8; //X
	values[1]=rxBuffer[3]|rxBuffer[4]<<8; //Y
	values[2]=rxBuffer[5]|rxBuffer[6]<<8; //Z

	return res;
}

short ADXL345_GetX(){
	unsigned short txBuffer[3]={0x80|0x40|0x32};
	unsigned short rxBuffer[3]={};

	ADXL345_Transfer(txBuffer, rxBuffer, 3);

	return rxBuffer[1]|rxBuffer[2]<<8;
}

short ADXL345_GetY(){
	unsigned short txBuffer[3]={0x80|0x40|0x34};
	unsigned short rxBuffer[3]={};

	ADXL345_Transfer(txBuffer, rxBuffer, 3);

	return rxBuffer[1]|rxBuffer[2]<<8;
}

short ADXL345_GetZ(){
	unsigned short txBuffer[3]={0x80|0x40|0x36};
	unsigned short rxBuffer[3]={};

	ADXL345_Transfer(txBuffer, rxBuffer, 3);

	return rxBuffer[1]|rxBuffer[2]<<8;
}
