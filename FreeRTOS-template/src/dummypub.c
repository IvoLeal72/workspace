/*
 * dummypub.c
 *
 *  Created on: 2 Jun 2020
 *      Author: psampaio
 */

#include <transport.h>
#include "LPC17xx.h"
#include "MQTTPacket.h"
#include "esp8266.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

#define groupId 11
unsigned int toPublish=0;

enum {STATE_INIT, STATE_CONNECT, STATE_WAIT_CONNECT, STATE_PUBLISH};

static void ScorePublisher_Task(void *pvParameters);

static ESP_DATA* lastData;
static unsigned int lastData_idx=0;
#define SAVED_BYTES (lastData->buff_size-lastData_idx)

bool ScorePublisher_Init(){
	return xTaskCreate(ScorePublisher_Task, "MQTT", 16*configMINIMAL_STACK_SIZE, NULL, 2, NULL)==pdPASS;
}

bool NETWORK_Init(void) {
	lastData=NULL;
	WIFI_NETWORK conn;
	strcpy(conn.ssid, "MI A2 LITE");
	strcpy(conn.pwd, "leal697121");
	conn.secur=3;
	return ESP_Init() && (ESP_SetAp(conn, false)==0);
}

bool NETWORK_Connect(const char *host, const unsigned short int port,
		const unsigned short int keepalive) {
	return ESP_RemoteStart("TCP", host, port, keepalive);
}

int NETWORK_Send(unsigned char *address, unsigned int bytes) {
	return ESP_RemoteSend(address, bytes)?bytes:0;
}

int NETWORK_Recv(unsigned char *address, unsigned int maxbytes) {
	unsigned int max=maxbytes;
	if(lastData==NULL || SAVED_BYTES==0){
		if(lastData!=NULL){
			vPortFree(lastData->buffer);
			vPortFree(lastData);
		}
		if(!ESP_WaitForIPD(10000)) return 0;
		lastData=ESP_RemoteReceive();
		lastData_idx=0;
	}
	if(SAVED_BYTES < max) max=SAVED_BYTES;
	memcpy(address, lastData->buffer+lastData_idx, max);
	lastData_idx+=max;
	return max;
}

#define CONNECTION_KEEPALIVE_S 	60UL
#define MQTT_ADDRESS			"iot-se2021.ddns.net"
#define MQTT_PORT				1883
#define MQTT_DEVICE_TOKEN		"SE2-BEST-RECORDS"

static void ScorePublisher_Task(void *pvParameters) {
	unsigned char buffer[128];
	MQTTTransport transporter;
	int result;
	int length;

	if (!NETWORK_Init()) {
		printf("Network fail\n");
		return;
	}

	// ESP8266 Transport Layer
	static transport_iofunctions_t iof = { NETWORK_Send, NETWORK_Recv };
	int transport_socket = transport_open(&iof);

	int state = STATE_INIT;
	TickType_t lastSend=xTaskGetTickCount();
	while (true) {
		switch (state) {
		case STATE_INIT:
			if (NETWORK_Connect(MQTT_ADDRESS, MQTT_PORT, CONNECTION_KEEPALIVE_S)) {
				state++;
			}
			break;
		case STATE_CONNECT:
			transporter.sck = &transport_socket;
			transporter.getfn = transport_getdatanb;
			transporter.state = 0;

			MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
			connectData.MQTTVersion = 3;
			connectData.username.cstring = MQTT_DEVICE_TOKEN;
			connectData.clientID.cstring = MQTT_DEVICE_TOKEN;
			connectData.keepAliveInterval = CONNECTION_KEEPALIVE_S * 2;
			length = MQTTSerialize_connect(buffer, sizeof(buffer), &connectData);
			// Send CONNECT to the mqtt broker.
			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {
				state++;
			} else {
				state = STATE_INIT;
			}
			break;
		case STATE_WAIT_CONNECT:
			// Wait for CONNACK response from the MQTT broker.
			while (true) {
				if ((result = MQTTPacket_readnb(buffer, sizeof(buffer), &transporter)) == CONNACK) {
					// Check if the connection was accepted.
					unsigned char sessionPresent, connack_rc;
					if ((MQTTDeserialize_connack(&sessionPresent, &connack_rc, buffer,
							sizeof(buffer)) != 1) || (connack_rc != 0)) {
						state = STATE_INIT;
						break;
					} else {
						state++;
						break;
					}
				} else if (result == -1) {
					state = STATE_INIT;
					break;
				}
			}
			break;
		case STATE_PUBLISH:
			;
			MQTTString topicString = MQTTString_initializer;
			topicString.cstring = "v1/devices/me/telemetry";
			unsigned char payload[64];
			length = sprintf(payload, "{\"group\":%d, \"score\":%d}", groupId, toPublish);
			toPublish++;
			length = MQTTSerialize_publish(buffer, sizeof(buffer), 0, 0, 0, 0, topicString, payload, length);
			// Send PUBLISH to the MQTT broker.
			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {
				vTaskDelayUntil(&lastSend, pdMS_TO_TICKS(10000));
			} else {
				state = STATE_INIT;
			}
			break;
		default:
			state = STATE_INIT;
		}
	}
}
