#include <time.h>
#include <stdio.h>
#include <stdbool.h>

/* Kernel includes. */
#include "FreeRTOS.h"

#include "task.h"
#include "queue.h"
#include "led.h"
#include "lcd.h"
#include "esp8266.h"
#include "wait.h"

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
		//vTaskDelay(pdMS_TO);
		xQueueSend(xQueue, &a, -1);
	}
}

void testTask(void* params){
	WAIT_Init();
	LED_Init(0b100);
	LED_SetState(ESP_Init());
	/*WIFI_NETWORK wifi_list[40];
	wifi_list[0].secur=6;
	int res=ESP_ListAp(wifi_list, 40);
	if(res<0) printf("retornou erro %d\n", res);
	putchar(10);
	for(int i=0; i<res; i++){
		printf("%d ; name: %s ; mac: %s\n", wifi_list[i].secur, wifi_list[i].ssid, wifi_list[i].mac);
	}*/
	WIFI_NETWORK con;
	strcpy(con.ssid, "NOS-LEAL 2");
	strcpy(con.pwd, "leal697121");
	con.secur=4;
	char *testStr="123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n";
	printf("%d\n", ESP_SetAp(con, false));
	printf("%s\n", ESP_RemoteStart("TCP", "192.168.1.17", 54321)?"TRUE":"FALSE");
	printf("%s\n", ESP_RemoteSend(testStr, strlen(testStr))?"TRUE":"FALSE");
	LED_SetState(0);
	while(!ESP_WaitForIPD(60000));
	LED_SetState(0b10);
	ESP_DATA* data=NULL;
	data=ESP_RemoteReceive();
	printf("%s\n", ESP_RemoteStop()?"TRUE":"FALSE");
	printf("%s\n", memcmp(testStr, data->buffer, strlen(testStr))==0?"TRUE":"FALSE");
}

int main(void)
{

	xTaskCreate(testTask, "testTask", 64*configMINIMAL_STACK_SIZE, NULL, 1, NULL);
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
