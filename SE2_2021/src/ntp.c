/*
 * ntp.c
 *
 *  Created on: 18/06/2021
 *      Author: ivogc
 */

#include "LPC17xx.h"
#include "esp8266.h"
#include <string.h>
//#include <arpa/inet.h>

#define DEFAULT_LI_VN_MODE 0xE3
#define DEFAULT_STRATUM 0
#define DEFAULT_POLL 6
#define DEFAULT_PRECISION 0xEC

#define EPOCH_1900_TO_1970 (-2208988800)

struct _ntp_packet{
 uint8_t li_vn_mode; // Eight bits. li, vn, and mode.
 // li. Two bits. Leap indicator.
// vn. Three bits. Version number of the protocol.
// mode. Three bits. Client will pick mode 3 for client.
 uint8_t stratum; // Eight bits. Stratum level of the local clock.
 uint8_t poll; // Eight bits. Maximum interval between successive messages.
 uint8_t precision; // Eight bits. Precision of the local clock.
 
 uint32_t rootDelay; // 32 bits. Total round trip delay time.
 uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
 uint32_t refId; // 32 bits. Reference clock identifier.
 
 uint32_t refTm_s; // 32 bits. Reference time-stamp seconds.
 uint32_t refTm_f; // 32 bits. Reference time-stamp fraction of a second.
 
 uint32_t origTm_s; // 32 bits. Originate time-stamp seconds.
 uint32_t origTm_f; // 32 bits. Originate time-stamp fraction of a second.
 
 uint32_t rxTm_s; // 32 bits. Received time-stamp seconds.
 uint32_t rxTm_f; // 32 bits. Received time-stamp fraction of a second.
 
 uint32_t txTm_s; // 32 bits. Transmit time-stamp seconds. Important
 uint32_t txTm_f; // 32 bits. Transmit time-stamp fraction of a second.
}; //48 bytes

uint32_t NTP_GetTime(){
    struct _ntp_packet data;
    data.li_vn_mode=DEFAULT_LI_VN_MODE;
    data.stratum=DEFAULT_STRATUM;
    data.poll=DEFAULT_POLL;
    data.precision=DEFAULT_PRECISION;
    if(!ESP_RemoteStart("UDP", "pool.ntp.org", 123, 0)) return 0;
    if(!ESP_RemoteSend((char*)&data, sizeof(data))) return 0;
    if(!ESP_WaitForIPD(15000)) return 0;
    ESP_DATA* rec=ESP_RemoteReceive();
    if(sizeof(data)!=rec->buff_size) return 0;
    memcpy(&data, rec->buffer, sizeof(data));
    return __builtin_bswap32(data.txTm_s)+EPOCH_1900_TO_1970;
}


