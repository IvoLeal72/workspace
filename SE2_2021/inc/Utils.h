/*
 * Utils.h
 *
 *  Created on: 11/04/2021
 *      Author: ivogc
 */

#ifndef UTILS_H_
#define UTILS_H_

void set_PCLK(char divider, char reg, char lsb);

void set_PINSEL(char mode, char reg, char lsb);

void set_PINMODE(char mode, char reg, char lsb);

void set_PINMODE_OD(char mode, char reg, char lsb);

#endif /* UTILS_H_ */
