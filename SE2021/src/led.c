/*
 * led.c
 *
 *  Created on: 03/11/2020
 *      Author: ivo
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "led.h"

void LED_Init(bool state){
	LPC_PINCON->PINSEL1 &= ~(3<<12);
	LPC_GPIO0->FIODIR |= (1<<22);
	if(state)
		LPC_GPIO0->FIOCLR=1<<22;
	else
		LPC_GPIO0->FIOSET=1<<22;
}

bool LED_GetState(void){
	return  !(LPC_GPIO0->FIOPIN & (1<<22));
}

void LED_On(void){
	LPC_GPIO0->FIOCLR=1<<22;
}

void LED_Off(void){
	LPC_GPIO0->FIOSET=1<<22;
}
