/*
 * ADXL345.h
 *
 *  Created on: 15/01/2021
 *      Author: ivogc
 */

#ifndef ADXL345_H_
#define ADXL345_H_
#include "spi.h"

/** @defgroup ADXL345 ADXL345
 * This package provides the core capabilities for using the ADXL345 peripheral.
 * @{
 */

/** @defgroup ADXL345_Public_Macros ADXL345 Public Macros
 * @{
 */

/**
 * @brief	Initialises the ADXL345 api.
 * @note	This functions must be called to any other ADXL345 functions,
 * 			as it uses the SPI api, SPI_Init must be called prior to this functions.
 * 			Uses the pin P0[21].
 */
void ADXL345_Init();

/**
 * @brief	Reads the ADXL345 DeviceId, used mostly as a test.
 * @return	Must return 0xE5.
 */
unsigned char ADXL345_GetId();

/**
 * @brief	Gets X,Y, Z axis data from the ADXL345 and saves those values to the array.
 * @param	values: an array in which to save X, Y and Z data, in this order.
 * @return	SPI transfer error code(0 if transfer was successful) accordingly to the SPI controller status register.
 * @note	the values read are only 10 bits in value, but sign expanded to 16.
 */
int ADXL345_GetValues(short values[3]);

/**
 * @brief	Reads and returns the X axis value from the ADXL345.
 * @return 	X axis value.
 * @note	the value read is only 10 bits in value, but sign extended to 16.
 */
short ADXL345_GetX();

/**
 * @brief	Reads and returns the Y axis value from the ADXL345.
 * @return 	Y axis value.
 * @note	the value read is only 10 bits in value, but sign extended to 16.
 */
short ADXL345_GetY();

/**
 * @brief	Reads and returns the Z axis value from the ADXL345.
 * @return 	Z axis value.
 * @note	the value read is only 10 bits in value, but sign extended to 16.
 */
short ADXL345_GetZ();

/**
 * @}
 */

/**
 * @}
 */
#endif /* ADXL345_H_ */
