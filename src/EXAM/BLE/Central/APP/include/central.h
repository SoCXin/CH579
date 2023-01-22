/********************************** (C) COPYRIGHT *******************************
 * File Name          : central.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/11/12
 * Description        : �۲�Ӧ��������������ϵͳ��ʼ��
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef CENTRAL_H
#define CENTRAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */


// Simple BLE Observer Task Events
#define START_DEVICE_EVT                              0x0001
#define START_DISCOVERY_EVT                           0x0002
#define START_SCAN_EVT                                0x0004
#define START_SVC_DISCOVERY_EVT                       0x0008
#define START_PARAM_UPDATE_EVT                        0x0010
#define START_READ_OR_WRITE_EVT                       0x0020
#define START_WRITE_CCCD_EVT                          0x0040
#define START_READ_RSSI_EVT                           0x0080
#define ESTABLISH_LINK_TIMEOUT_EVT                    0x0100

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void Central_Init( void );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 Central_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif 
