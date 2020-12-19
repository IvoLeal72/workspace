/*
 * lcd.h
 *
 *  Created on: 17/11/2020
 *      Author: ivo
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>

void LCDText_Init();

void LCDText_WriteChar(char ch);

void LCDText_Locate(int row, int column);

void LCDText_WriteString(char* str);

void LCDText_Clear();

void LCDText_Printf(char* fmt, ...);

void LCDText_CreateChar(unsigned char location, unsigned char charmap[]);


#endif /* LCD_H_ */
