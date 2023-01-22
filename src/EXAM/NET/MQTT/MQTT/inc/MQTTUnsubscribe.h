/********************************** (C) COPYRIGHT *******************************
 * File Name          : MQTTUnsubscribe.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2023/01/13
 * Description        : 
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef MQTTUNSUBSCRIBE_H_
#define MQTTUNSUBSCRIBE_H_

#if !defined(DLLImport)
  #define DLLImport 
#endif
#if !defined(DLLExport)
  #define DLLExport
#endif

DLLExport int MQTTSerialize_unsubscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid,
		int count, MQTTString topicFilters[]);

DLLExport int MQTTDeserialize_unsubscribe(unsigned char* dup, unsigned short* packetid, int max_count, int* count, MQTTString topicFilters[],
		unsigned char* buf, int len);

DLLExport int MQTTSerialize_unsuback(unsigned char* buf, int buflen, unsigned short packetid);

DLLExport int MQTTDeserialize_unsuback(unsigned short* packetid, unsigned char* buf, int len);

#endif /* MQTTUNSUBSCRIBE_H_ */
