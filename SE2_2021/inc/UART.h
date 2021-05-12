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
bool UART_IsChar(int id);
unsigned char UART_ReadChar(int id);
bool UART_GetChar(int id, unsigned char *ch);
void UART_WriteChar(int id, unsigned char ch);
void UART_WriteString(int id, char *str);
void UART_WriteBuffer(int id, unsigned char *buffer, int len);
void UART_Printf(int id, char *format, ...);

#endif /* UART_H_ */
