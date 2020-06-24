/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : WCH
* Version            : V1.2
* Date               : 2020/05/27
* Description 		 : 系统睡眠模式并唤醒演示：GPIOA_6和GPIOA_5作为唤醒源，共6种睡眠等级
*******************************************************************************/

/* 
注意：使用低功耗（Idle模式除外）睡眠唤醒后，需要重新配置系统时钟，否则使用的是内部32M时钟5分频，6.4M主频 
切换到外部HSE直接做主频或者间接方式（送入PLL再通过PLL分频做主频），唤醒后，延时1.2ms（一般），再切换用于HSE起振稳定。
切换到内部HSI直接做主频或者间接方式（送入PLL再通过PLL分频做主频），唤醒后，无需等待时间。

注意：切换到HSE时钟源，所需等待稳定时间和选择的外置晶体参数有关，选择一款新的晶体最好阅读厂家提供的晶体及其
负载电容参数值。通过配置R8_XT32M_TUNE寄存器，可以配置不同的负载电容和偏置电流，调整晶体稳定时间。
*/

#include "CH57x_common.h"

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{
    DelayMs(2); 
    SetSysClock( CLK_SOURCE_HSE_32MHz );            // 设置外部32M做主频
    GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
    GPIOB_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
    
    
/* 配置串口调试 */   
    DebugInit();
    PRINT( "Start @ChipID=%02x\n", R8_CHIP_ID );
    DelayMs(200); 

#if 1    
    /* 配置唤醒源为 GPIO - PA6&PA5 */
    GPIOA_ModeCfg( GPIO_Pin_6|GPIO_Pin_5, GPIO_ModeIN_PU );
    GPIOA_ITModeCfg( GPIO_Pin_6|GPIO_Pin_5, GPIO_ITMode_FallEdge );        // 下降沿唤醒
    NVIC_EnableIRQ( GPIO_IRQn );
    PWR_PeriphWakeUpCfg( ENABLE, RB_SLP_GPIO_WAKE );
#endif    
    
#if 1
    PRINT( "IDLE mode sleep \n");   
    DelayMs(1);
    LowPower_Idle();
    PRINT( "wake.. \n"); 
    DelayMs(500);
#endif    
    
#if 1
    PRINT( "Halt_1 mode sleep \n");   
    DelayMs(1);
    LowPower_Halt_1();
/*
使用HSI/5=6.4M睡眠，唤醒时间大概需要 2048Tsys≈330us
HSE起振一般不超过1.2ms(500us-1200us)，所以切换到外部HSE，需要 1.2ms-330us 这个时间可以保证HSE足够稳定，一般用于蓝牙
DelayUs()函数时基于32M时钟的书写，此时主频为6.4M，所以 DelayUs((1200-330)/5)    
*/
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_PON)) {     // 是否HSE上电
        PWR_UnitModCfg( ENABLE, UNIT_SYS_HSE );   // HSE上电
        DelayUs((1200)/5);
    }
    else if(!(R16_CLK_SYS_CFG&RB_CLK_OSC32M_XT)){   // 是否选择 HSI/5 做时钟源
        DelayUs((1200-330)/5);
    } 
    HSECFG_Current( HSE_RCur_100 );     // 降为额定电流(低功耗函数中提升了HSE偏置电流)
    DelayUs(5/5);                       // 等待稳定 1-5us
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PRINT( "wake.. \n"); 
    DelayMs(500);    
#endif    
    
#if 1
    PRINT( "Halt_2 mode sleep \n");   
    DelayMs(1);
    LowPower_Halt_2();
/*
使用HSI/5=6.4M睡眠，唤醒时间大概需要 2048Tsys≈330us
HSE起振一般不超过1.2ms(500us-1200us)，所以切换到外部HSE，需要 1.2ms-330us 这个时间可以保证HSE足够稳定，一般用于蓝牙
DelayUs()函数时基于32M时钟的书写，此时主频为6.4M，所以 DelayUs((1200-330)/5)    
*/
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_PON)) {     // 是否HSE上电
        PWR_UnitModCfg( ENABLE, UNIT_SYS_HSE );   // HSE上电
        DelayUs((1200)/5);
    }
    else if(!(R16_CLK_SYS_CFG&RB_CLK_OSC32M_XT)){   // 是否选择 HSI/5 做时钟源
        DelayUs((1200-330)/5);
    } 
    HSECFG_Current( HSE_RCur_100 );     // 降为额定电流(低功耗函数中提升了HSE偏置电流)
    DelayUs(5/5);                       // 等待稳定 1-5us
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PRINT( "wake.. \n"); 
    DelayMs(500);    
#endif    

#if 1
    PRINT( "sleep mode sleep \n");   
    DelayMs(1);
    LowPower_Sleep( RB_PWR_RAM14K|RB_PWR_RAM2K );       //只保留14+2K SRAM 供电
/*
使用HSI/5=6.4M睡眠，唤醒时间大概需要 2048Tsys≈330us
HSE起振一般不超过1.2ms(500us-1200us)，所以切换到外部HSE，需要 1.2ms-330us 这个时间可以保证HSE足够稳定，一般用于蓝牙
DelayUs()函数时基于32M时钟的书写，此时主频为6.4M，所以 DelayUs((1200-330)/5)    
*/
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_PON)) {     // 是否HSE上电
        PWR_UnitModCfg( ENABLE, UNIT_SYS_HSE );   // HSE上电
        DelayUs((1200)/5);
    }
    else if(!(R16_CLK_SYS_CFG&RB_CLK_OSC32M_XT)){   // 是否选择 HSI/5 做时钟源
        DelayUs((1200-330)/5);
    } 
    HSECFG_Current( HSE_RCur_100 );     // 降为额定电流(低功耗函数中提升了HSE偏置电流)
    DelayUs(5/5);                       // 等待稳定 1-5us
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PRINT( "wake.. \n");      
    DelayMs(500);
#endif

#if 1
    PRINT( "shut down mode sleep \n");   
    DelayMs(1);
    LowPower_Shutdown( NULL );                          //全部断电，唤醒后复位
/* 
   此模式唤醒后会执行复位，所以下面代码不会运行，
   注意要确保系统睡下去再唤醒才是唤醒复位，否则有可能变成IDLE等级唤醒 
*/
/*
使用HSI/5=6.4M睡眠，唤醒时间大概需要 2048Tsys≈330us
HSE起振一般不超过1.2ms(500us-1200us)，所以切换到外部HSE，需要 1.2ms-330us 这个时间可以保证HSE足够稳定，一般用于蓝牙
DelayUs()函数时基于32M时钟的书写，此时主频为6.4M，所以 DelayUs((1200-330)/5)    
*/
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_PON)) {     // 是否HSE上电
        PWR_UnitModCfg( ENABLE, UNIT_SYS_HSE );   // HSE上电
        DelayUs((1200)/5);
    }
    else if(!(R16_CLK_SYS_CFG&RB_CLK_OSC32M_XT)){   // 是否选择 HSI/5 做时钟源
        DelayUs((1200-330)/5);
    } 
    HSECFG_Current( HSE_RCur_100 );     // 降为额定电流(低功耗函数中提升了HSE偏置电流)
    DelayUs(5/5);                       // 等待稳定 1-5us
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PRINT( "wake.. \n");
    DelayMs(500);
#endif

    while(1);    
}


void GPIO_IRQHandler(void)
{
    GPIOA_ClearITFlagBit( GPIO_Pin_6|GPIO_Pin_5 );
}




