/********************************** (C) COPYRIGHT *******************************
 * File Name          : KEY.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2016/04/12
 * Description        : 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/



/******************************************************************************/
#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C"
{
#endif

	


/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/
#define HAL_KEY_POLLING_VALUE   	   100

/* Switches (keys) */
#define HAL_KEY_SW_1 0x01  // key1
#define HAL_KEY_SW_2 0x02  // key2
#define HAL_KEY_SW_3 0x04  // key3
#define HAL_KEY_SW_4 0x08  // key4
  
 /* �������� */

/* 1 - KEY */
  #define KEY1_BV           BV(22)
  #define KEY2_BV           BV(4)
  #define KEY3_BV           
  #define KEY4_BV           

  #define KEY1_PU        	(R32_PB_PU |= KEY1_BV)
  #define KEY2_PU        	(R32_PB_PU |= KEY2_BV)
  #define KEY3_PU        	()
  #define KEY4_PU        	()

  #define KEY1_DIR       	(R32_PB_DIR &= ~KEY1_BV)
  #define KEY2_DIR        (R32_PB_DIR &= ~KEY2_BV)
  #define KEY3_DIR        ()
  #define KEY4_DIR        ()

  #define KEY1_IN        	(ACTIVE_LOW(R32_PB_PIN&KEY1_BV))
  #define KEY2_IN        	(ACTIVE_LOW(R32_PB_PIN&KEY2_BV))
  #define KEY3_IN        	()
  #define KEY4_IN        	()

#define HAL_PUSH_BUTTON1()        ( KEY1_IN ) //����Զ��尴��
#define HAL_PUSH_BUTTON2()        ( KEY2_IN )
#define HAL_PUSH_BUTTON3()        ( 0 ) 
#define HAL_PUSH_BUTTON4()        ( 0 ) 






/**************************************************************************************************
 * TYPEDEFS
 **************************************************************************************************/
typedef void (*halKeyCBack_t) (uint8 keys );

typedef struct
{
  uint8  keys;  // keys
} keyChange_t;

/**************************************************************************************************
 *                                             GLOBAL VARIABLES
 ******************* *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Initialize the Key Service
 */
void HAL_KeyInit( void );

/*
 * This is for internal used by hal_driver
 */
void HAL_KeyPoll( void );

/*
 * Configure the Key Service
 */
void HalKeyConfig( const halKeyCBack_t cback);

/*
 * Read the Key callback
 */
void HalKeyCallback ( uint8 keys );

/*
 * Read the Key status
 */
uint8 HalKeyRead( void);

/**************************************************************************************************
******************* *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
