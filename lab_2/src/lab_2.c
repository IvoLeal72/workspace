/*
===============================================================================
 Name        : lab_2.c
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
#include <stdbool.h>
#include "button.h"
#include "wait.h"
#include "led.h"


// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {

    BUTTON_Init();
    WAIT_Init();
    LED_Init(false);

    // Force the counter to be placed into memory
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        int buttons=BUTTON_Read();
        //printf("%d\n", buttons);
        int time=1;
        if(buttons & 1<<2){
        	time=1500;
        }
        else if(buttons & 1<<1){
        	time=1000;
        }
        else if(buttons & 1){
        	time=500;
        }
        LED_On();
        WAIT_Milliseconds(time);
        LED_Off();
    }
    return 0 ;
}
