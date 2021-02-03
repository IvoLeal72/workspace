/*
 * spi.h
 *
 *  Created on: 09/01/2021
 *      Author: ivo
 */

#ifndef SPI_H_
#define SPI_H_


/** @defgroup SPI SPI
 * This package provides the core capabilities for spi transmissions.
 * @{
 */

/** @defgroup SPI_Public_Functions SPI Public Functions
 * @{
 */

/**
 * @brief	Initialises the SPI api in master mode.
 * @note	This functions must be called prior to any other SPI functions,
 * 			and use the pins P0[15], and P0[17..18].
 */
void SPI_Init(void);

/**
 * @brief	Sets the transfer configurations.
 * @param	frequency: the sck frequency that should be uses to communicate with the slave peripheral.
 * @param	bitData: the amount of bits per transfer, ranging from 8 to 16.
 * @param	mode: the transfer mode the bit1 meaning CPOL mode and bit0 the CPHA mode.
 * @note	This functions prior to SPI_transfer if it was not yet with the adequate parameters for the slave
 * 			to which the transfer is destined.
 */
void SPI_ConfigTransfer(int frequency, int bitData, int mode);

/**
 * @brief	Executes a transfer between the master and the slave.
 * @param	txBuffer: an array containing the values to send to the slave for which transfer
 * @param	rxBuffer: an array to save the data received from the slave (must be instantiated accordingly)
 * @param	length: the amount of transfers to execute, the txBuffer an rxBuffer must have at least this size.
 * @return	bitmap containing transfer errors (0 if no errors occured), accordingly to the status register of the SPI controller peripheral.
 */
int SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int length);

/**
 * @}
 */


/**
 * @}
 */

#endif /* SPI_H_ */
