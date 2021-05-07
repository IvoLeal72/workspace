/*
 * flash.h
 *
 *  Created on: 23/01/2021
 *      Author: ivogc
 */

#ifndef FLASH_H_
#define FLASH_H_

/** @defgroup FLASH FLASH
 * This package provides the core capabilities for using the FLASH ROM as a data storage device.
 * @{
 */

/** @defgroup FLASH_Public_Functions FLASH Public Functions
 * @{
 */

/**
 * @brief	Erases the specified sectors of the flash.
 * @param	startSector: the first sector to erase.
 * @param	endSector: the last sector to erase.
 */
unsigned int FLASH_EraseSectors(unsigned int startSector, unsigned int endSector);

/**
 * @brief	Writes the data in ram pointed by srcAdress to the ROM address pointed to by dstAdress
 * @param	dstAdress: ROM address to write the data to.
 * @param	srcAdress: memory address where the data is saved.
 * @param	size: amount of bytes to move form the source to the flash ROM.
 * @return	0 if successful, error code as specified by the iap api.
 */
unsigned int FLASH_WriteData(void *dstAdress, void *srcAdress, unsigned int size);

/**
 * @brief	Compares the data pointed by the pointers.
 * @param	dstAdress: pointer to one of the data locations.
 * @param	srcAdress: pointer to the other data location.
 * @param	size: amount of bytes to compare.
 * @return	0 if data is equal, error code as specified by the iap api.
 */
unsigned int FLASH_VerifyData(void *dstAdress, void *srcAdress, unsigned int size);

/**
 * @}
 */

/**
 * @}
 */
#endif /* FLASH_H_ */
