/*
 * button.c
 *
 *  Created on: 03/11/2020
 *      Author: ivo
 */
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "button.h"

static int lastState=0;

void BUTTON_Init(void){
	LPC_PINCON->PINSEL0=LPC_PINCON->PINSEL0 & ~0x3f;
	LPC_PINCON->PINMODE0=LPC_PINCON->PINMODE0 | 0x3f;
	LPC_GPIO0->FIODIR=LPC_GPIO0->FIODIR & ~7;
}

int BUTTON_Hit(void){
	return LPC_GPIO0->FIOPIN & 7;
}

int BUTTON_Read(void){
	int aux;
	while((aux=BUTTON_Hit())==0);
	return aux;
}

//for every button return 2 bits: 00 if unmodified, 01 if pressed and 10 if released
int BUTTON_GetButtonsEvents (void){
	int state=BUTTON_Hit();
	int result=0;
	for(int i=2; i>=0; i--){
		result<<=2;
		int stateBit=(state>>i) &1;
		int lastStateBit=(lastState>>i) &1;
		if(stateBit<lastStateBit) result+=0b10;
		else if(stateBit>lastStateBit) result+=0b01;
	}
	lastState=state;
	return result;
}

