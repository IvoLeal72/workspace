/*
===============================================================================
 Name        : testProject.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

/** @defgroup carRunner carRunner
 * @{
 */

#include "LPC17xx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gameUtils.h"
#include "flash.h"
#include "FreeRTOS.h"

/** @defgroup carRunner_Public_Definitions_Variables carRunner public definitions and variables
 * @{
 */

#define MAX_MENU_ID 6
#define FLASH_SCORE_ADDR (void*)0x78000
#define FLASH_SCORE_SECTOR 29

struct score{
	char name[4];
	unsigned int score;
	time_t scoreTime;
};

struct score scores[10]={0};
char scoreOwner[4]="AAA";

/**
 * @}
 */

/** @defgroup carRunner_Public_Functions carRunner Public Functions
 * @{
 */

/**
 * @brief 	Saves the actual scores table to the flash memory in the FLASH_SCORE_ADDR
 * @note	Due to passing a buffer with less than 256 bytes some ram values after the scores array will be saved too
 */
void storeScores(){
	unsigned int res=1;
	while(res){
		while(FLASH_EraseSectors(FLASH_SCORE_SECTOR,FLASH_SCORE_SECTOR));
		res=FLASH_WriteData(FLASH_SCORE_ADDR, scores, sizeof(struct score)*10);
		res|=FLASH_VerifyData(FLASH_SCORE_ADDR, scores, sizeof(struct score)*10);
	}
}

/**
 * @brief	Loads the scores tbale from Flash
 * @note	If the Flash was erased, the scores read will be super high values, and the scores should
 * 			erased through the maintenanceMenu (clearScores function)
 */
void loadScores(){
	memcpy(scores, FLASH_SCORE_ADDR, sizeof(struct score)*10);
}

/**
 * @brief	Translated thee buttons to 5 int values, 0-no action, 1-pressed S, 2-pressed R, 3-pressed L,
 * 			4-pressed R+L for more than 2 seconds.
 * @startuml
 * title buttonHandler
 * hide empty description
 *
 * state choice1 <<choice>>
 * state "return 0" as return_0
 * state "return 1" as return_1
 * state "return 2" as return_2
 * state "return 3" as return_3
 * state "return 4" as return_4
 *
 * [*]-->choice1
 *
 * choice1-->return_0 : no button pressed
 *
 * choice1-->S_wait : S pressed
 * S_wait-->return_1 : S released
 *
 * choice1-->R_wait : R pressed
 * R_wait-->return_2 : R released
 * R_wait-->L_R_wait : L pressed
 *
 * choice1-->L_wait : L pressed
 * L_wait-->return_3 : L released
 * L_wait-->L_R_wait : R pressed
 *
 * L_R_wait-->return_0 : L released
 * L_R_wait-->return_0 : R released
 * L_R_wait-->return_4 : after 2 seconds
 * @enduml
 */
int buttonHandler(){
	int buttons=RTOS_Buttons_GetEvents(0);
	if(BUTTON_PRESSED_EVENT(buttons, 1)){
		while(1){
			if(BUTTON_RELEASED_EVENT(buttons, 1)) return 1;
			buttons=RTOS_Buttons_GetEvents(portMAX_DELAY);
		}
	}
	if(BUTTON_PRESSED_EVENT(buttons, 2)){
		while(1){
			if(BUTTON_RELEASED_EVENT(buttons, 2)) return 2;
			if(BUTTON_PRESSED_EVENT(buttons, 3)){
				buttons=RTOS_Buttons_GetEvents(pdMS_TO_TICKS(2000));
				if(BUTTON_RELEASED_EVENT(buttons, 2) || BUTTON_RELEASED_EVENT(buttons, 3)) return 0;
				if(buttons==0) return 4;
			}
			buttons=RTOS_Buttons_GetEvents(portMAX_DELAY);
		}
	}
	if(BUTTON_PRESSED_EVENT(buttons, 3)){
		while(1){
			if(BUTTON_RELEASED_EVENT(buttons, 3)) return 3;
			if(BUTTON_PRESSED_EVENT(buttons, 2)){
				buttons=RTOS_Buttons_GetEvents(pdMS_TO_TICKS(2000));
				if(BUTTON_RELEASED_EVENT(buttons, 2) || BUTTON_RELEASED_EVENT(buttons, 3)) return 0;
				if(buttons==0) return 4;
			}
			buttons=RTOS_Buttons_GetEvents(portMAX_DELAY);
		}
	}
	return 0;
}

/**
 * @brief	Displays the start menu
 */
void startMenu(){
	RTOS_LCD_LocatedPrint(0,0,"   CAR RUNNER   Press S to start", false);
}

/**
 * @brief	Displays the clock on screen
 */
void clockMenu(){
	char str[33];
	struct tm dateTime;
	RTC_GetValue(&dateTime);
	strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", &dateTime);
	RTOS_LCD_LocatedPrint(0, 0, str, false);
}

/**
 * @brief	Displays 2 of the top scores
 * @param	pos: if 0 displays first and second scores, if 1 displays the third and forth,
 * 			if 2 fifth and sixth, and so on
 */
void scoreMenu(int pos){
	for(int i=2*pos; i<2*pos+2; i++) RTOS_LCD_LocatedPrintf(i%2, 0, false, "%2d: %3s - %6d", i+1, scores[i].name, scores[i].score);
}

/**
 * @brief 	Selects the menu to display
 * @param	id: id of the menu to display, 0-displays the start menu, 1-displays the clock,
 * 			other-displays the scores
 */
void menus(int id){
	if(id==0) return startMenu();
	if(id==1) return clockMenu();
	return scoreMenu(id-2);
}

/**
 * @brief	Menu to update the system clock
 * @startuml
 * title clockModify
 * hide empty description
 *
 * state "modify day" as day
 * state "modify month" as mon
 * state "modify year" as year
 * state "modify hour" as hour
 * state "modify minutes" as min
 * state "update RTC" as save
 *
 * [*]-->day
 * day-->day: R button-day++
 * day-->day: L button-day--
 * day-->mon: S button
 *
 * mon-->mon: R button-mon++
 * mon-->mon: L button-mon--
 * mon-->year: S button
 *
 * year-->year: R button-year++
 * year-->year: L button-year--
 * year-->hour: S button
 *
 * hour-->hour: R button-hour++
 * hour-->hour: L button-hour--
 * hour-->min: S button
 *
 * min-->min: R button-min++
 * min-->min: L button-min--
 * min-->save: S button
 *
 * save-->[*]
 * @enduml
 */
void clockModify(){
	char str[33];
	time_t t;
	struct tm ts;
	struct tm* dateTime=&ts;
	RTC_GetValue(dateTime);
	dateTime->tm_sec=0;
	strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
	RTOS_LCD_LocatedPrint(0,0,str, false);
	RTOS_LCD_SetCursor(true);
	WAIT_Milliseconds(200);
	RTOS_LCD_Locate(0,4);
	int buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_mday++;
		if(buttons==3) dateTime->tm_mday--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			RTOS_LCD_LocatedPrint(0,0,str,false);
			RTOS_LCD_Locate(0,4);
		}
		buttons=buttonHandler();
	}
	RTOS_LCD_Locate(0,7);
	WAIT_Milliseconds(200);
	buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_mon++;
		if(buttons==3) dateTime->tm_mon--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			RTOS_LCD_LocatedPrint(0,0,str,false);
			RTOS_LCD_Locate(0,7);
		}
		buttons=buttonHandler();
	}
	RTOS_LCD_Locate(0,12);
	WAIT_Milliseconds(200);
	buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_year++;
		if(buttons==3) dateTime->tm_year--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			RTOS_LCD_LocatedPrint(0,0,str,false);
			RTOS_LCD_Locate(0,12);
		}
		buttons=buttonHandler();
	}
	RTOS_LCD_Locate(1,5);
	WAIT_Milliseconds(200);
	buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_hour++;
		if(buttons==3) dateTime->tm_hour--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			RTOS_LCD_LocatedPrint(0,0,str,false);
			RTOS_LCD_Locate(1,5);
		}
		buttons=buttonHandler();
	}
	RTOS_LCD_Locate(1,8);
	WAIT_Milliseconds(200);
	buttons=buttonHandler();
	while(1){
		if(buttons==1) break;
		if(buttons==2) dateTime->tm_min++;
		if(buttons==3) dateTime->tm_min--;
		if(buttons!=0){
			t=mktime(dateTime);
			dateTime=localtime(&t);
			strftime(str, 33, "   %d/%m/%Y       %H:%M:%S    ", dateTime);
			RTOS_LCD_LocatedPrint(0,0,str,false);
			RTOS_LCD_Locate(1,8);
		}
		buttons=buttonHandler();
	}
	RTOS_LCD_SetCursor(false);
	RTC_SetValue(dateTime);
	WAIT_Milliseconds(200);
}

/**
 * @brief	Clears the score table in RAM and in the flash
 */
void clearScores(){
	memset(scores, 0, sizeof(struct score)*10);
	storeScores();
	RTOS_LCD_LocatedPrint(0,0,"    CLEARED!", true);
	WAIT_Milliseconds(1000);
}

/**
 * @brief	Shows the time (maintenance mode version)
 */
void maintenanceClockMenu(){
	char str[33];
	struct tm dateTime;
	RTC_GetValue(&dateTime);
	strftime(str, 33, "SET TIME   %d/%m%Y    %H:%M:%S", &dateTime);
	RTOS_LCD_LocatedPrint(0,0,str, false);
}

/**
 * @brief	Selects the maintenance menu to display
 * @param	id: id of the menu to display 0-clock, 1-clear scores, 2-exit
 */
void maintenanceMenus(int id){
	switch(id){
		case 0: maintenanceClockMenu(); break;
		case 1: RTOS_LCD_LocatedPrint(0,0,"   CLEAR ALL      SAVED SCORES  ", false); break;
		case 2: RTOS_LCD_LocatedPrint(0,0,"      EXIT                      ", false); break;
	}
}

/**
 * @brief	Maintenance menu handler, cycles through menus with L and R buttons, S to execute menu action
 *
 * @startuml
 * title maintenanceMode
 * hide empty description
 *
 * state "Clock menu" as clk
 * state "Score menu" as score
 * state "Exit menu" as exit
 *
 * [*]-->clk
 *
 * clk-->clockModify: S button
 * clk-right->score: R button
 * clk-right->exit: L button
 * clockModify-->clk
 *
 * score-->clearScores : S button
 * score-right->exit : R button
 * score-left->clk: L button
 * clearScores-->score
 *
 * exit-->[*] : S button
 * exit-->clk : R button
 * exit-->score : L button
 * @enduml
 */
void maintenanceMode(){
	RTOS_LCD_Clear();
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

/**
 * @brief	Shows the score and allow the user to change the score name
 * @startuml
 * title showScore
 * hide empty description
 *
 * state "first character" as 1
 * state "second character" as 2
 * state "third character" as 3
 *
 * [*]-->1
 *
 * 1-->1 : L button-char--
 * 1-->1 : R button-char++
 * 1-->2 : S button
 *
 * 2-->2 : L button-char--
 * 2-->2 : R button-char++
 * 2-->3 : S button
 *
 * 3-->3 : L button-char--
 * 3-->3 : R button-char++
 * 3-->[*] : S button
 * @enduml
 */
void showScore(unsigned int score){
	RTOS_LCD_LocatedPrintf(0,0,true,"Score: %d\n", score);
	RTOS_LCD_SetCursor(true);
	int i=6;
	while(i<9){
		RTOS_LCD_LocatedPrint(1,6,scoreOwner, false);
		RTOS_LCD_Locate(1,i);
		switch(buttonHandler()){
			case 1: i++; break;
			case 2: scoreOwner[i-6]++; break;
			case 3: scoreOwner[i-6]--; break;
		}
		WAIT_Milliseconds(100);
	}
	RTOS_LCD_SetCursor(false);
}

/**
 * @brief	Score compare function to use with qsort
 */
int scoreCmp(const void* e1, const void* e2){
	const struct score *s1=e1;
	const struct score *s2=e2;
	if(s1->score==s2->score){
		return s2->scoreTime-s1->scoreTime;
	}
	return s2->score-s1->score;
}

/**
 * @brief	Updates the score tables in RAm and flash if the score is greater or equal than the
 * 			lowest score in the table
 */
void saveScore(unsigned int score){
	if(score<scores[9].score) return;
	scores[9].score=score;
	strcpy(scores[9].name, scoreOwner);
	scores[9].scoreTime=RTC_GetSeconds();
	qsort(scores, 10, sizeof(struct score), scoreCmp);
	storeScores();
}

/**
 * @brief	Starts, runs and ends the game, and after that calls saveScore().
 * @startuml
 * title runGame
 * hide empty description
 *
 * state "Decrease gameTick by 1" as dec_tick
 * state "Generate obstacles" as next_tick
 * state "Game running" as game
 * state "Save score" as save
 *
 * [*]-->game
 * game-left->dec_tick : after 100 ms
 * dec_tick-right->game
 * game-right->next_tick: after gameTick ms
 * next_tick-left->game
 * game-->save : Player lost
 * save-->[*]
 * @enduml
 */
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

/**
 * @brief	Starts the device drivers, loads the score table from flash and then acts as menu handler
 * 			as described in the UML
 *
 * @startuml
 * title menuHandler
 * hide empty description
 *
 * state "show menu" as menu
 * state "move to previous menu" as dec
 * state "move to next menu" as inc
 *
 * [*]-->menu
 * menu-left->dec : L button
 * dec-right->menu
 * menu-right->inc : R button
 * inc-left->menu
 * menu-->runGame : S button
 * runGame-->menu : Game ended
 * menu-->maintenanceMode : L+R combo
 * maintenanceMode-->menu : exited maintenance mode
 * @enduml
 */
void gameTask(void* params) {
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
}

/**
 * @}
 */

/**
 * @}
 */
