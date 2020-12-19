/*
 * rtc.h
 *
 *  Created on: 15/12/2020
 *      Author: ivo
 */

#ifndef RTC_H_
#define RTC_H_

#include <time.h>

void RTC_Init(time_t seconds);

void RTC_GetValue(struct tm *dateTime);

void RTC_SetValue(struct tm *dateTime);

void RTC_SetSeconds(time_t seconds);

time_t RTC_GetSeconds();

#endif /* RTC_H_ */
