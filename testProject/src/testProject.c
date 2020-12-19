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

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

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
	WAIT_Init();
	LCDText_Init();
	struct tm time;
	char buffer[80];
	RTC_Init(0);
	time.tm_year=100;
	time.tm_mon=1;
	time.tm_mday=7;
	time.tm_yday=38;
	RTC_SetValue(&time);
	RTC_GetValue(&time);

	strftime(buffer,80,"%x - %I:%M%p", &time);
	printf("Formatted date & time : |%s|\n", buffer );
    return 0 ;
}
