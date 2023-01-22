/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_common.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/12/05
 * Description        : 
 ********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_COMM_H__
#define __CH57x_COMM_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define  NULL			0
#define  ALL			0xFFFF

#ifndef	 SUCCESS	 	 
#define  SUCCESS		0		
#define  FAILED			(!SUCCESS)
#endif

#ifndef	 ENABLE	 
#define  DISABLE		0		
#define  ENABLE			(!DISABLE)	 
#endif

#define Debug_UART0        0
#define Debug_UART1        1
#define Debug_UART2        2
#define Debug_UART3        3
 
#ifdef DEBUG
#include <stdio.h>
#endif
   
#ifndef	 FREQ_SYS  
#define  FREQ_SYS		32000000   
#endif   

/**
 * @brief  32Kʱ�ӣ�Hz��
 */
#ifdef CLK_OSC32K
#if ( CLK_OSC32K == 1 )
#define CAB_LSIFQ       32000
#else
#define CAB_LSIFQ       32768
#endif
#else
#define CAB_LSIFQ       32000
#endif

#include <string.h>
#include "CH57x_clk.h"
#include "CH57x_uart.h"
#include "CH57x_gpio.h"
#include "CH57x_lcd.h"
#include "CH57x_flash.h"
#include "CH57x_pwr.h"
#include "CH57x_pwm.h"	 
#include "CH57x_adc.h"
#include "CH57x_sys.h"
#include "CH57x_timer.h"	 
#include "CH57x_spi.h"
#include "CH57x_usbdev.h"
#include "CH57x_usbhost.h"




  
#define DelayMs(x)      mDelaymS(x)	  
#define DelayUs(x)      mDelayuS(x)	  


#ifdef __cplusplus
}
#endif

#endif  // __CH57x_COMM_H__	 

