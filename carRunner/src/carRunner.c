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

int gameTickTime=1000;


int main(void) {
	Game_Init();
	repeat:
	Game_Begin();
	BUTTON_Read();
	uint32_t elapsed=WAIT_GetElapsedMillis(0);
	while(Game_Update()){
		if(WAIT_GetElapsedMillis(elapsed)>=gameTickTime){
			elapsed=WAIT_GetElapsedMillis(0);
			Game_NextTick();
			gameTickTime--;
		}
		WAIT_Milliseconds(5);
	}
	LCDText_Clear();
	LCDText_WriteString("   GAME OVER!   ");
	LCDText_Printf("Score: %d", Game_GetScore());
	BUTTON_Read();
	goto repeat;
	return 0;
}
