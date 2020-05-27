/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : WCH
* Version            : V1.1
* Date               : 2020/03/18
* Description 		 : 系统睡眠模式并唤醒演示：GPIOA_6和GPIOA_5作为唤醒源，共6种睡眠等级
*******************************************************************************/

/* 注意：使用低功耗（Idle模式除外）睡眠唤醒后，需要重新配置系统时钟，否则使用的是32M时钟分频，4M主频 */

#include "CH57x_common.h"

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{
    SetSysClock( CLK_SOURCE_HSE_32MHz );        // 设置外部32M做主频
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
    SetSysClock( CLK_SOURCE_HSE_32MHz );  // 切换到原始时钟
    PRINT( "wake.. \n"); 
    DelayMs(500);    
#endif    
    
#if 1
    PRINT( "Halt_2 mode sleep \n");   
    DelayMs(1);
    LowPower_Halt_2();
    SetSysClock( CLK_SOURCE_HSE_32MHz );  // 切换到原始时钟
    PRINT( "wake.. \n"); 
    DelayMs(500);    
#endif    

#if 1
    PRINT( "sleep mode sleep \n");   
    DelayMs(1);
    LowPower_Sleep( RB_PWR_RAM14K|RB_PWR_RAM2K );       //只保留14+2K SRAM 供电
    SetSysClock( CLK_SOURCE_HSE_32MHz );  // 切换到原始时钟
    PRINT( "wake.. \n");      
    DelayMs(500);
#endif

#if 1
    PRINT( "shut down mode sleep \n");   
    DelayMs(1);
    LowPower_Shutdown( NULL );                          //全部断电，唤醒后复位
/* 此模式唤醒后会执行复位，所以下面代码不会运行 */
    SetSysClock( CLK_SOURCE_HSE_32MHz );  // 切换到原始时钟
    PRINT( "wake.. \n");
    DelayMs(500);
#endif

    while(1);    
}


void GPIO_IRQHandler(void)
{
    GPIOA_ClearITFlagBit( GPIO_Pin_6|GPIO_Pin_5 );
}




