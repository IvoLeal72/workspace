/*
 * flash.c
 *
 *  Created on: 23/01/2021
 *      Author: ivogc
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdlib.h>
#include "flash.h"

#define IAP_ADRESS 0x1fff1ff1

typedef void (*IAP)(unsigned int[], unsigned int[]);
IAP iap_entry=(IAP) IAP_ADRESS;


static unsigned int FLASH_Prepare(unsigned int startSector, unsigned int endSector){
	unsigned int param[3]={50, startSector, endSector};
	unsigned int res[1];
	iap_entry(param, res);
	return res[0];
}

unsigned int FLASH_EraseSectors(unsigned int startSector, unsigned int endSector){
	unsigned int ret=FLASH_Prepare(startSector, endSector);
	if(ret!=0) return ret;
	unsigned int param[4]={52, startSector, endSector, SystemCoreClock/1000};
	unsigned int res[1];
	iap_entry(param, res);
	return res[0];
}

unsigned int FLASH_WriteData(void *dstAdress, void *srcAdress, unsigned int size){
	if(size<=256) size=256;
	else if(size<=512) size=512;
	else if(size<=1024) size=1024;
	else size=4096;
	unsigned int dst=(unsigned int)dstAdress;
	unsigned int src=(unsigned int)srcAdress;
	dst&=~0xFF;
	src&=~3;
	unsigned int startSector, endSector;
	if(dst<0x10000) startSector=dst>>12;
	else startSector=(dst>>15) + 14;
	dst+=size;
	if(dst<0x10000) endSector=dst>>12;
	else endSector=(dst>>15) + 14;
	dst-=size;
	unsigned int ret=FLASH_Prepare(startSector, endSector);
	if(ret!=0) return ret;
	unsigned int param[5]={51, dst, src, size, SystemCoreClock/1000};
	unsigned int result[1];
	iap_entry(param, result);
	return result[0];
}

unsigned int FLASH_VerifyData(void *dstAdress, void *srcAdress, unsigned int size){
	unsigned int src=(unsigned int)srcAdress;
	unsigned int dst=(unsigned int)dstAdress;
	src&=~3;
	dst&=~3;
	if(size%4!=0) size+=4-size%4;
	unsigned int param[4]={56, dst, src, size};
	unsigned int result[2];
	iap_entry(param, result);
	return result[0];
}
