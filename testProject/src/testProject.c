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

int main(void) {
	WAIT_Init();
	LCDText_Init();
	RTC_Init(949967820);
	struct tm dateTime;
	char str[20];
	while(1){
		RTC_GetValue(&dateTime);
		strftime(str, 20, "%d/%m/%Y\n%H:%M:%S", &dateTime);
		LCDText_Locate(0,0);
		LCDText_WriteString(str);
		WAIT_Milliseconds(10);
	}
	return 0;
}
