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
#include "Utils.h"

#define RED (1<<22)
#define GREEN (1<<25)
#define BLUE (1<<26)


void LED_Init(char state){
	pin_Config(0, 22, 0, PULLUP, NORMAL);
	pin_Config(3, 25, 0, PULLUP, NORMAL);
	pin_Config(3, 26, 0, PULLUP, NORMAL);
	LPC_GPIO0->FIODIR |= (1<<22);
	LPC_GPIO3->FIODIR |= (3<<25);
	LED_SetState(state);
}

char LED_GetState(void){
	char state=0;
	if(!(LPC_GPIO0->FIOPIN & RED)){
		state|=0b100;
	}
	int var=LPC_GPIO3->FIOPIN;
	if(!(var & GREEN)){
		state|=0b010;
	}
	if(!(var & BLUE)){
		state|=0b001;
	}
	return state;
}

void LED_SetState(char state){
	if(state&4)
		LPC_GPIO0->FIOCLR=RED;
	else
		LPC_GPIO0->FIOSET=RED;

	if(state&2)
		LPC_GPIO3->FIOCLR=GREEN;
	else
		LPC_GPIO3->FIOSET=GREEN;

	if(state&1)
		LPC_GPIO3->FIOCLR=BLUE;
	else
		LPC_GPIO3->FIOSET=BLUE;
}

