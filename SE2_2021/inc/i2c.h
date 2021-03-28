/*
 * I2C.h
 *
 *  Created on: 27/03/2021
 *      Author: ivogc
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdbool.h>
#include "wait.h"

#define IDLE 0
#define BUSY 1
#define DONE 2

void I2C_Init(char id);
bool I2C_Start(char id, char* data, size_t data_size, bool receive);
char I2C_Status(char id);
bool I2C_Stop(char id);

#endif /* I2C_H_ */
