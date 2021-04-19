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
#include "lcd.h"
#include "wait.h"
#include "i2c.h"
#include "EEPROM.h"
#define TEST_SIZE 10

int main(void) {
	WAIT_Init();
	LCDText_Init();
	I2C_Init(1, 1);
	char arr[TEST_SIZE];
	for(int i=0; i<TEST_SIZE; i++) arr[i]=i;
	LCDText_Printf("%d ", EEPROM_Write(0, arr, TEST_SIZE));
	//for(int i=0; i<TEST_SIZE; i++) LCDText_Printf("%d ", arr[i]);
	//printf("done\n");
	while(true);
}
