/**
* @file		wait.h
* @brief	Contains the delay API.
* @version	1.1
* @date		10 Out 2017
* @author	PSampaio
*
* Copyright(C) 2015-2020, PSampaio
* All rights reserved.
*
***********************************************************************
* Software that is described here in is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
**********************************************************************/

#ifndef _WAIT_H_
#define _WAIT_H_
#include <stdint.h>

/** @defgroup WAIT WAIT
 * This package provides the core capabilities for wait functions.
 * @{
 */

/** @defgroup WAIT_Public_Functions WAIT Public Functions
 * @{
 */

/**
 * @brief	Initialises the wait API for 1 ms resolution.
 * @return	0 if initialisation successed; -1 if fails.
 * @note	This function must be called prior to any other WAIT functions,
 * 			and use the SYSTICK and TIMER0 resources.
 */
int32_t WAIT_Init(void);

/**
 * @brief	Waits a number of milliseconds.
 * @param	millis: the whole number of milliseconds to wait.
 */
void WAIT_Milliseconds(uint32_t millis);

/**
 * @brief	Waits a number of microseconds.
 * @param	waitUs: the whole number of microseconds to wait.
 */
void WAIT_ChronoUs(uint32_t waitUs);

/**
 * @brief	Get difference in milliseconds from parameter.
 * @param	start: if 0 get current milliseconds.
 */
uint32_t WAIT_GetElapsedMillis(uint32_t start);


/**
 * @}
 */


/**
 * @}
 */

#endif /* _WAIT_H_ */
