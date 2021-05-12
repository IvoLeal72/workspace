/*
 * i2c.c
 *
 *  Created on: 27/03/2021
 *      Author: ivogc
 */

#include <stdio.h>
#include "i2c.h"
#include "Utils.h"

#ifdef __USE_CMSIS
	#include "LPC17xx.h"
#endif

#define I2C_CCLK_DIVIDER 8
#define TIMEOUT 1000
#define I2C_CTT 3

#define AA (1<<2)
#define SI (1<<3)
#define STO (1<<4)
#define STA (1<<5)

struct I2C_controller{
	LPC_I2C_TypeDef* perif;
	char state;
	char addr;
	bool receive;
	char* data;
	size_t dataIdx;
	size_t dataSize;
	bool auto_stop;
};

static struct I2C_controller I2C_ctrl_arr[I2C_CTT]={{LPC_I2C0}, {LPC_I2C1}, {LPC_I2C2}};

static void I2Cn_IRQHandler(int id){
	struct I2C_controller* ctrl=&I2C_ctrl_arr[id];
	char STAT=ctrl->perif->I2STAT;
	switch(STAT){
		case 0x08: case 0x10:
			ctrl->state=BUSY;
			ctrl->perif->I2DAT=(ctrl->addr<<1) | (ctrl->receive?1:0);
			ctrl->perif->I2CONCLR=SI | STA;
			return;
		case 0x18: case 0x28:
			if(ctrl->receive) break;
			if(ctrl->dataIdx < ctrl->dataSize){
				ctrl->perif->I2DAT=ctrl->data[ctrl->dataIdx++];
				ctrl->perif->I2CONCLR=SI;
				return;
			}
			if(ctrl->auto_stop){
				ctrl->perif->I2CONSET=STO;
				ctrl->perif->I2CONCLR=SI;
				while(ctrl->perif->I2CONSET & STO);
				ctrl->state=DONE_STP;
			}
			else
				ctrl->state=DONE;
			NVIC_DisableIRQ(I2C0_IRQn+id);
			return;
		case 0x30:
			if(ctrl->receive || ctrl->dataIdx!=ctrl->dataSize) break;
			if(ctrl->auto_stop){
				ctrl->perif->I2CONSET=STO;
				ctrl->perif->I2CONCLR=SI;
				while(ctrl->perif->I2CONSET & STO);
				ctrl->state=DONE_STP;
			}
			else
				ctrl->state=DONE;
			NVIC_DisableIRQ(I2C0_IRQn+id);
			return;
		case 0x40:
			if(!ctrl->receive) break;
			if(ctrl->dataIdx<(ctrl->dataSize-1)) ctrl->perif->I2CONSET=AA;
			else ctrl->perif->I2CONCLR=AA;
			ctrl->perif->I2CONCLR=SI;
			return;
		case 0x50:
			if(!ctrl->receive) break;
			ctrl->data[ctrl->dataIdx]=ctrl->perif->I2DAT;
			if(ctrl->dataIdx<(ctrl->dataSize-2)) ctrl->perif->I2CONSET=AA;
			else ctrl->perif->I2CONCLR=AA;
			ctrl->dataIdx++;
			ctrl->perif->I2CONCLR=SI;
			return;
		case 0x58:
			if(!ctrl->receive || ctrl->dataIdx!=(ctrl->dataSize-1)) break;
			ctrl->data[ctrl->dataIdx]=ctrl->perif->I2DAT;
			if(ctrl->auto_stop){
				ctrl->perif->I2CONSET=STO;
				ctrl->perif->I2CONCLR=SI;
				while(ctrl->perif->I2CONSET & STO);
				ctrl->state=DONE_STP;
			}
			else
				ctrl->state=DONE;
			NVIC_DisableIRQ(I2C0_IRQn+id);
			return;
		case 0xf8: return;
	}
	ctrl->state=ERROR;
	ctrl->perif->I2CONSET=STO;
	ctrl->perif->I2CONCLR=SI;
	while(ctrl->perif->I2CONSET & STO);
	NVIC_DisableIRQ(I2C0_IRQn+id);
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

void I2C_Init(int id, char options){
	switch(id){
		case 0:
			LPC_SC->PCONP |= 1<<7;
			set_PCLK(I2C_CCLK_DIVIDER, 0, 14);
			set_PINSEL(1, 1, 22);
			set_PINSEL(1, 1, 24);
			LPC_PINCON->I2CPADCFG=0;
			break;
		case 1:
			LPC_SC->PCONP |= 1<<19;
			set_PCLK(I2C_CCLK_DIVIDER, 1, 6);
			switch(options){
				case 1:
					set_PINSEL(3, 1, 6);
					set_PINSEL(3, 1, 8);
					set_PINMODE(2, 1, 6);
					set_PINMODE(2, 1, 8);
					set_PINMODE_OD(1, 0, 19);
					set_PINMODE_OD(1, 0, 20);
					break;
				default:
					set_PINSEL(3, 0, 0);
					set_PINSEL(3, 0, 2);
					set_PINMODE(2, 0, 0);
					set_PINMODE(2, 0, 2);
					set_PINMODE_OD(1, 0, 0);
					set_PINMODE_OD(1, 0, 1);
			}
			break;
		case 2:
			LPC_SC->PCONP |= 1<<26;
			set_PCLK(I2C_CCLK_DIVIDER, 1, 20);
			set_PINSEL(2, 0, 20);
			set_PINSEL(2, 0, 22);
			set_PINMODE(2, 0, 20);
			set_PINMODE(2, 0, 22);
			set_PINMODE_OD(1, 0, 10);
			set_PINMODE_OD(1, 0, 11);
			break;
		default: return;
	}
	I2C_ctrl_arr[id].perif->I2CONSET=1<<6;
	I2C_ctrl_arr[id].state=STOPPED;
}

bool I2C_ConfigTransfer(int id, unsigned int frequency, unsigned int duty_cycle){
	if(id>=I2C_CTT) return false;
	if(I2C_ctrl_arr[id].state!=IDLE && I2C_ctrl_arr[id].state!=ERROR && I2C_ctrl_arr[id].state!=STOPPED) return false;

	unsigned int period_sum=(SystemCoreClock/I2C_CCLK_DIVIDER)/(frequency*1000);
	unsigned short period_high=period_sum*duty_cycle/100;
	if(period_high<4) period_high=4;
	unsigned short period_low=period_sum-period_high;
	if(period_low<4) period_low=4;

	I2C_ctrl_arr[id].perif->I2SCLH=period_high;
	I2C_ctrl_arr[id].perif->I2SCLL=period_low;

	return true;
}

bool I2C_Start(int id, char address, char* data, size_t data_size, bool receive, bool auto_stop){
	if(id>=I2C_CTT) return false;
	struct I2C_controller* ctrl=&I2C_ctrl_arr[id];
	if(ctrl->state==BUSY) return false;
	ctrl->addr=address & 0x7f;
	ctrl->dataIdx=0;
	ctrl->dataSize=data_size;
	ctrl->receive=receive;
	ctrl->data=data;
	ctrl->auto_stop=auto_stop;
	if(!receive){
		ctrl->data=malloc(data_size);
		if(ctrl->data==NULL) return false;
		memcpy(ctrl->data, data, data_size);
	}

	ctrl->perif->I2CONSET=STA;
	uint32_t start=WAIT_GetElapsedMillis(0);
	if(ctrl->state==IDLE) ctrl->perif->I2CONCLR=SI;
	NVIC_EnableIRQ(I2C0_IRQn+id);
	while(WAIT_GetElapsedMillis(start)<TIMEOUT){
		if(ctrl->state==BUSY) return true;
	}
	return false;
}

char I2C_Status(int id){
	if(id>=I2C_CTT) return -1;
	char state=I2C_ctrl_arr[id].state;
	if(state==DONE){
		I2C_ctrl_arr[id].state=IDLE;
	}
	else if(state==DONE_STP){
		I2C_ctrl_arr[id].state=STOPPED;
	}
	return state;
}

bool I2C_Stop(int id){
	if(id>=I2C_CTT) return false;
	struct I2C_controller* ctrl=&I2C_ctrl_arr[id];
	if(ctrl->state==STOPPED) return true;
	if(ctrl->state!=IDLE && ctrl->state!=ERROR) return false;
	ctrl->perif->I2CONSET=1<<4;
	if(ctrl->state==IDLE) ctrl->perif->I2CONCLR=1<<3;
	uint32_t start=WAIT_GetElapsedMillis(0);
	while(WAIT_GetElapsedMillis(start)<TIMEOUT){
		if(!(ctrl->perif->I2CONSET & 1<<4)){
			ctrl->state=STOPPED;
			return true;
		}
	}
	return false;
}

char I2C_GetErrorCode(int id){
	if(I2C_ctrl_arr[id].state!=ERROR) return 0;
	return I2C_ctrl_arr[id].perif->I2STAT &0xff;
}
