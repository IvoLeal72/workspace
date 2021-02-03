/*
 * rtc.h
 *
 *  Created on: 15/12/2020
 *      Author: ivo
 */

#ifndef RTC_H_
#define RTC_H_
#include <time.h>

/** @defgroup RTC RTC
 * This package provide an api for time keeping using the RTC peripheral
 * @{
 */

/** @defgroup RTC_Public_Functions RTC Public Functions
 * @{
 */

/**
 * @brief	Initialises the RTC api to be able to use it.
 * @param 	seconds: time_t expressing the wanted time in seconds since 1/1/1970
 * @note	This function must be called prior to any other RTC functions,
 * 			and use the RTC resource.
 */
void RTC_Init(time_t seconds);

/**
 * @brief	Reads the time in the RTC peripheral and updates the tm struct pointed by dateTime accordingly.
 * @param	dateTime: pointer to tm struct where the values of the rtc will be saved
 */
void RTC_GetValue(struct tm *dateTime);


/**
 * @brief	Updates the RTC peripheral with the values in the tm struct pointed by dateTime
 * @param	dateTime: valid pointer to a tm struct with the time to set to the RTC
 */
void RTC_SetValue(struct tm *dateTime);

/**
 * @brief	Updates the RTC peripheral with the correct dated based on the seconds since 1/1/1970
 * @param	seconds: time_t expressing the wanted time in seconds since 1/1/1970
 */
void RTC_SetSeconds(time_t seconds);

/**
 * @brief	Reads the RTC and returns the time expressed in seconds since 1/1/1970
 * @return	time_t expressing the read time in seconds since 1/1/1970
 */
time_t RTC_GetSeconds();

/**
 * @}
 */

/**
 * @}
 */

#endif /* RTC_H_ */
