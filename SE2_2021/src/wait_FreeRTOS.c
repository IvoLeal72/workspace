/*
 * wait.c
 *
 *  Created on: 10 Out 2017
 *      Author: psampaio
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "wait.h"
#include "FreeRTOS.h"
#include "task.h"

#define pdTICKS_TO_MS( xTicks )    ( ( ( TickType_t ) ( xTicks ) * 1000u ) / configTICK_RATE_HZ )

void WAIT_Milliseconds(uint32_t millis)
{
	vTaskDelay(pdMS_TO_TICKS(millis));
}

int32_t WAIT_Init(void)
{
	SystemCoreClockUpdate();
    LPC_TIM0->PR = (SystemCoreClock/10000000) -1;
	LPC_TIM0->MCR = 0b11;
	LPC_TIM0->CTCR = 0;
	LPC_TIM0->TCR=0b11;
	LPC_SC->PCLKSEL0 |= 1<<2;
	LPC_SC->PCLKSEL0 &= ~(1<<3);
	LPC_SC->PCONP |= 0b10;
    return 0;
}

uint32_t WAIT_GetElapsedMillis(uint32_t start)
{
	return pdTICKS_TO_MS(xTaskGetTickCount()) - start;
}

/*void WAIT_ChronoUs(uint32_t waitUs){
	LPC_TIM0->MR0 = waitUs*10;
	LPC_TIM0->IR =1;
	LPC_TIM0->TCR=1;
	while((LPC_TIM0->IR & 1)==0);
	LPC_TIM0->TCR=0b11;
}*/
