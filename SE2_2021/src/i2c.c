/*
 * i2c.c
 *
 *  Created on: 27/03/2021
 *      Author: ivogc
 */

#include "i2c.h"

static struct _controller{
	const LPC_I2C_TypeDef* ctrl;
	char state;
	char* data;
	size_t dataIdx;
	size_t data_size;
};

void I2C0_IRQHandler(void){
	I2Cn_IRQHandler(0);
}

void I2C1_IRQHandler(void){
	I2Cn_IRQHandler(1);
}

void I2C2_IRQHandler(void){
	I2Cn_IRQHandler(2);
}

void I2Cn_IRQHandler(char n){

}

static struct _controller ctrl_arr[3]={{LPC_I2C0, IDLE}, {LPC_I2C1, IDLE}, {LPC_I2C2, IDLE}};

bool I2C_Start(char id, char* data, size_t data_size, bool receive){

}

char I2C_Status(char id){
	return ctrl_arr[id].state;
}

bool I2C_Stop(char id){

}
