/*
===============================================================================
 Name        : lab_0.c
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
#include "led.h"
#define DELAY 500/2

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {

    WAIT_Init();
    LED_Init(true);
    while(1){
    	WAIT_Milliseconds(DELAY);
    	LED_Off();
    	WAIT_Milliseconds(DELAY);
    	LED_On();
    }
    return 0 ;
}
