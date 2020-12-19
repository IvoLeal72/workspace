/**
* @file		button.h
* @brief	Contains the buttons API.
* @version	1.0
* @date		3 Nov 2020
* @author	ILeal
*
* Copyright(C) 2015-2020, ILeal
* All rights reserved.
*
***********************************************************************
* Software that is described here in is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
**********************************************************************/

#ifndef BUTTON_H_
#define BUTTON_H_

/** @defgroup BUTTON BUTTON
 * This package provides the core capabilities for wait functions.
 * @{
 */

/** @defgroup BUTTON_Public_Macros BUTTON Public Macros
 * @{
 */

/**
 * @brief 	Macro to check if a specific button is pressed.
 * @param	data: int returned by BUTTON_Hit or BUTTON_Read.
 * @param	button: int between 1 and 3 meaning which button to check.
 */
#define BUTTON_PRESSED(data, button) ((data)&(1<<(button-1)))

/**
 * @brief 	Macro to check if a specific button got pressed.
 * @param	data: int returned by BUTTON_GetButtonEvents.
 * @param	button: int between 1 and 3 meaning which button to check.
 */
#define BUTTON_PRESSED_EVENT(data, button) ((data)&(1<<((button-1)*2)))

/**
 * @brief 	Macro to check if a specific button got released.
 * @param	data: int returned by BUTTON_GetButtonEvents.
 * @param	button: int between 1 and 3 meaning which button to check.
 */
#define BUTTON_RELEASED_EVENT(data, button) ((data)&(0b10<<((button-1)*2)))

/**
 * @}
 */


/** @defgroup BUTTON_Public_Functions BUTTON Public Functions
 * @{
 */

/**
 * @brief 	Initialises the button api to be able to read them.
 * @note  	This function must be called prior to any other BUTTON functions,
 * 			and use the pins P0[0..2] resources.
 */
void BUTTON_Init(void);

/**
 * @brief	Reads the button values (non-blocking).
 * @return	Returns a 3-bits bitmap, in which every bit is a button,
 * 			and 1 means the button is pressed,
 * 			0 meaning the button is not pressed.
 */
int BUTTON_Hit(void);


/**
 * @brief	Reads the button values (blocking, until at least a button is pressed).
 * @return	Returns a 3-bits bitmap, in which every bit is a button,
 * 			and 1 means the button is pressed,
 * 			0 meaning the button is not pressed.
 */
int BUTTON_Read(void);

/**
 * @brief	Reads the button values (blocking, until at least a button is pressed).
 * @return	Returns a 3-bits bitmap, in which every bit is a button,
 * 			and 1 means the button is pressed,
 * 			0 meaning the button is not pressed.
 */
int BUTTON_GetButtonsEvents (void);

/**
 * @}
 */


/**
 * @}
 */

#endif /* BUTTON_H_ */
