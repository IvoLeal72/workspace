/*
 * UART.h
 *
 *  Created on: 05/05/2021
 *      Author: ivo
 */

#ifndef UART_H_
#define UART_H_

#include <stdbool.h>

bool UART_Initialize(int id, int options, unsigned int baud);
uint32_t UART_WriteBuffer(int id, uint8_t *buffer, uint32_t len);
uint32_t UART_ReadBuffer(int id, uint8_t *buffer, uint32_t len);
void UART_Printf(int id, char *format, ...);

#endif /* UART_H_ */
