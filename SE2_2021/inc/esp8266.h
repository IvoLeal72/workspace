/*
 * esp8266.h
 *
 *  Created on: 02/06/2021
 *      Author: ivo
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#include <stdbool.h>

typedef struct{
	int secur;
	char ssid[33];
	char pwd[64];
	char mac[18];
}WIFI_NETWORK;

typedef struct{
	char* buffer;
	size_t buff_size;
}ESP_DATA;

bool ESP_Init();

int ESP_ListAp(WIFI_NETWORK list[], size_t max);

int ESP_SetAp(WIFI_NETWORK network, bool useMac);

ESP_DATA* ESP_RemoteReceive();

bool ESP_RemoteStart(char* type, char* addr, unsigned int port);

bool ESP_RemoteSend(char* data, size_t length);

bool ESP_RemoteStop();

bool ESP_WaitForIPD(uint32_t timeout);

#endif /* ESP8266_H_ */
