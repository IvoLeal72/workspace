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

#define CCLK_DIVIDER 8
#define TIMEOUT 1000
#define CTRL_CTT 3

#define SEND 0
#define RECEIVE 1

struct _controller{
	LPC_I2C_TypeDef* perif;
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
	printf("0x%02X - %d\n", ctrl->perif->I2STAT, ctrl->state);
	if(ctrl->state==ERROR || ctrl->state==DONE) return;
	if(ctrl->dir==SEND){
		switch(ctrl->perif->I2STAT & 0xff){
			case 0xf8: case 0x58:
				return;
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
				printf("%d\n", ctrl->dataIdx);
				break;
			case 0x30:
				if(ctrl->dataIdx==ctrl->dataSize){
					ctrl->state=DONE;
					return;
				}
			default: ctrl->state=ERROR; return;
		}
		ctrl->perif->I2CONCLR=1<<3;
		return;
	}
	if(ctrl->dir==RECEIVE){
		switch(ctrl->perif->I2STAT & 0xff){
			case 0xf8:
				return;
			case 0x08: case 0x10:
				ctrl->perif->I2DAT=(ctrl->addr)<<1 | 1;
				ctrl->state=BUSY;
				ctrl->perif->I2CONCLR=1<<5;
				break;
			case 0x40:
				if(ctrl->dataSize>1) ctrl->perif->I2CONSET=1<<2;
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
	//ctrl_arr[0].perif->I2CONCLR=1<<3;
}

void I2C1_IRQHandler(void){
	I2Cn_IRQHandler(1);
	//ctrl_arr[1].perif->I2CONCLR=1<<3;
}

void I2C2_IRQHandler(void){
	I2Cn_IRQHandler(2);
	//ctrl_arr[2].perif->I2CONCLR=1<<3;
}

void I2C_Init(int id, char options){
	switch(id){
		case 0:
			LPC_SC->PCONP |= 1<<7;
			set_PCLK(CCLK_DIVIDER, 0, 14);
			set_PINSEL(1, 1, 22);
			set_PINSEL(1, 1, 24);
			LPC_PINCON->I2CPADCFG=0;
			break;
		case 1:
			LPC_SC->PCONP |= 1<<19;
			set_PCLK(CCLK_DIVIDER, 1, 6);
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
			set_PCLK(CCLK_DIVIDER, 1, 20);
			set_PINSEL(2, 0, 20);
			set_PINSEL(2, 0, 22);
			set_PINMODE(2, 0, 20);
			set_PINMODE(2, 0, 22);
			set_PINMODE_OD(1, 0, 10);
			set_PINMODE_OD(1, 0, 11);
			break;
		default: return;
	}
	ctrl_arr[id].perif->I2CONSET=1<<6;
	ctrl_arr[id].perif->I2CONCLR=0xf<<2;
	ctrl_arr[id].state=IDLE;
}

bool I2C_Start(int id, char address, char* data, size_t data_size, bool receive, unsigned int frequency, unsigned int duty_cycle){
	if(id>=CTRL_CTT) return false;
	struct _controller* ctrl=&ctrl_arr[id];
	if(ctrl->state==BUSY || ctrl->state==ERROR) return false;
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

	unsigned int period_sum=(SystemCoreClock/CCLK_DIVIDER)/(frequency*1000);
	unsigned short period_high=period_sum*duty_cycle/100;
	if(period_high<4) period_high=4;
	unsigned short period_low=period_sum-period_high;
	if(period_low<4) period_low=4;

	ctrl->perif->I2SCLH=period_high;
	ctrl->perif->I2SCLL=period_low;
	//printf("Start\n");
	NVIC_EnableIRQ(I2C0_IRQn+id);
	ctrl->perif->I2CONSET=1<<5;
	uint32_t start=WAIT_GetElapsedMillis(0);
	while(WAIT_GetElapsedMillis(start)<TIMEOUT){
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
	while(WAIT_GetElapsedMillis(start)<TIMEOUT){
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
