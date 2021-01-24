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

void Game_Init();

void Game_Begin();

bool Game_Update();

void Game_NextTick();

unsigned int Game_GetScore();

#endif /* GAMEUTILS_H_ */
