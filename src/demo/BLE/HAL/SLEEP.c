/********************************** (C) COPYRIGHT *******************************
* File Name          : SLEEP.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 睡眠配置及其初始化
*******************************************************************************/




/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"


/*******************************************************************************
 * @fn          CH57X_LowPower
 *
 * @brief       启动睡眠
 *
 * input parameters
 *
 * @param       time-唤醒的时间点（RTC绝对值）
 *
 * output parameters
 *
 * @param       
 *
 * @return      None.
 */
u32 CH57X_LowPower( u32 time )
{
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  u32 tmp,irq_status,sys_cfg;

  SYS_DisableAllIrq( &irq_status );
  tmp = RTC_GetCycle32k();
  if( time - tmp > 0x51F40 ){ // 检测睡眠的最长时间 10.24s,可以去掉
    SYS_RecoverIrq( irq_status );
    PRINT("! t1:%x %x...\n",time,tmp);
    return 1;
  }
  if( (time < tmp) || ((time - tmp) < WAKE_UP_RTC_MAX_TIME) ){ // 检测睡眠的最短时间
    SYS_RecoverIrq( irq_status );
    return 2;
  }    
  RTC_SetTignTime( time );
  SYS_RecoverIrq( irq_status );
#if( DEBUG == Debug_UART1 )
  while((R8_UART1_LSR&RB_LSR_TX_ALL_EMP)== 0 ) __nop();// 使用其他串口输出打印信息需要修改这行代码
#endif
// LOW POWER-sleep模式
  sys_cfg = R16_CLK_SYS_CFG;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  R16_CLK_SYS_CFG = RB_CLK_OSC32M_XT |0x08;		
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  R16_POWER_PLAN &= (RB_PWR_DCDC_EN|RB_PWR_DCDC_PRE);
  R16_POWER_PLAN |= RB_PWR_PLAN_EN\
                  |RB_PWR_MUST_0010\
                  |RB_PWR_MUST_1\
                  |RB_PWR_CORE\
                  |RB_PWR_RAM2K\
                  |RB_PWR_RAM14K\
                  |RB_PWR_EXTEND;
  R8_SAFE_ACCESS_SIG = 0;
  if( !RTCTigFlag ) __WFI();
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  R16_CLK_SYS_CFG = sys_cfg; //恢复系统主频配置值
  R8_SAFE_ACCESS_SIG = 0;
#endif
  return 0;
}

/*******************************************************************************
 * @fn          HAL_SleepInit
 *
 * @brief       配置睡眠唤醒的方式-RTC唤醒，触发模式
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       
 *
 * @return      None.
 */
void HAL_SleepInit( void )
{
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  R8_SLP_WAKE_CTRL  |= RB_SLP_RTC_WAKE; // RTC唤醒
  R8_RTC_MODE_CTRL  |= RB_RTC_TRIG_EN;	// 触发模式
  R8_SAFE_ACCESS_SIG = 0;						    // 
  NVIC_EnableIRQ(RTC_IRQn);	
  TMOS_SleepRegister( CH57X_LowPower );
#endif
}
