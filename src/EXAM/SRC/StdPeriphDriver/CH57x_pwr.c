/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_pwr.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : PWR_DCDCCfg
* Description    : �����ڲ�DC/DC��Դ�����ڽ�Լϵͳ����
* Input          : s:  
                    ENABLE  - ��DCDC��Դ
                    DISABLE - �ر�DCDC��Դ   				
* Return         : None
*******************************************************************************/
void PWR_DCDCCfg( UINT8 s )
{
    if(s == DISABLE)
    {		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R16_POWER_PLAN &= ~(RB_PWR_DCDC_EN|RB_PWR_DCDC_PRE);		// ��· DC/DC 
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG0;		
    }
    else
    {
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R16_POWER_PLAN |= RB_PWR_DCDC_PRE;
        DelayUs(10);
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R16_POWER_PLAN |= RB_PWR_DCDC_EN;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG0;			
    }
}

/*******************************************************************************
* Function Name  : PWR_UnitModCfg
* Description    : �ɿص�Ԫģ��ĵ�Դ����
* Input          : s:  
                    ENABLE  - ��   
                    DISABLE - �ر�
                   unit:
                    please refer to unit of controllable power supply 				
* Return         : None
*******************************************************************************/
void PWR_UnitModCfg( UINT8 s, UINT8 unit )
{
    if(s == DISABLE)		//�ر�
    {
    	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        if(unit&UNIT_ETH_PHY)		R8_SLP_POWER_CTRL |= RB_SLP_ETH_PWR_DN;
        R8_HFCK_PWR_CTRL &= ~(unit&0x1c);
        R8_CK32K_CONFIG &= ~(unit&0x03);
    }
    else					//��
    {
    	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        if(unit&UNIT_ETH_PHY)		R8_SLP_POWER_CTRL &= ~RB_SLP_ETH_PWR_DN;
        R8_HFCK_PWR_CTRL |= (unit&0x1c);
        R8_CK32K_CONFIG |= (unit&0x03);
    }
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : PWR_PeriphClkCfg
* Description    : ����ʱ�ӿ���λ
* Input          : s:  
                    ENABLE  - ������ʱ��   
                    DISABLE - �ر�����ʱ��
                   perph:
                    please refer to Peripher CLK control bit define						
* Return         : None
*******************************************************************************/
void PWR_PeriphClkCfg( UINT8 s, UINT16 perph )
{
    if( s == DISABLE )
    {
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R32_SLEEP_CONTROL |= perph;
    }
    else
    {
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R32_SLEEP_CONTROL &= ~perph;
    }
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : PWR_PeriphWakeUpCfg
* Description    : ˯�߻���Դ����
* Input          : s:  
                    ENABLE  - �򿪴�����˯�߻��ѹ���   
                    DISABLE - �رմ�����˯�߻��ѹ���
                   perph:
                    RB_SLP_USB_WAKE	    -  USB Ϊ����Դ
                    RB_SLP_ETH_WAKE	    -  ETH Ϊ����Դ
                    RB_SLP_RTC_WAKE	    -  RTC Ϊ����Դ
                    RB_SLP_GPIO_WAKE	-  GPIO Ϊ����Դ
                    RB_SLP_BAT_WAKE	    -  BAT Ϊ����Դ
                    ALL     -  ��������
* Return         : None
*******************************************************************************/
void PWR_PeriphWakeUpCfg( UINT8 s, UINT16 perph )
{
    if( s == DISABLE )
    {
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R8_SLP_WAKE_CTRL &= ~perph;		
    }
    else
    {
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R8_SLP_WAKE_CTRL |= perph;
    }
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : PowerMonitor
* Description    : ��Դ���
* Input          : s:  
                    ENABLE  - �򿪴˹���   
                    DISABLE - �رմ˹���
* Return         : None
*******************************************************************************/
void PowerMonitor( UINT8 s )
{
    if( s == DISABLE )
    {
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R8_BAT_DET_CTRL = 0;
        R8_SAFE_ACCESS_SIG = 0; 
    }
    else
    {
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R8_BAT_DET_CFG = 1;                     // 2.05V - 2.33V
        R8_BAT_DET_CTRL = RB_BAT_DET_EN;
        R8_SAFE_ACCESS_SIG = 0; 
        mDelayuS(1); 	
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R8_BAT_DET_CTRL = RB_BAT_LOW_IE|RB_BAT_LOWER_IE|RB_BAT_DET_EN;  
        R8_SAFE_ACCESS_SIG = 0;  
    }   
}

/*******************************************************************************
* Function Name  : LowPower_Idle
* Description    : �͹���-Idleģʽ
* Input          : None
* Return         : None
*******************************************************************************/
void LowPower_Idle( void )
{
    SCB -> SCR &= ~SCB_SCR_SLEEPDEEP_Msk;				// sleep
    __WFI();
}

/*******************************************************************************
* Function Name  : EnterCodeUpgrade
* Description    : ����BOOT����׼����������
* Input          : None
* Return         : None
*******************************************************************************/
void EnterCodeUpgrade( void )
{	
/* RTC wakeup */	
    UINT32 t;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE;
    R16_POWER_PLAN = RB_PWR_PLAN_EN		    \
                    |RB_PWR_MUST_0010;
    R8_SAFE_ACCESS_SIG = 0; 

	do{
    	t = R32_RTC_CNT_32K;
    }while( t != R32_RTC_CNT_32K );
    
    t = t + 10;
    if( t>0xA8C00000 )	t -= 0xA8C00000;	 
    if( t&0xFFFF )	t = t+0x10000;	

	R8_RTC_FLAG_CTRL = RB_RTC_TRIG_CLR|RB_RTC_TMR_CLR;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R32_RTC_TRIG = t;    
    R8_RTC_MODE_CTRL = 0x2f;    // ����boot���ر�Ҫ����
    R8_SAFE_ACCESS_SIG = 0;
	
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_CLK_SYS_CFG = 5;		        // ��Ƶ HSI/5=6.4M    
    R8_SAFE_ACCESS_SIG = 0;
    	
/* ready to BOOT */	
	__SEV();
    __WFE();
    __WFE();
    while(1);
}




