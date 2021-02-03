/*
 * lcd.h
 *
 *  Created on: 17/11/2020
 *      Author: ivo
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>

/** @defgroup LCDText LCDText
 * This package provides the core api for the LCD 16x2 text display.
 * @{
 */

/** @defgroup LCDText_Public_FUnctions LCDText Public Functions
 * @{
 */

/**
 * @brief	Initialises the LCD api and executes the configurations of the LCD external display.
 * @note	This functions must be called prior to any other LCDText function, as the LCDText functions use
 * 			some WAIT functions, WAIT_Init must be called prior to LCDText_Init and any other LCDText function.
 * 			This functions use the P0[3], P0[16] and P0[23..26] resources.
 */
void LCDText_Init();

/**
 * @brief	Writes a character to display.
 * @param	ch: code of character to be written
 */
void LCDText_WriteChar(char ch);

/**
 * @brief	Sets the location for the next character to be written.
 * @param	row: value of the row of the display, in this case 0 for the top row, and 1 for the bottom row.
 * @param	column: value of the column of the display, 0 means the leftmost position
 */
void LCDText_Locate(int row, int column);

/**
 * @brief	Writes a string to the display.
 * @param	str: a null-terminated string to be written.
 */
void LCDText_WriteString(char* str);

/**
 * @brief	Clears the display.
 * @note	The action of clearing resets the display row and columns to (0,0).
 */
void LCDText_Clear();

/**
 * @brief	An implementation of the standard printf function that prints on the display.
 */
void LCDText_Printf(char* fmt, ...);

/**
 * @brief	Creates a custom character on the display.
 * @param	location: a value from 0 to 7 meaning the code of the custom character to edit.
 * @param	charmap: an array of bytes where the five less significant bits mean if the corresponding pixel is lit or not, this array must have 8 bytes.
 */
void LCDText_CreateChar(unsigned char location, unsigned char charmap[]);

/**
 * @brief	Enables or disables the cursor on the display.
 * @param	state: true to enable cursor, false to disable
 * @note	The cursor is disabled by default when the LCDText_Init functions is run.
 */
void LCDText_SetCursor(bool state);

/**
 * @}
 */


/**
 * @}
 */

#endif /* LCD_H_ */
