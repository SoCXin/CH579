/********************************** (C) COPYRIGHT *******************************
 * File Name          : cyclingsensor.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/11
 * Description        :            
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef CYCLINGSENSOR_H
#define CYCLINGSENSOR_H

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


// Cycling Sensor Task Events
#define START_DEVICE_EVT                              0x0001
#define CSC_PERIODIC_EVT                              0x0002
#define CSC_CONN_PARAM_UPDATE_EVT                     0x0004
#define CSC_NEGLECT_TIMEOUT_EVT                       0x0008
  
#define CSC_MEAS_LEN                            			11

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void CyclingSensor_Init( void );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 CyclingSensor_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* CYCLINGSENSOR_H */
