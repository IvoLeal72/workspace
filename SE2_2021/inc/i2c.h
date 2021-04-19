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

#define STOPPED 0
#define IDLE 1
#define BUSY 2
#define DONE 3
#define DONE_STP 4
#define ERROR 5

void I2C_Init(int id, char options);
bool I2C_ConfigTransfer(int id, unsigned int frequency, unsigned int duty_cycle);
bool I2C_Start(int id, char address, char* data, size_t data_size, bool receive, bool auto_stop);
char I2C_Status(int id);
bool I2C_Stop(int id);

#endif /* I2C_H_ */
