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

#define DATA_FREQ 400
#define DUTY_CYCLE 50
#define EEPROM_ADDR 0x50
#define I2C_CTRL 1

static int EEPROM_Transfer(char* data, size_t data_size, bool read){
	if(!I2C_Start(I2C_CTRL, EEPROM_ADDR, data, data_size, read, DATA_FREQ, DUTY_CYCLE)) return -2;
	char status;
	while((status=I2C_Status(I2C_CTRL))!=DONE){
		if(status==ERROR) return -2;
		__WFI();
	}
	return 0;
}

int EEPROM_Write(short addr, char* data, size_t data_size){
	if((addr+data_size) >= (1<<13) || addr<0) return -1;
	size_t data_idx=0;
	while(data_idx<data_size){
		char to_send=32-((addr+data_idx)&0x1f);
		if(to_send>(data_size-data_idx)) to_send=data_size-data_idx;
		char arr[to_send+2];
		arr[0]=(addr+data_idx)>>8;
		arr[1]=addr+data_idx;
		int res=EEPROM_Transfer(arr, to_send+2, false);
		if(res!=0) return res;
		data_idx+=to_send;
		WAIT_Milliseconds(7);
	}
	if(!I2C_Stop(I2C_CTRL)) return -2;
	return data_size;
}

int EEPROM_Read(short addr, char* data, size_t data_size){
	if((addr+data_size) >= (1<<13) || addr<0) return -1;
	char arr[]={addr>>8, addr&0xff};
	int res=EEPROM_Transfer(arr, 2, false);
	if(res!=0) return res;
	res=EEPROM_Transfer(data, data_size, true);
	if(res!=0) return res;
	if(!I2C_Stop(I2C_CTRL)) return -2;
	return data_size;
}
