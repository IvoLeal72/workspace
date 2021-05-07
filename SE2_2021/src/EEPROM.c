/*
 * EEPROM.C
 *
 *  Created on: 15/04/2021
 *      Author: ivogc
 */


#ifdef __USE_CMSIS
	#include "LPC17xx.h"
#endif

#include "i2c.h"
#include "EEPROM.h"
#include <string.h>

#define DATA_FREQ 300
#define DUTY_CYCLE 50
#define EEPROM_ADDR 0x50
#define I2C_CTRL 1

static int EEPROM_Transfer(char* data, size_t data_size, bool read, bool auto_stop){
	if(!I2C_Start(I2C_CTRL, EEPROM_ADDR, data, data_size, read, auto_stop)) return -2;
	char status;
	do{
		status=I2C_Status(I2C_CTRL);
		if(status==ERROR) return -2;
		__WFI();
	}while(status!=DONE && status!=DONE_STP);
	if(auto_stop && status==DONE) return -1;
	if(!auto_stop && status==DONE_STP) return -1;
	return 0;
}

int EEPROM_Write(short addr, char* data, size_t data_size){
	if((addr+data_size) > (1<<13) || addr<0) return -1;
	size_t data_idx=0;
	while(data_idx<data_size){
		char to_send=32-((addr+data_idx)&0x1f);
		if(to_send>(data_size-data_idx)) to_send=data_size-data_idx;
		char arr[to_send+2];
		arr[0]=(addr+data_idx)>>8;
		arr[1]=addr+data_idx;
		memcpy(arr+2, data+data_idx, to_send);
		data_idx+=to_send;
		int res=EEPROM_Transfer(arr, to_send+2, false, true);
		if(res!=0) return res;
		WAIT_Milliseconds(15);
	}
	return data_size;
}

int EEPROM_Read(short addr, char* data, size_t data_size){
	if((addr+data_size) > (1<<13) || addr<0) return -1;
	char arr[]={addr>>8, addr&0xff};
	int res=EEPROM_Transfer(arr, 2, false, false);
	if(res!=0) return res;
	res=EEPROM_Transfer(data, data_size, true, true);
	if(res!=0) return res;
	return data_size;
}
