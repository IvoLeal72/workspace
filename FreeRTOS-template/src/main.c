#include <time.h>
#include <stdio.h>
#include <stdbool.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "wait.h"
#include "RTOS_LCD.h"
#include "RTOS_Buttons.h"
#include "esp8266.h"
#include "MQTTPacket.h"

SemaphoreHandle_t sem;

void task1(void* parameters){
	WAIT_Milliseconds(100);
	char bits[7];
	bits[6]=0;
	int buttons=0;
	while(1){
		buttons=RTOS_Buttons_GetEvents(portMAX_DELAY);
		if(buttons==-1) for(;;);
		for(int i=0; i<6; i++){
			bits[i]=(buttons&(0x20>>i))?'1':'0';
		}
		RTOS_LCD_LocatedPrint(0,0, bits, true);
	}
}

void gameTask(void*);

int main(void)
{
	LED_Init(false);
	WAIT_Init();
	RTOS_LCD_Init();
	RTOS_Buttons_Init();
	ADXL345_Init();
	RTC_Init(949881600);
	ScorePublisher_Init();
	xTaskCreate(gameTask, "gameTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

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
