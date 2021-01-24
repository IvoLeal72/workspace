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
#include "flash.h"

int main(void) {
	int* ptr=0x78000;
	for(int i=0; i<64; i++){
		printf("%d\n", ptr[i]);
	}
	return 0;
}
