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

#include "lcd.h"
#include "wait.h"
#include "i2c.h"
#include "EEPROM.h"

int main(void) {
	WAIT_Init();
	LCDText_Init();
	I2C_Init(1, 1);
	char arr[10];
	for(int i=0; i<10; i++){
		arr[i]=i;
	}
	LCDText_Printf("%d\n", EEPROM_Write(0, arr, 10));
	printf("done\n");
	while(true);
}
