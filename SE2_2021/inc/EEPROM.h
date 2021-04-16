/*
 * EEPROM.h
 *
 *  Created on: 15/04/2021
 *      Author: ivogc
 */

#ifndef EEPROM_H_
#define EEPROM_H_

int EEPROM_Write(short addr, char* data, size_t data_size);

int EEPROM_Read(short addr, char* data, size_t data_size);

#endif /* EEPROM_H_ */
