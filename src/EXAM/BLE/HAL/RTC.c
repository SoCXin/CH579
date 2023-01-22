/********************************** (C) COPYRIGHT *******************************
 * File Name          : RTC.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2019/11/05
 * Description        : RTC���ü����ʼ��
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CH57x_common.h"
#include "HAL.h"

/*********************************************************************
 * CONSTANTS
 */

/***************************************************
 * Global variables
 */
u32V RTCTigFlag;

/*******************************************************************************
* Function Name  : RTC_SetTignTime
* Description    : ����RTC����ʱ��
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_SetTignTime( u32 time )
{
	if( time&0xffff ) time += (1<<16); 
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
	R32_RTC_TRIG       = time;
	RTCTigFlag = 0;
}

/*******************************************************************************
 * @fn          RTC_IRQHandler
 *
 * @brief       RTC�жϴ���
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void RTC_IRQHandler( void )
{
	R8_RTC_FLAG_CTRL =(RB_RTC_TMR_CLR|RB_RTC_TRIG_CLR);
	RTCTigFlag = 1;
}

/*******************************************************************************
 * @fn          HAL_Time0Init
 *
 * @brief       ϵͳ��ʱ����ʼ��
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HAL_TimeInit( void )
{
#if( CLK_OSC32K )
  R8_SAFE_ACCESS_SIG = 0x57;
  R8_SAFE_ACCESS_SIG = 0xa8;
  R8_CK32K_CONFIG &= ~(RB_CLK_OSC32K_XT|RB_CLK_XT32K_PON);
  R8_CK32K_CONFIG |= RB_CLK_INT32K_PON;
  R8_SAFE_ACCESS_SIG = 0;
  Calibration_LSI();
#else
  R8_SAFE_ACCESS_SIG = 0x57; 
  R8_SAFE_ACCESS_SIG = 0xa8;
  R8_CK32K_CONFIG    |= RB_CLK_OSC32K_XT | RB_CLK_INT32K_PON | RB_CLK_XT32K_PON;
  R8_SAFE_ACCESS_SIG = 0;
#endif
  RTC_InitTime( 2020, 1, 1, 0, 0, 0 );    //RTCʱ�ӳ�ʼ����ǰʱ��
  TMOS_TimerInit( 0 );
}

/******************************** endfile @ time ******************************/
