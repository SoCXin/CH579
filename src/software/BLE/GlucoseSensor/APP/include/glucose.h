/********************************** (C) COPYRIGHT *******************************
* File Name          : glucose.h
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/12
* Description        : 
            
*******************************************************************************/

#ifndef GLUCOSE_H
#define GLUCOSE_H

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

// Glucose Task Events
#define START_DEVICE_EVT                              0x0001
#define NOTI_TIMEOUT_EVT                              0x0002

// Message event  
#define CTL_PNT_MSG                                   0xE0
  
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */
  
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Task Initialization for the Glucose Application
 */
extern void Glucose_Init( uint8 task_id );

/*
 * Task Event Processor for the Glucose Application
 */
extern uint16 Glucose_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif 
