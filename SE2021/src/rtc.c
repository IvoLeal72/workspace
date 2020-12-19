/*
 * rtc.c
 *
 *  Created on: 15/12/2020
 *      Author: ivo
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdbool.h>
#include "rtc.h"


void RTC_Init(time_t seconds){
	LPC_SC->PCONP |= 1<<9;

	RTC_SetSeconds(seconds);
}

void RTC_SetSeconds(time_t seconds){
	struct tm *time=localtime(&seconds);
	RTC_SetValue(time);
}

void RTC_SetValue(struct tm *dateTime){
	LPC_RTC->CCR = 2;
	LPC_RTC->SEC = dateTime->tm_sec;
	LPC_RTC->MIN = dateTime->tm_min;
	LPC_RTC->HOUR = dateTime->tm_hour;
	LPC_RTC->DOM = dateTime->tm_mday;
	LPC_RTC->DOW = dateTime->tm_wday;
	LPC_RTC->DOY = dateTime->tm_yday;
	LPC_RTC->MONTH = dateTime->tm_mon + 1;
	LPC_RTC->YEAR = dateTime->tm_year + 1900;
	LPC_RTC->CCR=1;
}

void RTC_GetValue(struct tm *dateTime){
	int i=0;
	int ctime0[3]={0};
	int ctime1[3]={0};
	int ctime2[3]={0};
	bool verified=false;

	while(!verified){
		ctime0[i]=LPC_RTC->CTIME0 & ~(0b11<<6 | 0b11<<14 | 0b111<<21 | 0b11111<<27);
		ctime1[i]=LPC_RTC->CTIME1 & ~(0b111<<5 | 0b111<<12 | 0b1111<<28);
		ctime2[i]=LPC_RTC->CTIME2 & 0xFFF;
		verified=true;
		if(ctime0[0]!=ctime0[1] || ctime0[1]!=ctime0[2]) verified=false;
		if(ctime1[0]!=ctime1[1] || ctime1[1]!=ctime1[2]) verified=false;
		if(ctime2[0]!=ctime2[1] || ctime2[1]!=ctime2[2]) verified=false;
		i=(i+1)%3;
	}

	dateTime->tm_sec=ctime0[0] & 0x3F;
	dateTime->tm_min=(ctime0[0] <<8) & 0x3F;
	dateTime->tm_hour=(ctime0[0] <<16) & 0x1F;
	dateTime->tm_wday=(ctime0[0] <<24) & 7;
	dateTime->tm_mday=ctime1[0] & 0x1F;
	dateTime->tm_mon=((ctime1[0] <<8) & 0xF)-1;
	dateTime->tm_year=((ctime1[0] <<16) & 0xFFF) -1900;
	dateTime->tm_yday=ctime2[0] & 0xFFF;
	dateTime->tm_isdst=0;
}

time_t RTC_GetSeconds(){
	struct tm time;
	RTC_GetValue(&time);
	return mktime(&time);
}
