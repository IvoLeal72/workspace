/*
 * esp8266.c
 *
 *  Created on: 02/06/2021
 *      Author: ivo
 */

#include "LPC17xx.h"
#include "wait.h"
#include "esp8266.h"
#include "UART.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define UART_ID 2
#define UART_OPT 0
#define BAUD_RATE 115200
#define ESP_RST_PIN 5
#define CHAR_TIMEOUT 1000

struct _ipdNode{
	struct _ipdNode* next;
	char* buffer;
	size_t buff_size;
};

struct _ipdNode* head=NULL;

static char lastReads[6]={0};

static void ESP_HardRst(){
	LPC_GPIO0->FIOCLR=1<<ESP_RST_PIN;
	WAIT_Milliseconds(1);
	LPC_GPIO0->FIOSET=1<<ESP_RST_PIN;
	WAIT_Milliseconds(10000); //delay para arranque, ajustar
}

static void ESP_SendCommand(char* cmd){
	for(int i=0; i<strlen(cmd);)
		i+=UART_WriteBuffer(UART_ID, (unsigned char*)cmd+i, strlen(cmd)-i);
}

static void ESP_GotIPD(){
	char tmp=0;
	size_t size=0;
	while(tmp!=':'){
		if(UART_ReadBuffer(UART_ID, (unsigned char*)&tmp, 1)==1 && tmp!=':'){
			size=size*10+tmp-'0';
		}
	}
	char* buff=malloc(size*sizeof(char));
	if(buff==NULL)
		for(;;);
	for(int i=0; i<size;)
		i+=UART_ReadBuffer(UART_ID, (unsigned char*)buff+i, size-i);
	struct _ipdNode* new=malloc(sizeof(struct _ipdNode));
	if(new==NULL)
		for(;;);
	new->next=NULL;
	new->buffer=buff;
	new->buff_size=size;
	if(head==NULL) head=new;
	struct _ipdNode* node=head;
	while(node->next != NULL)
		node=node->next;
	node->next=new;
}

static bool ESP_WaitForOk(uint32_t timeout){
	char tmp;
	uint32_t lastTime=WAIT_GetElapsedMillis(0);
	while(true){
		if(UART_ReadBuffer(UART_ID, (unsigned char*)&tmp, 1)==1){
			memmove(lastReads, lastReads+1, 4);
			lastReads[4]=tmp;
			if(strcmp(lastReads+1, "OK\r\n")==0) return true;
			if(strcmp(lastReads, "+IPD,")==0) ESP_GotIPD();
			lastTime=WAIT_GetElapsedMillis(0);
		}
		if(WAIT_GetElapsedMillis(lastTime)>timeout) return false;
	}
	return true;
}

static void ESP_SendData(char* data, size_t len){
	for(int i=0; i<len;)
		i+=UART_WriteBuffer(UART_ID, (unsigned char*)data+i, len-i);
}

static int ESP_ReceiveData(char* buffer, size_t max, size_t* received, uint32_t timeout){
	size_t i=0;
	uint32_t lastTime=WAIT_GetElapsedMillis(0);
	*received=0;
	while(i<max){
		if(UART_ReadBuffer(UART_ID, (unsigned char*)buffer+i, 1)==1){
			memmove(lastReads, lastReads+1, 4);
			lastReads[4]=buffer[i++];
			*received=i;
			if(strcmp(lastReads+1, "OK\r\n")==0) return 0;
			if(strcmp(lastReads, "+IPD,")==0){
				ESP_GotIPD();
				if(i>=5) i-=5;
				else i=0;
			}
			lastTime=WAIT_GetElapsedMillis(0);
		}
		if(WAIT_GetElapsedMillis(lastTime)>timeout) return -2;
	}
	return -1;
}

bool ESP_Init(){
	LPC_GPIO0->FIODIR|=1<<ESP_RST_PIN;
	ESP_HardRst();
	UART_Initialize(UART_ID, UART_OPT, BAUD_RATE);

	ESP_SendCommand("AT\r\n");
	if(!ESP_WaitForOk(CHAR_TIMEOUT)) return false;

	ESP_SendCommand("AT+CWMODE_CUR=1\r\n");
	if(!ESP_WaitForOk(CHAR_TIMEOUT)) return false;

	ESP_SendCommand("AT+CIPMUX=0\r\n");
	if(!ESP_WaitForOk(CHAR_TIMEOUT)) return false;

	ESP_SendCommand("AT+CIPDINFO=0\r\n");
	if(!ESP_WaitForOk(CHAR_TIMEOUT)) return false;

	ESP_SendCommand("AT+CIPDNS_CUR=1,\"8.8.8.8\",\"8.8.4.4\"\r\n");
	if(!ESP_WaitForOk(CHAR_TIMEOUT)) return false;

	return true;
}

static int ESP_ApParser(WIFI_NETWORK* network, char* str){
	char* tmp;
	const char delim[]="\"";
	int i=0;
	memset(network->ssid, 0, 33);
	memset(network->pwd, 0, 64);
	memset(network->mac, 0, 18);
	tmp=strchr(str, '(')+1;
	network->secur=tmp[0]-'0';
	tmp=strtok(str, delim);
	while(tmp!=NULL){
		if(i==1) strncpy(network->ssid, tmp, 32);
		if(i==3) strncpy(network->mac, tmp, 63);
		tmp=strtok(NULL, delim);
		i++;
	}
	if(i!=5) return -1;
	return 0;
}

int ESP_ListAp(WIFI_NETWORK list[], size_t max){
	ESP_SendCommand("AT+CWLAP\r\n");
	char buffer[4096]={0};
	size_t rec;
	int res=ESP_ReceiveData(buffer, 4095, &rec, 20*CHAR_TIMEOUT);
	if(res<0) return res;
	int ctt=0;
	for(int i=0; i<rec; i++)
		if(buffer[i]=='\n')
			ctt++;
	ctt-=3;
	char *lines[ctt];
	strtok(buffer, "\n");
	for(int i=0; i<ctt; i++){
		lines[i]=strtok(NULL, "\n");
	}
	for(int i=0; i<max && i<ctt; i++){
		if(ESP_ApParser(list+i, lines[i])!=0) return -1;
	}
	if(ctt<max) return ctt;
	return max;
}

int ESP_SetAp(WIFI_NETWORK network, bool useMac){
	if(network.secur>4) return -3;
	char str[200];
	if(network.secur==0) network.pwd[0]=0;
	if(useMac) sprintf(str, "AT+CWJAP_CUR=\"%s\",\"%s\",\"%s\"\r\n", network.ssid, network.pwd, network.mac);
	else sprintf(str, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", network.ssid, network.pwd);
	ESP_SendCommand(str);
	size_t rec;
	int res=ESP_ReceiveData(str, 199, &rec, 20*CHAR_TIMEOUT);
	str[rec]=0;
	if(res==-3){
		char* tmp=strchr(str, ':')+1;
		return tmp[0]-'0';
	}
	return res;
}

ESP_DATA* ESP_RemoteReceive(){
	if(head==NULL) return NULL;
	ESP_DATA* data=malloc(sizeof(ESP_DATA));
	if(data==NULL)
		for(;;);
	data->buffer=head->buffer;
	data->buff_size=head->buff_size;

	struct _ipdNode* old_head=head;
	head=old_head->next;
	free(old_head);
	return data;
}

bool ESP_RemoteStart(char* type, char* addr, unsigned int port, unsigned int keep_alive){
	char str[2048];
	sprintf(str, "AT+CIPSTART=\"%s\",\"%s\",%d,%d\r\n", type, addr, port, keep_alive);
	ESP_SendCommand(str);
	size_t rec=0;
	int ret=ESP_ReceiveData(str, 2047, &rec, 10*CHAR_TIMEOUT);
	if(ret==0) return true;
	if(rec<19) return false;
	if(memcmp(str+rec-20, "ALREADY CONNECTED\r\n", 19)==0) return true;
	return false;
}

bool ESP_RemoteSend(char* data, size_t length){
	char str[18]={0};
	if(length>2048) length=2048;
	sprintf(str, "AT+CIPSEND=%d\r\n", length);
	ESP_SendCommand(str);
	char tmp=0;
	size_t rec=0;
	while(tmp!='>' || rec!=1){
		if(ESP_ReceiveData(&tmp, 1, &rec, 20*CHAR_TIMEOUT)==-2)
			return false;
	}
	ESP_SendData(data, length);
	return ESP_WaitForOk(20*CHAR_TIMEOUT);
}

bool ESP_RemoteStop(){
	ESP_SendCommand("AT+CIPCLOSE\r\n");
	return ESP_WaitForOk(10*CHAR_TIMEOUT);
}

char buffer[20];

bool ESP_WaitForIPD(uint32_t timeout){
	if(head!=NULL) return true;
	char tmp=0;
	bool received=false;
	WAIT_Milliseconds(10000);
	UART_ReadBuffer(UART_ID, buffer, 20);
	uint32_t lastTime=WAIT_GetElapsedMillis(0);
	while(!received){
		if(UART_ReadBuffer(UART_ID, (unsigned char*)&tmp, 1)==1){
			memmove(lastReads, lastReads+1, 4);
			lastReads[4]=tmp;
			if(strcmp(lastReads, "+IPD,")==0){
				ESP_GotIPD();
				received=true;
			}
			lastTime=WAIT_GetElapsedMillis(0);
		}
		if(WAIT_GetElapsedMillis(lastTime)>timeout) return false;
	}
	return true;
}





