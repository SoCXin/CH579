/********************************** (C) COPYRIGHT *******************************
* File Name          : RTC.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/06
* Description        : RTC配置及其初始化
*******************************************************************************/




/******************************************************************************/
/* 头文件包含 */
#include "CH57x_common.h"
#include "HAL.h"


/***************************************************
 * Global variables
 */
u32V RTCTigFlag;


/*******************************************************************************
* Function Name  : RTC_SetTignTime
* Description    : 配置RTC触发时间
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
 * @brief       RTC中断处理
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
 * @brief       系统定时器初始化
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
#if( CLK_OSC32K_RC )
  HSE_Calibration_LSI();
  TMOS_Rc32KRegister( HSE_Calibration_LSI ); // 注册内部32K时钟校准，并配置为使用内部时钟作为32K时钟源
#endif
#if( CLK_OSC32K_XT )
  R8_SAFE_ACCESS_SIG = 0x57; 
  R8_SAFE_ACCESS_SIG = 0xa8;
  R8_CK32K_CONFIG    |= RB_CLK_OSC32K_XT | RB_CLK_INT32K_PON | RB_CLK_XT32K_PON;
  R8_SAFE_ACCESS_SIG = 0;
#endif
}

/******************************** endfile @ time ******************************/
