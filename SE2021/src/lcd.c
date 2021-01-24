/*
 * lcd.c
 *
 *  Created on: 17/11/2020
 *      Author: ivo
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "lcd.h"
#include "wait.h"

static int grow=0, gcol=0;

static void LCDText_sendData8(bool dataRegister, uint8_t data){
	if(dataRegister) LPC_GPIO0->FIOSET=1<<3;
	else LPC_GPIO0->FIOCLR=1<<3;
	WAIT_ChronoUs(1);
	LPC_GPIO0->FIOSET=1<<16;
	LPC_GPIO0->FIOCLR=0xF<<23;
	LPC_GPIO0->FIOSET=((data>>4)&0xF)<<23;
	WAIT_ChronoUs(10);
	LPC_GPIO0->FIOCLR=1<<16;
	WAIT_ChronoUs(10);
	LPC_GPIO0->FIOSET=1<<16;
	LPC_GPIO0->FIOCLR=0xF<<23;
	LPC_GPIO0->FIOSET=((data)&0xF)<<23;
	WAIT_ChronoUs(10);
	LPC_GPIO0->FIOCLR=1<<16;
	WAIT_ChronoUs(10);
}

static void LCDText_sendData4(bool dataRegister, uint8_t data){
	if(dataRegister) LPC_GPIO0->FIOSET=1<<3;
	else LPC_GPIO0->FIOCLR=1<<3;
	WAIT_ChronoUs(1);
	LPC_GPIO0->FIOSET=1<<16;
	LPC_GPIO0->FIOCLR=0xF<<23;
	LPC_GPIO0->FIOSET=(data&0xF)<<23;
	WAIT_ChronoUs(1);
	LPC_GPIO0->FIOCLR=1<<16;
	WAIT_ChronoUs(1);
}

void LCDText_Init(){
	LPC_GPIO0->FIODIR|=(1<<3 | 1<<16 | 0xF<<23);
	LPC_GPIO0->FIOCLR=1<<16;
	LPC_PINCON->PINSEL0 &= ~(0b11<<6);
	LPC_PINCON->PINSEL1 &= ~((0xFF<<14) | 3);

	WAIT_Milliseconds(41);
	LCDText_sendData4(false, 0b11);
	WAIT_Milliseconds(5);
	LCDText_sendData4(false, 0b11);
	WAIT_ChronoUs(100);
	LCDText_sendData4(false, 0b11);
	WAIT_ChronoUs(39);
	LCDText_sendData4(false, 0b10);
	WAIT_ChronoUs(39);
	LCDText_sendData8(false, 0b101000);
	WAIT_ChronoUs(39);
	LCDText_sendData8(false, 0b1100);
	WAIT_ChronoUs(39);
	LCDText_sendData8(false, 1);
	WAIT_Milliseconds(3);
	LCDText_sendData8(false, 0b110);
	WAIT_ChronoUs(39);
}

void LCDText_WriteChar(char ch){
	if( ch=='\n' || (gcol==16 && grow==0)){
		LCDText_Locate(1, 0);
	}
	if(ch!='\n' && grow<16){
		LCDText_sendData8(true, ch);
		gcol++;
		WAIT_ChronoUs(39);
	}
}

void LCDText_Locate(int row, int column){
	if((row == 0 || row==1) && column<16 ){
		LCDText_sendData8(false, 0x80 | (0x40*row) | (column & 0x3f));
		WAIT_ChronoUs(39);
		grow=row;
		gcol=column;
	}
}

void LCDText_WriteString(char* str){
	if(str!=NULL){
		for(int i=0; str[i]!=0; i++){
			LCDText_WriteChar(str[i]);
			WAIT_ChronoUs(100);
		}


	}
}

void LCDText_Clear(){
	LCDText_sendData8(false, 1);
	WAIT_Milliseconds(4);
	grow=0;
	gcol=0;
}

void LCDText_Printf(char* fmt, ...){
	va_list args;
	va_start(args, fmt);
	char str[33];
	vsnprintf(str, 33, fmt, args);
	va_end(args);
	LCDText_WriteString(str);
}

void LCDText_CreateChar(unsigned char location, unsigned char charmap[]){
	location&=7;
	LCDText_sendData8(false, 0x40 | location<<3);
	WAIT_ChronoUs(39);
	for(int i=0; i<8; i++){
		LCDText_sendData8(true, charmap[i]&0x1f);
		WAIT_ChronoUs(39);
	}
	LCDText_Locate(grow, gcol); //retorna ao endereço de ddram onde estava
}

void LCDText_SetCursor(bool state){
	if(state) 	LCDText_sendData8(false, 0b1110);
	else 		LCDText_sendData8(false, 0b1100);
	WAIT_ChronoUs(39);
}
