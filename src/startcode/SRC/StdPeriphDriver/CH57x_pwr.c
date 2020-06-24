/********************************** (C) COPYRIGHT *******************************
* File Name          : CH57x_pwr.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/15
* Description 
*******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : PWR_DCDCCfg
* Description    : 启用内部DC/DC电源，用于节约系统功耗
* Input          : s:  
                    ENABLE  - 打开DCDC电源
                    DISABLE - 关闭DCDC电源   				
* Return         : None
*******************************************************************************/
void PWR_DCDCCfg( UINT8 s )
{
    if(s == DISABLE)
    {		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R16_POWER_PLAN &= ~(RB_PWR_DCDC_EN|RB_PWR_DCDC_PRE);		// 旁路 DC/DC 
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
* Description    : 可控单元模块的电源控制
* Input          : s:  
                    ENABLE  - 打开   
                    DISABLE - 关闭
                   unit:
                    please refer to unit of controllable power supply 				
* Return         : None
*******************************************************************************/
void PWR_UnitModCfg( UINT8 s, UINT8 unit )
{
    if(s == DISABLE)		//关闭
    {
    	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        if(unit&UNIT_ETH_PHY)		R8_SLP_POWER_CTRL |= RB_SLP_ETH_PWR_DN;
        R8_HFCK_PWR_CTRL &= ~(unit&0x1c);
        R8_CK32K_CONFIG &= ~(unit&0x03);
    }
    else					//打开
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
* Description    : 外设时钟控制位
* Input          : s:  
                    ENABLE  - 打开外设时钟   
                    DISABLE - 关闭外设时钟
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
* Description    : 睡眠唤醒源配置
* Input          : s:  
                    ENABLE  - 打开此外设睡眠唤醒功能   
                    DISABLE - 关闭此外设睡眠唤醒功能
                   perph:
                    RB_SLP_USB_WAKE	    -  USB 为唤醒源
                    RB_SLP_ETH_WAKE	    -  ETH 为唤醒源
                    RB_SLP_RTC_WAKE	    -  RTC 为唤醒源
                    RB_SLP_GPIO_WAKE	-  GPIO 为唤醒源
                    RB_SLP_BAT_WAKE	    -  BAT 为唤醒源
                    ALL     -  以上所有
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
* Description    : 电源监控
* Input          : s:  
                    ENABLE  - 打开此功能   
                    DISABLE - 关闭此功能
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
* Description    : 低功耗-Idle模式
* Input          : None
* Return         : None
*******************************************************************************/
void LowPower_Idle( void )
{
    SCB -> SCR &= ~SCB_SCR_SLEEPDEEP_Msk;				// sleep
    __WFI();
}

/*******************************************************************************
* Function Name  : LowPower_Halt_1
* Description    : 低功耗-Halt_1模式。
                   此低功耗切到HSI/5时钟运行，唤醒后需要用户自己重新选择系统时钟源
* Input          : None
* Return         : None
*******************************************************************************/
void LowPower_Halt_1( void )
{
    UINT8  x32Kpw, x32Mpw;
    
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%额定电流
    if(R16_RTC_CNT_32K>0x3fff){     // 超过500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE驱动电流降低到额定电流
    }
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_SLP_POWER_CTRL &= ~RB_SLP_ROM_PWR_DN;		  // flash待机
    R8_BAT_DET_CTRL = 0;                              // 关闭电压监控
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R16_CLK_SYS_CFG = 5;		        // 降频 HSI/5=6.4M
    R8_SAFE_ACCESS_SIG = 0;
        
    SCB -> SCR |= SCB_SCR_SLEEPDEEP_Msk;				//deep sleep
    __WFI();
    R8_SAFE_ACCESS_SIG = 0;
 
    /* 开启电压监控 */
    PowerMonitor( ENABLE );    
}

/*******************************************************************************
* Function Name  : LowPower_Halt_2
* Description    : 低功耗-Halt_2模式。
                   此低功耗切到HSI/5时钟运行，唤醒后需要用户自己重新选择系统时钟源
* Input          : None
* Return         : None
*******************************************************************************/
void LowPower_Halt_2( void )
{
    UINT8  x32Kpw, x32Mpw;
    
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%额定电流
    if(R16_RTC_CNT_32K>0x3fff){     // 超过500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE驱动电流降低到额定电流
    }    
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_SLP_POWER_CTRL |= RB_SLP_ROM_PWR_DN;			  // flash停机
    R8_BAT_DET_CTRL = 0;                              // 关闭电压监控
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R16_CLK_SYS_CFG = 5;		        // 降频 HSI/5=6.4M
    R8_SAFE_ACCESS_SIG = 0;

    SCB -> SCR |= SCB_SCR_SLEEPDEEP_Msk;				//deep sleep
    __WFI();
    R8_SAFE_ACCESS_SIG = 0;

    /* 开启电压监控 */
    PowerMonitor( ENABLE );   
}

/*******************************************************************************
* Function Name  : LowPower_Sleep
* Description    : 低功耗-Sleep模式。
                   此低功耗切到HSI/5时钟运行，唤醒后需要用户自己重新选择系统时钟源
                   注意调用此函数，DCDC功能强制关闭，唤醒后可以手动再次打开
* Input          : rm:
                    RB_PWR_RAM2K	-	最后2K SRAM 供电
                    RB_PWR_RAM14K	-	0x20004000 - 0x20007800 14K SRAM 供电
                    RB_PWR_EXTEND	-	USB和RF 单元保留区域供电
                   NULL	-	以上单元都断电
* Return         : None
*******************************************************************************/
void LowPower_Sleep( UINT8 rm )
{
    UINT8  x32Kpw, x32Mpw;
    
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%额定电流
    if(R16_RTC_CNT_32K>0x3fff){     // 超过500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE驱动电流降低到额定电流
    } 
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_POWER_PLAN = RB_PWR_PLAN_EN		    \
                    |RB_PWR_MUST_0010		\
                    |RB_PWR_CORE            \
                    |rm;    
    R8_SAFE_ACCESS_SIG = 0;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_BAT_DET_CTRL = 0;                // 关闭电压监控
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R16_CLK_SYS_CFG = 5;		        // 降频 HSI/5=6.4M  
    R8_SAFE_ACCESS_SIG = 0;

	SCB -> SCR |= SCB_SCR_SLEEPDEEP_Msk;				//deep sleep
    __WFI();
    R8_SAFE_ACCESS_SIG = 0;

    /* 开启电压监控 */
    PowerMonitor( ENABLE );    
}

/*******************************************************************************
* Function Name  : LowPower_Shutdown
* Description    : 低功耗-Shutdown模式。
                   此低功耗切到HSI/5时钟运行，唤醒后需要用户自己重新选择系统时钟源
                   注意调用此函数，DCDC功能强制关闭，唤醒后可以手动再次打开
* Input          : rm:
                    RB_PWR_RAM2K	-	最后2K SRAM 供电
                   NULL	-	以上单元都断电
* Return         : None
*******************************************************************************/
void LowPower_Shutdown( UINT8 rm )
{	
    UINT8  x32Kpw, x32Mpw;
    
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%额定电流
    if(R16_RTC_CNT_32K>0x3fff){     // 超过500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE驱动电流降低到额定电流
    }
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_POWER_PLAN = RB_PWR_PLAN_EN		    \
                    |RB_PWR_MUST_0010		\
                    |rm;
    R8_SAFE_ACCESS_SIG = 0; 

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_BAT_DET_CTRL = 0;                // 关闭电压监控
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R16_CLK_SYS_CFG = 5;		        // 降频 HSI/5=6.4M    
    R8_SAFE_ACCESS_SIG = 0;    

	SCB -> SCR |= SCB_SCR_SLEEPDEEP_Msk;				//deep sleep
    __WFI();
    R8_SAFE_ACCESS_SIG = 0;

    /* 开启电压监控 */
    PowerMonitor( ENABLE );   
}

/*******************************************************************************
* Function Name  : EnterCodeUpgrade
* Description    : 跳入BOOT程序，准备代码升级
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
    if( t&0xFFFF )	t = t+0x10000;	

	R8_RTC_FLAG_CTRL = RB_RTC_TRIG_CLR|RB_RTC_TMR_CLR;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R32_RTC_TRIG = t;    
    R8_RTC_MODE_CTRL = 0x2f;    // 进入boot下载必要条件
    R8_SAFE_ACCESS_SIG = 0;
	
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_CLK_SYS_CFG = 5;		        // 降频 HSI/5=6.4M    
    R8_SAFE_ACCESS_SIG = 0;
    	
/* ready to BOOT */	
	__SEV();
    __WFE();
    __WFE();
    while(1);
}




