/********************************** (C) COPYRIGHT *******************************
 * File Name          : MQTTPublish.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2023/01/13
 * Description        : 
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef MQTTPUBLISH_H_
#define MQTTPUBLISH_H_

#if !defined(DLLImport)
  #define DLLImport 
#endif
#if !defined(DLLExport)
  #define DLLExport
#endif

DLLExport int MQTTSerialize_publish(unsigned char* buf, int buflen, unsigned char dup, int qos, unsigned char retained, unsigned short packetid,
		MQTTString topicName, unsigned char* payload, int payloadlen);

DLLExport int MQTTDeserialize_publish(unsigned char* dup, int* qos, unsigned char* retained, unsigned short* packetid, MQTTString* topicName,
		unsigned char** payload, int* payloadlen, unsigned char* buf, int len);

DLLExport int MQTTSerialize_puback(unsigned char* buf, int buflen, unsigned short packetid);
DLLExport int MQTTSerialize_pubrel(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid);
DLLExport int MQTTSerialize_pubcomp(unsigned char* buf, int buflen, unsigned short packetid);
DLLExport int MQTTSerialize_pubrec(unsigned char* buf, int buflen, unsigned short packetid);
#endif /* MQTTPUBLISH_H_ */
