/*
 * ADXL345.h
 *
 *  Created on: 15/01/2021
 *      Author: ivogc
 */

#ifndef ADXL345_H_
#define ADXL345_H_

#include "spi.h"

void ADXL345_Init();

unsigned char ADXL345_GetId();

int ADXL345_GetValues(short values[3]);

short ADXL345_GetX();

short ADXL345_GetY();

short ADXL345_GetZ();

#endif /* ADXL345_H_ */
