/********************************** (C) COPYRIGHT *******************************
 * File Name          : multiCentral.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/11/12
 * Description        : 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef MULTICENTRAL_H
#define MULTICENTRAL_H

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
#define START_READ_RSSI_EVT                           0x0040
#define ESTABLISH_LINK_TIMEOUT_EVT                    0x0080
#define START_WRITE_CCCD_EVT                          0x0100

#define CONNECT0_ITEM                                 0
#define CONNECT1_ITEM                                 1
#define CONNECT2_ITEM                                 2

/*********************************************************************
 * MACROS
 */
typedef struct
{
  uint8  taskID;                // Task ID for internal task/event processing
  uint16  connHandle;           // Connection handle of current connection
  uint8  state;                 // Application state
  uint8  peerAddr[B_ADDR_LEN];
  uint8  discState;             // Discovery state
  uint8  procedureInProgress;   // GATT read/write procedure state
  uint16  svcStartHdl;          // Discovered service start handle
  uint16  svcEndHdl;            // Discovered service end handle
  uint16  charHdl;              // Discovered characteristic handle
  uint16_t cccHdl;              // client characteristic configuration discovery handle
} centralConnItem_t;

typedef struct
{
  uint8  peerAddr[B_ADDR_LEN];
} peerAddrDefItem_t;

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

#endif /* MULTICENTRAL_H */
