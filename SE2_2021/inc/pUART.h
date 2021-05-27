/*
 * UART.h
 *
 *  Created on: 05/05/2021
 *      Author: ivo
 */

#ifndef pUART_H_
#define pUART_H_

#include <stdbool.h>

bool pUART_Initialize(int id, int options, unsigned int baud);
bool pUART_IsChar(int id);
unsigned char pUART_ReadChar(int id);
void pUART_ReadString(int id, char* str, size_t strSize);
void pUART_ReadBuffer(int id, char* buff, size_t buffSize);
bool pUART_GetChar(int id, unsigned char *ch);
void pUART_WriteChar(int id, unsigned char ch);
void pUART_WriteString(int id, char *str);
void pUART_WriteBuffer(int id, unsigned char *buffer, int len);
void pUART_Printf(int id, char *format, ...);

#endif /* pUART_H_ */
