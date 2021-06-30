/*
 * dummypub.c
 *
 *  Created on: 2 Jun 2020
 *      Author: psampaio
 */

#include "LPC17xx.h"
#include "MQTTPacket.h"
#include "transport.h"
#include "esp8266.h"
#include "FreeRTOS.h"
#include "task.h"
#include "MQTTConnect.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"
#include "MQTTFormat.h"

#define groupId 11
#define score 72

enum {STATE_INIT, STATE_CONNECT, STATE_WAIT_CONNECT, STATE_PUBLISH};

static void TemperaturePublisher_Task(void *pvParameters);

bool ScorePublisher_Init(){
	return xTaskCreate(TemperaturePublisher_Task, "MQTT", configMINIMAL_STACK_SIZE, NULL, 1, NULL)==pdPASS;
}

bool NETWORK_Init(void) {
	return true;
}

bool NETWORK_Connect(const char *host, const unsigned short int port,
		const unsigned short int keepalive) {
	return ESP_RemoteStart("TCP", host, port, keepalive);
}

int NETWORK_Send(unsigned char *address, unsigned int bytes) {
	return ESP_RemoteSend(address, bytes)?bytes:0;
}

int NETWORK_Recv(unsigned char *address, unsigned int maxbytes) {
	if(!ESP_WaitForIPD(10000)) return false;
	ESP_DATA* data=ESP_RemoteReceive();
	unsigned int max=maxbytes;
	if(data->buff_size < max) max=data->buff_size;
	memcpy(address, data->buffer, max);
	return max;
}

#define CONNECTION_KEEPALIVE_S 	60UL
#define MQTT_ADDRESS			"iot-se2021.ddns.net"
#define MQTT_PORT				1883
#define MQTT_DEVICE_TOKEN		"SE2-BEST-RECORDS"

static void TemperaturePublisher_Task(void *pvParameters) {
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
			length = sprintf(payload, "{\"group\":%d, \"score\":%d}", groupId, score);
			length = MQTTSerialize_publish(buffer, sizeof(buffer), 0, 0, 0, 0, topicString, payload, length);
			// Send PUBLISH to the MQTT broker.
			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {
				// Wait for new data.
			} else {
				state = STATE_INIT;
			}
			break;
		default:
			state = STATE_INIT;
		}
	}
}
