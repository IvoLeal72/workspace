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
#include <stdlib.h>
#include "lcd.h"
#include "wait.h"
#include "i2c.h"
#include "EEPROM.h"
#include "button.h"
#define TEST_SIZE 40
#define SUBS_TEST 5

int main(void) {
	WAIT_Init();
	LCDText_Init();
	BUTTON_Init();
	LCDText_WriteString("Waiting");
	BUTTON_Read();
	int seed=WAIT_GetElapsedMillis(0);
	LCDText_Clear();
	srand(seed);
	I2C_Init(1, 1);
	char send[TEST_SIZE];
	char rec[TEST_SIZE];
	for(int i=0; i<TEST_SIZE; i++) send[i]=rand()%256;
	LCDText_Printf("%d ", EEPROM_Write(31, send, TEST_SIZE));
	LCDText_Printf("%d ", EEPROM_Read(31, rec, TEST_SIZE));
	LCDText_Printf("%d\n", memcmp(send, rec, TEST_SIZE));
	char test[SUBS_TEST];
	for(int i=0; i<SUBS_TEST; i++){
		test[i]=rand()%256;
		send[i+3]=test[i];
	}
	LCDText_Printf("%d ", EEPROM_Write(34, test, SUBS_TEST));
	LCDText_Printf("%d ", EEPROM_Read(31, rec, TEST_SIZE));
	LCDText_Printf("%d\n", memcmp(send, rec, TEST_SIZE));

	//printf("done\n");
	while(true);
}
