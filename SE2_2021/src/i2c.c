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
			case 0x08: case 0x10:
				ctrl->perif->I2DAT=(ctrl->addr)<<1;
				ctrl->state=BUSY;
				ctrl->perif->I2CONCLR=1<<5;
				break;
			case 0x18: case 0x28:
				if(ctrl->dataIdx==ctrl->dataSize){
					ctrl->state=DONE;
					return;
				}
				ctrl->perif->I2DAT=ctrl->data[ctrl->dataIdx];
				ctrl->dataIdx++;
				break;
			default: ctrl->state=ERROR; return;
		}
		ctrl->perif->I2CONCLR=1<<3;
		return;
	}
	if(dir==RECEIVE){
		switch(ctrl->perif->I2STAT & 0xff){
			case 0x08: case 0x10:
				ctrl->perif->I2DAT=(ctrl->addr)<<1;
				ctrl->state=BUSY;
				ctrl->perif->I2CONCLR=1<<5;
				break;
			case 0x40:
				if(data_size>1) ctrl->perif->I2CONSET=1<<2;
				else ctrl->perif->I2CONCLR=1<<2;
				break;
			case 0x50:
				if(ctrl->dataIdx==ctrl->dataSize-1) ctrl->perif->I2CONCLR=1<<2;
				else ctrl->perif->I2CONSET=1<<2;
				ctrl->data[ctrl->dataIdx]=ctrl->perif->I2DAT & 0xff;
				ctrl->dataIdx++;
				break;
			case 0x58:
				ctrl->data[ctrl->dataIdx]=ctrl->perif->I2DAT & 0xff;
				ctrl->state=DONE;
				return;
			default:
				ctrl->state=ERROR;
		}
		ctrl->perif->I2CONCLR=1<<3;
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
	struct _controller* ctrl=&ctrl_arr[id];
	if(ctrl->state==BUSY | ctrl->state==ERROR) return false;
	ctrl->addr=address & 0x7f;
	ctrl->dataIdx=0;
	ctrl->dataSize=data_size;
	ctrl->dir=receive;
	ctrl->data=data;
	if(!receive){
		ctrl->data=malloc(data_size);
		if(ctrl->data==NULL) return false;
		memcpy(ctrl->data, data, data_size);
	}
	ctrl->perif->I2CONCLR=0x1c;
	ctrl->perif->I2CONSET=1<<5;
	uint32_t start=WAIT_GetElapsedMillis(0);
	while(WAIT_GetElapsedMillis(start)<1000){
		if(ctrl->state==BUSY) return true;
	}
	return false;
}

char I2C_Status(int id){
	if(id>=CTRL_CTT) return -1;
	return ctrl_arr[id].state;
}

bool I2C_Stop(int id){
	if(id>=CTRL_CTT) return false;
	struct _controller* ctrl=&ctrl_arr[id];
	if(ctrl->state!=DONE) return false;
	ctrl->perif->I2CONSET=1<<4;
	uint32_t start=WAIT_GetElapsedMillis(0);
	while(WAIT_GetElapsedMillis(start)<1000){
		if(ctrl->perif->I2CONSET & 1<<4){
			ctrl->state=IDLE;
			return true;
		}
	}
	return false;
}

char I2C_GetErrorCode(int id){
	if(ctrl_arr[id].state!=ERROR) return 0;
	return ctrl_arr[id].perif->I2STAT &0xff;
}
