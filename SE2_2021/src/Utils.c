/*
 * Utils.c
 *
 *  Created on: 11/04/2021
 *      Author: ivogc
 */

#ifdef __USE_CMSIS
	#include "LPC17xx.h"
#endif
#include "Utils.h"

static void set_2bits(__IO uint32_t* reg, char value, char lsb){
	lsb&=~1;
	if(lsb>30 || lsb<0) return;
	switch(value){
		case 0:
			*reg &= ~(3<<lsb);
			break;
		case 1:
			*reg |= 1<<lsb;
			*reg &= ~(1<<(lsb+1));
			break;
		case 2:
			*reg &= ~(1<<lsb);
			*reg |= 1<<(lsb+1);
			break;
		case 3:
			*reg |= 3<<lsb;
	}
}

static void set_bit(__IO uint32_t* reg, char value, char lsb){
	if(lsb>31 || lsb<0) return;
	switch(value){
		case 0:
			*reg&=~(1<<lsb);
			break;
		case 1:
			*reg |= 1<<lsb;
	}
}

void set_PCLK(char divider, char reg, char lsb){
	__IO uint32_t* pclksel;
	if(reg==0) pclksel=&(LPC_SC->PCLKSEL0);
	else if(reg==1) pclksel=&(LPC_SC->PCLKSEL1);
	else return;
	switch(divider){
		case 1:
			set_2bits(pclksel, 1, lsb);
			break;
		case 2:
			set_2bits(pclksel, 2, lsb);
			break;
		case 4:
			set_2bits(pclksel, 0, lsb);
			break;
		case 6: case 8:
			set_2bits(pclksel, 3, lsb);
	}
}

void set_PINSEL(char mode, char reg, char lsb){
	switch(reg){
		case 0: set_2bits(&(LPC_PINCON->PINSEL0), mode, lsb); break;
		case 1: set_2bits(&(LPC_PINCON->PINSEL1), mode, lsb); break;
		case 2: set_2bits(&(LPC_PINCON->PINSEL2), mode, lsb); break;
		case 3: set_2bits(&(LPC_PINCON->PINSEL3), mode, lsb); break;
		case 4: set_2bits(&(LPC_PINCON->PINSEL4), mode, lsb); break;
		case 7: set_2bits(&(LPC_PINCON->PINSEL7), mode, lsb); break;
		case 8: set_2bits(&(LPC_PINCON->PINSEL8), mode, lsb); break;
		case 9: set_2bits(&(LPC_PINCON->PINSEL9), mode, lsb); break;
		case 10: set_2bits(&(LPC_PINCON->PINSEL10), mode, lsb);
	}
}

void set_PINMODE(char mode, char reg, char lsb){
	switch(reg){
		case 0: set_2bits(&(LPC_PINCON->PINMODE0), mode, lsb); break;
		case 1: set_2bits(&(LPC_PINCON->PINMODE1), mode, lsb); break;
		case 2: set_2bits(&(LPC_PINCON->PINMODE2), mode, lsb); break;
		case 3: set_2bits(&(LPC_PINCON->PINMODE3), mode, lsb); break;
		case 4: set_2bits(&(LPC_PINCON->PINMODE4), mode, lsb); break;
		case 5: set_2bits(&(LPC_PINCON->PINMODE5), mode, lsb); break;
		case 6: set_2bits(&(LPC_PINCON->PINMODE6), mode, lsb); break;
		case 7: set_2bits(&(LPC_PINCON->PINMODE7), mode, lsb); break;
		case 9: set_2bits(&(LPC_PINCON->PINMODE9), mode, lsb);
	}
}

void set_PINMODE_OD(char mode, char reg, char lsb){
	switch(reg){
		case 0: set_bit(&(LPC_PINCON->PINMODE_OD0), mode, lsb); break;
		case 1: set_bit(&(LPC_PINCON->PINMODE_OD1), mode, lsb); break;
		case 2: set_bit(&(LPC_PINCON->PINMODE_OD2), mode, lsb); break;
		case 3: set_bit(&(LPC_PINCON->PINMODE_OD3), mode, lsb); break;
		case 4: set_bit(&(LPC_PINCON->PINMODE_OD4), mode, lsb);
	}
}

void pin_Config(char gpio, char pin, char func, char res_mode, char dr_mode){
	set_PINSEL(func, gpio*2+(pin>15?1:0), (pin&0xf)*2);
	set_PINMODE(func, gpio*2+(pin>15?1:0), (pin&0xf)*2);
	set_PINMODE_OD(dr_mode, gpio, pin);
}

