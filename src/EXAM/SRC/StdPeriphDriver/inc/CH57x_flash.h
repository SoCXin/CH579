/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_flash.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/12/05
 * Description        : 
 ********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/


#ifndef __CH57x_FLASH_H__
#define __CH57x_FLASH_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "CH579SFR.h"
#include "core_cm0.h"
     
#define ROM_UUID_ADDR        0x40608         // chip UID address ( ID-48bit + CKS-16bit )
#define ROM_MAC_ADDR         0x40608         // NET MAC address 48bit

// ��ȡΨһID
void GetUniqueID(PUINT8 buf);                                           /* ��ȡоƬΨһID��С��ģʽ��6B-ID�� 2B-CKS */    
     
// ��ȡ����MAC
void GetMACAddress(PUINT8 buf);                                         /* ��ȡ����MAC��С��ģʽ��6B-MAC */   


UINT8 FlashBlockErase(UINT32 addr);     
UINT8 FlashWriteDW(UINT32 addr, UINT32 dat);
UINT8 FlashWriteBuf(UINT32 addr, PUINT32 pdat, UINT16 len);	 

	 
	 
#ifdef __cplusplus
}
#endif

#endif  // __CH57x_FLASH_H__	

