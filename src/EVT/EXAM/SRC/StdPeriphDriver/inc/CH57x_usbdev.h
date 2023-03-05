/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_usbdev.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/12/05
 * Description        : 
 ********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/


#ifndef __CH57x_USBDEV_H__
#define __CH57x_USBDEV_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "CH579SFR.h"
#include "core_cm0.h"

/* ���»�������USBģ���շ�ʹ�õ����ݻ��������ܹ�9��ͨ����9�黺�棩���û��ɸ���ʵ��ʹ�õ�ͨ����������Ӧ������ */
extern PUINT8  pEP0_RAM_Addr;		//ep0(64)+ep4_out(64)+ep4_in(64)
extern PUINT8  pEP1_RAM_Addr;		//ep1_out(64)+ep1_in(64)
extern PUINT8  pEP2_RAM_Addr;		//ep2_out(64)+ep2_in(64)
extern PUINT8  pEP3_RAM_Addr;		//ep3_out(64)+ep3_in(64)

#define	pSetupReqPak		((PUSB_SETUP_REQ)pEP0_RAM_Addr)
#define pEP0_DataBuf		(pEP0_RAM_Addr)
#define pEP1_OUT_DataBuf	(pEP1_RAM_Addr)
#define pEP1_IN_DataBuf		(pEP1_RAM_Addr+64)
#define pEP2_OUT_DataBuf	(pEP2_RAM_Addr)
#define pEP2_IN_DataBuf		(pEP2_RAM_Addr+64)
#define pEP3_OUT_DataBuf	(pEP3_RAM_Addr)
#define pEP3_IN_DataBuf		(pEP3_RAM_Addr+64)
#define pEP4_OUT_DataBuf	(pEP0_RAM_Addr+64)
#define pEP4_IN_DataBuf		(pEP0_RAM_Addr+128)	 
	 

	 
void USB_DeviceInit( void );			/* USB�豸���ܳ�ʼ����4���˵㣬8��ͨ�� */	 
void USB_DevTransProcess( void );		/* USB�豸Ӧ���䴦�� */	 
	 
void DevEP1_OUT_Deal( UINT8 l );		/* �豸�˵�1�´�ͨ������ */
void DevEP2_OUT_Deal( UINT8 l );		/* �豸�˵�2�´�ͨ������ */
void DevEP3_OUT_Deal( UINT8 l );		/* �豸�˵�3�´�ͨ������ */
void DevEP4_OUT_Deal( UINT8 l );		/* �豸�˵�4�´�ͨ������ */

void DevEP1_IN_Deal( UINT8 l );		/* �豸�˵�1�ϴ�ͨ������ */
void DevEP2_IN_Deal( UINT8 l );		/* �豸�˵�2�ϴ�ͨ������ */
void DevEP3_IN_Deal( UINT8 l );		/* �豸�˵�3�ϴ�ͨ������ */
void DevEP4_IN_Deal( UINT8 l );		/* �豸�˵�4�ϴ�ͨ������ */

// 0-δ���  (!0)-�����
#define EP1_GetINSta()		(R8_UEP1_CTRL&UEP_T_RES_NAK)		/* ��ѯ�˵�1�Ƿ��ϴ���� */
#define EP2_GetINSta()		(R8_UEP2_CTRL&UEP_T_RES_NAK)		/* ��ѯ�˵�2�Ƿ��ϴ���� */
#define EP3_GetINSta()		(R8_UEP3_CTRL&UEP_T_RES_NAK)		/* ��ѯ�˵�3�Ƿ��ϴ���� */
#define EP4_GetINSta()		(R8_UEP4_CTRL&UEP_T_RES_NAK)		/* ��ѯ�˵�4�Ƿ��ϴ���� */


	 
#ifdef __cplusplus
}
#endif

#endif  // __CH57x_USBDEV_H__	

