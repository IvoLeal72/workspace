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
#include "lcd.h"


// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {

    BUTTON_Init();
    WAIT_Init();
    LCDText_Init();
    int buttons=0;
    // Force the counter to be placed into memory
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        buttons=BUTTON_Hit();
        //printf("%d\n", buttons);
        //LCDText_Clear();
        LCDText_Locate(0,0);
        LCDText_WriteString("Buttons=");
        for(int i=2; i>=0; i--){
        	LCDText_WriteChar((buttons&(1<<i))?'1':'0');
        }
        LCDText_WriteChar('\n');
    }
    return 0 ;
}
