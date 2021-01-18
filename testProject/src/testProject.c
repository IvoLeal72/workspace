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
#include "ADXL345.h"

int main(void) {
	ADXL345_Init();
	WAIT_Init();
	LCDText_Init();

	LCDText_Printf("0x%x\n", ADXL345_GetId());
	WAIT_Milliseconds(5000);
	short values[3];
	int res;
	while(1){
		res=ADXL345_GetValues(values);
		LCDText_Locate(0,0);
		LCDText_Printf("0x%02X X=%06d Y=%06d Z=%06d", res, values[0], values[1], values[2]);
		WAIT_Milliseconds(100);
	}
	return 0;
}
