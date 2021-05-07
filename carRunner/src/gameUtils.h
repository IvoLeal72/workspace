/*
 * gameUtils.h
 *
 *  Created on: 18/01/2021
 *      Author: ivogc
 */

#ifndef GAMEUTILS_H_
#define GAMEUTILS_H_

#include <stdbool.h>
#include "ADXL345.h"
#include "wait.h"
#include "lcd.h"
#include "led.h"
#include "button.h"
#include "rtc.h"

/** @defgroup Game Game
 * This package provides the core api for game control.
 * @{
 */

/** @defgroup Game_Public_FUnctions Game Public Functions
 * @{
 */

/**
 * @brief	Prepares everything needed for the game, including starting all the used apis.
 */
void Game_Init();

/**
 * @brief	Clears the game map and score, and prepares everything for a new game.
 */
void Game_Begin();

/**
 * @brief	Core Game function, must be run in a cyclic wait, as quickly as possible, updates the display, reads player input, and checks if the player lost.
 * @return	Returns if the player is still alive, meant to be use as parameter of a while loop.
 */
bool Game_Update();

/**
 * @brief	Creates the next obstacle line and moves the obstacles left, Game_Update must be called after to check for car crash.
 */
void Game_NextTick();

/**
 * @brief	Returns the game actual score.
 */
unsigned int Game_GetScore();


/**
 * @}
 */

/**
 * @}
 */
#endif /* GAMEUTILS_H_ */
