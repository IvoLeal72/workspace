/*
 * RTOS_Buttons.h
 *
 *  Created on: 26/06/2021
 *      Author: ivo
 */

#ifndef RTOS_BUTTONS_H_
#define RTOS_BUTTONS_H_

#include <stdbool.h>
#include "button.h"

bool RTOS_Buttons_Init();

int RTOS_Buttons_GetEvents(uint32_t timeout);

#endif /* RTOS_BUTTONS_H_ */
