/*
 * Utils.h
 *
 *  Created on: 11/04/2021
 *      Author: ivogc
 */

#ifndef UTILS_H_
#define UTILS_H_

#define PULLUP 0
#define REPEAT 1
#define NEITHER 2
#define PULLDOWN 3

#define NORMAL 0
#define OPEN 1

void set_PCLK(char divider, char reg, char lsb);

void set_PINSEL(char mode, char reg, char lsb);

void set_PINMODE(char mode, char reg, char lsb);

void set_PINMODE_OD(char mode, char reg, char lsb);

void pin_Config(char gpio, char pin, char func, char res_mode, char dr_mode);

#endif /* UTILS_H_ */
