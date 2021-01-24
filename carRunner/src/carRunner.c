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
#include <string.h>
#include <stdlib.h>
#include "gameUtils.h"
#define MAX_MENU_ID 6
#define FLASH_SCORE_ADDR 0x78000
#define FLASH_SCORE_SECTOR 29

struct score{
	char name[4];
	unsigned int score;
	time_t scoreTime;
};

struct score scores[10]={0};
char scoreOwner[4]="AAA";

void storeScores(){
	unsigned int res=1;
	while(res){
		while(FLASH_EraseSectors(FLASH_SCORE_SECTOR,FLASH_SCORE_SECTOR));
		res=FLASH_WriteData(FLASH_SCORE_ADDR, scores, sizeof(struct score)*10);
		res|=FLASH_VerifyData(FLASH_SCORE_ADDR, scores, sizeof(struct score)*10);
	}
}

void loadScores(){
	memcpy(scores, FLASH_SCORE_ADDR, sizeof(struct score)*10);
}

int buttonHandler(){
	int buttons=BUTTON_GetButtonsEvents();
	if(BUTTON_PRESSED_EVENT(buttons, 1)){
		while(1){
			if(BUTTON_RELEASED_EVENT(buttons, 1)) return 1;
			buttons=BUTTON_GetButtonsEvents();
		}
	}
	if(BUTTON_PRESSED_EVENT(buttons, 2)){
		while(1){
			if(BUTTON_RELEASED_EVENT(buttons, 2)) return 2;
			if(BUTTON_PRESSED_EVENT(buttons, 3)){
				uint32_t start=WAIT_GetElapsedMillis(0);
				while(1){
					if(BUTTON_RELEASED_EVENT(buttons, 2) || BUTTON_RELEASED_EVENT(buttons, 3)) return 0;
					if(WAIT_GetElapsedMillis(start)>=2000) return 4;
					buttons=BUTTON_GetButtonsEvents();
				}
			}
			buttons=BUTTON_GetButtonsEvents();
		}
	}
	if(BUTTON_PRESSED_EVENT(buttons, 3)){
		while(1){
			if(BUTTON_RELEASED_EVENT(buttons, 3)) return 3;
			if(BUTTON_PRESSED_EVENT(buttons, 2)){
				uint32_t start=WAIT_GetElapsedMillis(0);
				while(1){
					if(BUTTON_RELEASED_EVENT(buttons, 2) || BUTTON_RELEASED_EVENT(buttons, 3)) return 0;
					if(WAIT_GetElapsedMillis(start)>=2000) return 4;
					buttons=BUTTON_GetButtonsEvents();
				}
			}
			buttons=BUTTON_GetButtonsEvents();
		}
	}
	return 0;
}

void startMenu(){
	LCDText_Locate(0,0);
	LCDText_WriteString("   CAR RUNNER   Press S to start");
}

void clockMenu(){
	LCDText_Locate(0,0);
	char str[33];
	struct tm dateTime;
	RTC_GetValue(&dateTime);
	strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", &dateTime);
	LCDText_WriteString(str);
}

void scoreMenu(int pos){
	LCDText_Locate(0,0);
	for(int i=2*pos; i<2*pos+2; i++) LCDText_Printf("%2d: %3s - %6d", i+1, scores[i].name, scores[i].score);
}

void menus(int id){
	if(id==0) return startMenu();
	if(id==1) return clockMenu();
	return scoreMenu(id-2);
}

void maintenanceClockMenu(){
	LCDText_Locate(0,0);
	char str[33];
	struct tm dateTime;
	RTC_GetValue(&dateTime);
	strftime(str, 33, "SET TIME   %d/%m%Y    %H:%M:%S", &dateTime);
	LCDText_WriteString(str);
}

void clockModify(){
	LCDText_Locate(0,0);
	char str[33];
	time_t t;
	struct tm ts;
	struct tm* dateTime=&ts;
	RTC_GetValue(dateTime);
	dateTime->tm_sec=0;
	strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
	LCDText_WriteString(str);
	LCDText_SetCursor(true);

	LCDText_Locate(0,4);
	int buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_mday++;
		if(buttons==3) dateTime->tm_mday--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			LCDText_Locate(0,0);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			LCDText_WriteString(str);
			LCDText_Locate(0,4);
		}
		buttons=buttonHandler();
	}
	LCDText_Locate(0,7);
	WAIT_Milliseconds(200);
	buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_mon++;
		if(buttons==3) dateTime->tm_mon--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			LCDText_Locate(0,0);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			LCDText_WriteString(str);
			LCDText_Locate(0,7);
		}
		buttons=buttonHandler();
	}
	LCDText_Locate(0,12);
	WAIT_Milliseconds(200);
	buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_year++;
		if(buttons==3) dateTime->tm_year--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			LCDText_Locate(0,0);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			LCDText_WriteString(str);
			LCDText_Locate(0,12);
		}
		buttons=buttonHandler();
	}
	LCDText_Locate(1,5);
	WAIT_Milliseconds(200);
	buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_hour++;
		if(buttons==3) dateTime->tm_hour--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			LCDText_Locate(0,0);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			LCDText_WriteString(str);
			LCDText_Locate(1,5);
		}
		buttons=buttonHandler();
	}
	LCDText_Locate(1,8);
	WAIT_Milliseconds(200);
	buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_min++;
		if(buttons==3) dateTime->tm_min--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			LCDText_Locate(0,0);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			LCDText_WriteString(str);
			LCDText_Locate(1,8);
		}
		buttons=buttonHandler();
	}
	LCDText_SetCursor(false);
	RTC_SetValue(dateTime);
}

void clearScores(){
	memset(scores, 0, sizeof(struct score)*10);
	storeScores();
	LCDText_Clear();
	LCDText_WriteString("    CLEARED!");
	WAIT_Milliseconds(1000);
}

void maintenanceMenus(int id){
	switch(id){
		case 0: maintenanceClockMenu(); break;
		case 1: LCDText_Locate(0,0); LCDText_WriteString("   CLEAR ALL      SAVED SCORES  "); break;
		case 2: LCDText_Locate(0,0); LCDText_WriteString("      EXIT                      "); break;
	}
}

void maintenanceMode(){
	LCDText_Clear();
	int maintenanceMenuId=0;
	while(1){
		maintenanceMenus(maintenanceMenuId);
		switch(buttonHandler()){
			case 1: switch(maintenanceMenuId){
				case 0: clockModify(); break;
				case 1: clearScores(); break;
				case 2: return;
			} break;
			case 2: maintenanceMenuId++; if(maintenanceMenuId>2) maintenanceMenuId=0; break;
			case 3: maintenanceMenuId--; if(maintenanceMenuId<0) maintenanceMenuId=2; break;
		}
	}
}

void showScore(unsigned int score){
	LCDText_Clear();
	LCDText_Printf("Score: %d\n", score);
	LCDText_SetCursor(true);
	int i=6;
	while(i<9){
		LCDText_Locate(1,6);
		LCDText_WriteString(scoreOwner);
		LCDText_Locate(1,i);
		switch(buttonHandler()){
			case 1: i++; break;
			case 2: scoreOwner[i-6]++; break;
			case 3: scoreOwner[i-6]--; break;
		}
		WAIT_Milliseconds(100);
	}
}

int scoreCmp(const void* e1, const void* e2){
	const struct score *s1=e1;
	const struct score *s2=e2;
	if(s1->score==s2->score){
		return s2->scoreTime-s1->scoreTime;
	}
	return s2->score-s1->score;
}

void saveScore(unsigned int score){
	if(score<scores[9].score) return;
	scores[9].score=score;
	strcpy(scores[9].name, scoreOwner);
	scores[9].scoreTime=RTC_GetSeconds();
	qsort(scores, 10, sizeof(struct score), scoreCmp);
	storeScores();
}

void runGame(){
	Game_Begin();
	uint32_t gameTick=1000;
	uint32_t startTick=WAIT_GetElapsedMillis(0);
	uint32_t tickUpdate=100;
	uint32_t startTickUpdate=WAIT_GetElapsedMillis(0);
	while(Game_Update()){
		if(WAIT_GetElapsedMillis(startTick)>gameTick){
			startTick=WAIT_GetElapsedMillis(0);
			Game_NextTick();
		}
		if(WAIT_GetElapsedMillis(startTickUpdate)>tickUpdate){
			startTickUpdate=WAIT_GetElapsedMillis(0);
			gameTick--;
		}
	}
	unsigned int score=Game_GetScore();
	showScore(score);
	saveScore(score);
}

int main(void) {
	Game_Init();
	loadScores();
	int menuId=0;
	while(1){
		menus(menuId);
		switch(buttonHandler()){
			case 1: runGame(); break;
			case 2: menuId++; if(menuId>MAX_MENU_ID) menuId=0; break;
			case 3: menuId--; if(menuId<0) menuId=MAX_MENU_ID; break;
			case 4: maintenanceMode(); break;
		}
		WAIT_Milliseconds(100);
	}
	return 0;
}
