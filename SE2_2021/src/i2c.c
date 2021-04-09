/*
 * i2c.c
 *
 *  Created on: 27/03/2021
 *      Author: ivogc
 */

#include "i2c.h"

#ifdef __USE_CMSIS
	#include "LPC17xx.h"
#endif

#define CTRL_CTT 3

#define SEND 0
#define RECEIVE 1

struct _controller{
	const LPC_I2C_TypeDef* ctrl;
	char state;
	int addr;
	char dir;
	char* data;
	size_t dataIdx;
	size_t dataSize;
};

static struct _controller ctrl_arr[CTRL_CTT]={{LPC_I2C0, IDLE}, {LPC_I2C1, IDLE}, {LPC_I2C2, IDLE}};

static void I2Cn_IRQHandler(char n){

}

void I2C0_IRQHandler(void){
	I2Cn_IRQHandler(0);
}

void I2C1_IRQHandler(void){
	I2Cn_IRQHandler(1);
}

void I2C2_IRQHandler(void){
	I2Cn_IRQHandler(2);
}

bool I2C_Start(int id, int address, char* data, size_t data_size, bool receive){
	if(id>=CTRL_CTT) return false;
}

char I2C_Status(int id){
	if(id>=CTRL_CTT) return -1;
	return ctrl_arr[id].state;
}

bool I2C_Stop(int id){
	return false;
}
