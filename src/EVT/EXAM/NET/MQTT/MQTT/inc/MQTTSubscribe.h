/********************************** (C) COPYRIGHT *******************************
 * File Name          : MQTTSubscribe.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2023/01/13
 * Description        : 
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef MQTTSUBSCRIBE_H_
#define MQTTSUBSCRIBE_H_

#if !defined(DLLImport)
  #define DLLImport 
#endif
#if !defined(DLLExport)
  #define DLLExport
#endif

DLLExport int MQTTSerialize_subscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid,
		int count, MQTTString topicFilters[], int requestedQoSs[]);

DLLExport int MQTTDeserialize_subscribe(unsigned char* dup, unsigned short* packetid,
		int maxcount, int* count, MQTTString topicFilters[], int requestedQoSs[], unsigned char* buf, int len);

DLLExport int MQTTSerialize_suback(unsigned char* buf, int buflen, unsigned short packetid, int count, int* grantedQoSs);

DLLExport int MQTTDeserialize_suback(unsigned short* packetid, int maxcount, int* count, int grantedQoSs[], unsigned char* buf, int len);


#endif /* MQTTSUBSCRIBE_H_ */
