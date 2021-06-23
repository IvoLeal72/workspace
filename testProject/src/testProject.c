/*
===============================================================================
 Name        : testProject.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC17xx.h"

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
	return 0;
}
