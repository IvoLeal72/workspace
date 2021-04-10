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
	const LPC_I2C_TypeDef* perif;
	char state;
	char addr;
	char dir;
	char* data;
	size_t dataIdx;
	size_t dataSize;
};

static struct _controller ctrl_arr[CTRL_CTT]={{LPC_I2C0, IDLE}, {LPC_I2C1, IDLE}, {LPC_I2C2, IDLE}};

static void I2Cn_IRQHandler(int n){
	struct _controller* ctrl=&ctrl_arr[n];
	if(ctrl->state==ERROR | ctrl->state==DONE) return;
	if(dir==SEND){
		switch(ctrl->perif->I2STAT & 0xff){
			case 0x08: case 0x10: ctrl->perif->I2DAT=(ctrl->addr)<<1; ctrl->state=BUSY; ctrl->perif->I2CONCLR=1<<3 | 1<<5; break;
			case 0x18: case 0x28: ctrl->perif->I2DAT=ctrl->data[ctrl->dataIdx]; ctrl->dataIdx++; if(ctrl->dataIdx==ctrl->dataSize) ctrl->state=DONE; ctrl->perif->I2CONCLR=1<<3; break;
			default: ctrl->state=ERROR;
		}
		return;
	}
	if(dir==RECEIVE){

		return;
	}
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

bool I2C_Start(int id, char address, char* data, size_t data_size, bool receive){
	if(id>=CTRL_CTT) return false;

}

char I2C_Status(int id){
	if(id>=CTRL_CTT) return -1;
	return ctrl_arr[id].state;
}

bool I2C_Stop(int id){
	return false;
}
