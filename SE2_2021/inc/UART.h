/*
 * UART.h
 *
 *  Created on: 05/05/2021
 *      Author: ivo
 */

#ifndef UART_H_
#define UART_H_

#include <stdbool.h>

bool UART_Initialize(unsigned int baud);
bool UART_IsChar(void);
unsigned char UART_ReadChar(void);
bool UART_GetChar(unsigned char *ch);
void UART_WriteChar(unsigned char ch);
void UART_WriteString(char *str);
void UART_WriteBuffer(unsigned char *buffer, int len);
void UART_Printf(char *format, ...);

#endif /* UART_H_ */
