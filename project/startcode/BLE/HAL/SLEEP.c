/********************************** (C) COPYRIGHT *******************************
* File Name          : SLEEP.c
* Author             : WCH
* Version            : V1.1
* Date               : 2019/11/05
* Description        : ˯�����ü����ʼ��
*******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CH57x_common.h"
#include "HAL.h"


/*******************************************************************************
 * @fn          CH57X_LowPower
 *
 * @brief       ����˯��
 *
 * input parameters
 *
 * @param       time-���ѵ�ʱ��㣨RTC����ֵ��
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
  u32 tmp,irq_status;

  SYS_DisableAllIrq( &irq_status );
  tmp = RTC_GetCycle32k();
  if( (time < tmp) || ((time - tmp) < 30) ){ // ���˯�ߵ����ʱ��
    SYS_RecoverIrq( irq_status );
    return 2;
  }    
  RTC_SetTignTime( time );
  SYS_RecoverIrq( irq_status );
#if( DEBUG == Debug_UART1 )
  while((R8_UART1_LSR&RB_LSR_TX_ALL_EMP)== 0 ) __nop();// ʹ���������������ӡ��Ϣ��Ҫ�޸����д���
#endif
// LOW POWER-sleepģʽ
  if( !RTCTigFlag ){
		LowPower_Sleep(RB_PWR_RAM2K|RB_PWR_RAM14K|RB_PWR_EXTEND );
		SetSysClock( CLK_SOURCE_HSI_32MHz );
    HSECFG_Current( HSE_RCur_100 );     // ��Ϊ�����(�͹��ĺ�����������HSEƫ�õ���)
    RTC_SetTignTime( time + 47 );
    LowPower_Idle();
		SetSysClock( CLK_SOURCE_HSE_32MHz );
  }
  else{
    return 3;
  }
#endif
  return 0;
}

/*******************************************************************************
 * @fn          HAL_SleepInit
 *
 * @brief       ����˯�߻��ѵķ�ʽ-RTC���ѣ�����ģʽ
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
  R8_SLP_WAKE_CTRL  |= RB_SLP_RTC_WAKE; // RTC����
  R8_RTC_MODE_CTRL  |= RB_RTC_TRIG_EN;	// ����ģʽ
  R8_SAFE_ACCESS_SIG = 0;						    // 
  NVIC_EnableIRQ(RTC_IRQn);	
#endif
}