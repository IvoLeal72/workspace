/*
 * RTOS_LCD.h
 *
 *  Created on: 25/06/2021
 *      Author: ivogc
 */

#ifndef RTOS_LCD_H_
#define RTOS_LCD_H_

#include <stdbool.h>

bool RTOS_LCD_Init();

void RTOS_LCD_CreateChar(unsigned char location, unsigned char charmap[8]);

void RTOS_LCD_SetCursor(bool state);

void RTOS_LCD_LocatedChar(int row, int column, char ch, bool clear);

void RTOS_LCD_LocatedPrint(int row, int column, char* str, bool clear);

void RTOS_LCD_Printf(int row, int column, bool clear, char *fmt, ...);


#endif /* RTOS_LCD_H_ */
