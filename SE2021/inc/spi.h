/*
 * spi.h
 *
 *  Created on: 09/01/2021
 *      Author: ivo
 */

#ifndef SPI_H_
#define SPI_H_

void SPI_Init(void);

void SPI_ConfigTransfer(int frequency, int bitData, int mode);

#endif /* SPI_H_ */
