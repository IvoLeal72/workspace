/*
===============================================================================
 Name        : testProject.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include "wait.h"
#include "lcd.h"
#include "button.h"
#include "rtc.h"

void loadGameSprites(){
	unsigned char E_T[] = {
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x0E,
		0x0E,
		0x04
	};
	unsigned char T_E[] = {
		0x00,
		0x0E,
		0x0E,
		0x04,
		0x00,
		0x00,
		0x00,
		0x00
	};
	unsigned char T_T[] = {
		0x00,
		0x0E,
		0x0E,
		0x04,
		0x00,
		0x0E,
		0x0E,
		0x04
	};
	unsigned char E_C[] = {
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x0E,
		0x1F,
		0x0A
	};
	unsigned char C_E[] = {
		0x00,
		0x0E,
		0x1F,
		0x0A,
		0x00,
		0x00,
		0x00,
		0x00
	};
	unsigned char T_C[] = {
		0x00,
		0x0E,
		0x0E,
		0x04,
		0x00,
		0x0E,
		0x1F,
		0x0A
	};
	unsigned char C_T[] = {
		0x00,
		0x0E,
		0x1F,
		0x0A,
		0x00,
		0x0E,
		0x0E,
		0x04
	};
	LCDText_CreateChar(1, E_T);
	LCDText_CreateChar(2, T_E);
	LCDText_CreateChar(3, T_T);
	LCDText_CreateChar(4, E_C);
	LCDText_CreateChar(5, C_E);
	LCDText_CreateChar(6, T_C);
	LCDText_CreateChar(7, C_T);
}

int main(void) {
	char buffer[80];
	WAIT_Init();
	LCDText_Init();
	time_t t=949885323;
	struct tm *dateTime=localtime(&t);
	strftime(buffer,80,"%d/%m/%Y %X", dateTime);
	LCDText_WriteString(buffer);
	return 0;
}
