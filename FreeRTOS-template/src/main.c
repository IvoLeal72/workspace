#include <time.h>
#include <stdio.h>
#include <stdbool.h>

/* Kernel includes. */
#include "FreeRTOS.h"

#include "task.h"
#include "queue.h"
#include "led.h"
#include "lcd.h"

QueueHandle_t xQueue;

void ledTask(void* params){
	(void) params;
	unsigned int i;

	for(;;){
		xQueueReceive(xQueue, &i, -1);
		if(i%4==0) LED_On();
		else LED_Off();
	}
}

void timerTask(void* params){
	(void) params;
	char a='4';

	for(;;){
		vTaskDelay(pdMS_TO);
		xQueueSend(xQueue, &a, -1);
	}
}

int main(void)
{
	LED_Init(false);
	xQueue=xQueueCreate(1,sizeof(unsigned int));

	xTaskCreate(ledTask, "LedTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(timerTask, "LedTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	/* Create tasks */
	vTaskStartScheduler();
	return 0;
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	/* This function will get called if a task overflows its stack. */

	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void vConfigureTimerForRunTimeStats( void )
{
const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

	/* This function configures a timer that is used as the time base when
	collecting run time statistical information - basically the percentage
	of CPU time that each task is utilising.  It is called automatically when
	the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
	to 1). */

	/* Power up and feed the timer. */
	LPC_SC->PCONP |= (1 << 2);
	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & (~(0x3<<4))) | (0x01 << 4);

	/* Reset Timer 1 */
	LPC_TIM1->TCR = TCR_COUNT_RESET;

	/* Just count up. */
	LPC_TIM1->CTCR = CTCR_CTM_TIMER;

	/* Prescale to a frequency that is good enough to get a decent resolution,
	but not too fast so as to overflow all the time. */
	LPC_TIM1->PR =  ( configCPU_CLOCK_HZ / 10000UL ) - 1UL;

	/* Start the counter. */
	LPC_TIM1->TCR = TCR_COUNT_ENABLE;
}
/*-----------------------------------------------------------*/
