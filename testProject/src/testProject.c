/*
===============================================================================
 Name        : testProject.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC17xx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "led.h"

int main(void) {
	LED_Init(0b001);
	return 0;
}
