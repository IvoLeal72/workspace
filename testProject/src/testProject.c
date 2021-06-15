/*
===============================================================================
 Name        : testProject.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "led.h"
#include "wait.h"
#include "lcd.h"
#include "esp8266.h"

int main(void) {
	WAIT_Init();
	LED_Init(0b100);
	LED_SetState(ESP_Init());
	WIFI_NETWORK wifi_list[40];
	wifi_list[0].secur=6;
	int res=ESP_ListAp(wifi_list, 40);
	if(res<0) printf("retornou erro %d\n", res);
	putchar(10);
	for(int i=0; i<res; i++){
		printf("%d ; name: %s ; mac: %s\n", wifi_list[i].secur, wifi_list[i].ssid, wifi_list[i].mac);
	}
	WIFI_NETWORK con;
	strcpy(con.ssid, "NOS-LEAL 2");
	strcpy(con.pwd, "leal697121");
	con.secur=4;
	printf("%d\n", ESP_SetAp(con, false));
	return 0;
}
