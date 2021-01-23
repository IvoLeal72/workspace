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
#include "gameUtils.h"
#define MAX_MENU_ID 1

int buttonHandler(){
	int buttons=BUTTON_GetButtonsEvents();
	if(BUTTON_PRESSED_EVENT(buttons, 1)) return 1; //se o botao premido for S devolve ja o resultado
	if(BUTTON_PRESSED_EVENT(buttons, 2)){ //se o botao for R espera para decidir devido a combinação de manutenção
		while(1){
			if(BUTTON_RELEASED_EVENT(buttons, 2)) return 2;
			if(BUTTON_PRESSED_EVENT(buttons, 3)){ //L e R premidos, esperar 2 segundos
				WAIT_Milliseconds(2000);
				if(BUTTON_GetButtonsEvents()==0) return 4; //retorna combinação
			}
			buttons=BUTTON_GetButtonsEvents();
		}
	}
	if(BUTTON_PRESSED_EVENT(buttons, 3)){ //se o botao for L espera para decidir devido a combinação de manutenção
		while(1){
			if(BUTTON_RELEASED_EVENT(buttons, 3)) return 3;
			if(BUTTON_PRESSED_EVENT(buttons, 2)){ //L e R premidos, esperar 2 segundos
				WAIT_Milliseconds(2000);
				if(BUTTON_GetButtonsEvents()==0) return 4; //retorna combinação
			}
			buttons=BUTTON_GetButtonsEvents();
		}
	}
	return 0;
}

void startMenu(){
	LCDText_Locate(0,0);
	LCDText_WriteString("   CAR RUNNER\nPress S to start");
}

void clockMenu(){
	LCDText_Locate(0,0);
	char str[33];
	struct tm dateTime;
	RTC_GetValue(&dateTime);
	strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", &dateTime);
	LCDText_WriteString(str);
}

void menus(int id){
	if(id==0) return startMenu();
	if(id==1) return clockMenu();
	//return scoreMenu(id-2);
}

void maintenanceMode(){
	LCDText_Clear();
	LCDText_WriteString("MAINTENANCE");
	while(1);
}

int main(void) {
	Game_Init();
	int menuId=0;
	while(1){
		menus(menuId);
		switch(buttonHandler()){
			//case 1: runGame(); break;
			case 2: menuId=(menuId+1)%(MAX_MENU_ID+1); break;
			case 3: menuId=(menuId-1)%(MAX_MENU_ID+1); break;
			case 4: maintenanceMode(); break;
		}
		WAIT_Milliseconds(200);
	}
	return 0;
}
