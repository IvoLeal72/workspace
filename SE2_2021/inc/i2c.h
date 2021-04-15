/*
 * I2C.h
 *
 *  Created on: 27/03/2021
 *      Author: ivogc
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "wait.h"

#define IDLE 0
#define BUSY 1
#define DONE 2
#define ERROR 3

void I2C_Init(int id, char options);
bool I2C_Start(int id, char address, char* data, size_t data_size, bool receive, unsigned int frequency, unsigned int duty_cycle);
char I2C_Status(int id);
bool I2C_Stop(int id);

#endif /* I2C_H_ */
