/*
 * UART.h
 *
 *  Created on: 05/05/2021
 *      Author: ivo
 */

#ifndef UART_H_
#define UART_H_

#include <stdbool.h>

bool UART_Initailize(int id, unsigned int baud);
uint32_t UART_ReadBuffer(int id, char* buffer, size_t len);
uint32_t UART_WriteBuffer(int id, char* buffer, size_t len);

#endif /* UART_H_ */
