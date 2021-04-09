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

void func(int n){
	int x[n];
	for(int i=0; i<n; i++) x[i]=i+1;
}

int main(void) {
	return 1;
}
