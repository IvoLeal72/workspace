/*
 * gameUtils.c
 *
 *  Created on: 18/01/2021
 *      Author: ivogc
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include "gameUtils.h"
#include <string.h>
#include <stdlib.h>

char gameMap[16][4];
char displayMap[16][2];
char carLocation=1;
char nextCarLocation=1;
bool makeLine=true;
bool mapChanged=true;
unsigned long score=0;

static void loadGameSprites(){
	unsigned char E_E[8] = {0};
	unsigned char E_T[] = {
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x0E,
		0x0E,
		0x04
	};
	unsigned char T_E[] = {
		0x00,
		0x0E,
		0x0E,
		0x04,
		0x00,
		0x00,
		0x00,
		0x00
	};
	unsigned char T_T[] = {
		0x00,
		0x0E,
		0x0E,
		0x04,
		0x00,
		0x0E,
		0x0E,
		0x04
	};
	unsigned char E_C[] = {
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x0E,
		0x1F,
		0x0A
	};
	unsigned char C_E[] = {
		0x00,
		0x0E,
		0x1F,
		0x0A,
		0x00,
		0x00,
		0x00,
		0x00
	};
	unsigned char T_C[] = {
		0x00,
		0x0E,
		0x0E,
		0x04,
		0x00,
		0x0E,
		0x1F,
		0x0A
	};
	unsigned char C_T[] = {
		0x00,
		0x0E,
		0x1F,
		0x0A,
		0x00,
		0x0E,
		0x0E,
		0x04
	};
	LCDText_CreateChar(0, E_E);
	LCDText_CreateChar(1, E_T);
	LCDText_CreateChar(2, T_E);
	LCDText_CreateChar(3, T_T);
	LCDText_CreateChar(4, E_C);
	LCDText_CreateChar(5, C_E);
	LCDText_CreateChar(6, T_C);
	LCDText_CreateChar(7, C_T);
}

static void Game_InputDetect(){
	short y=ADXL345_GetY();
	if(y>220) nextCarLocation=3;
	else if(y>170 && y<180) nextCarLocation=2;
	else if(y>120 && y<130) nextCarLocation=1;
	else if(y>50 && y<80) nextCarLocation=0;

	if(nextCarLocation>carLocation) carLocation++;
	else if(nextCarLocation<carLocation) carLocation--;
}

bool Game_Update(){
	Game_InputDetect();
	if(gameMap[0][(int)carLocation]==1) return false;
	for(int y=0; y<2; y++){
		for(int x=0; x<16; x++){
			displayMap[x][y]=gameMap[x][2*y+1]|gameMap[x][2*y]<<1; //0 se não houver nenhum obstaculo, 1 se obstaculo em cima, 2 se obsatculo em baixo, e 3 se obstaculo em ambos
		}
	}
	switch(carLocation){
		case 0: if(gameMap[0][1]) displayMap[0][0]=7; else displayMap[0][0]=5; break;
		case 1:	if(gameMap[0][0]) displayMap[0][0]=6; else displayMap[0][0]=4; break;
		case 2:	if(gameMap[0][3]) displayMap[0][1]=7; else displayMap[0][1]=5; break;
		case 3:	if(gameMap[0][2]) displayMap[0][1]=6; else displayMap[0][1]=4; break;
	}

	LCDText_Locate(0,0);
	for(int y=0; y<2; y++){
		for(int x=0; x<16; x++){
			LCDText_WriteChar(displayMap[x][y]);
		}
	}

	return true;
}

void Game_Init(){
	LED_Init(false);
	WAIT_Init();
	LCDText_Init();
	BUTTON_Init();
	ADXL345_Init();
	RTC_Init(949881600);
	loadGameSprites();
}

void Game_Begin(){
	memset(gameMap, 0, 4*16);
	srand(WAIT_GetElapsedMillis(0));
	carLocation=rand()%4;
	score=0;
}

void Game_NextTick(){
	for(int i=0; i<4; i++) score+=gameMap[0][i];
	memmove(gameMap, gameMap+1, 4*15);
	char line=makeLine?rand()%15:0; //cria linha vazia quando makeLine é falso e linha com obstacuilos em posições aleatorias excepto em todas
	makeLine=!makeLine;
	for(int i=0; i<4; i++) gameMap[15][i] = (line>>i)&1;
}

unsigned long Game_GetScore(){ return score; }
