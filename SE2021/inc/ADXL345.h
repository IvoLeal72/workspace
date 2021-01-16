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

#endif /* ADXL345_H_ */
