/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/15
* Description 		 : 系统睡眠模式并唤醒演示：GPIOA_6作为唤醒源，共6种睡眠等级
*******************************************************************************/

#include "CH57x_common.h"

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{
/* 配置串口调试 */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );
    DelsyMs(500);
    
#if 1    
    /* 配置唤醒源为 GPIO - PA6 */
    GPIOA_ModeCfg( GPIO_Pin_6, GPIO_ModeIN_PU );
    GPIOA_ITModeCfg( GPIO_Pin_6, GPIO_ITMode_FallEdge );        // 下降沿唤醒
    NVIC_EnableIRQ( GPIO_IRQn );
    PWR_PeriphWakeUpCfg( ENABLE, RB_SLP_GPIO_WAKE );
#endif    
    
#if 1
    PRINT( "IDLE mode sleep \n");   
    DelsyMs(1);
    LowPower_Idle();
    PRINT( "wake.. \n"); 
    DelsyMs(500);
#endif    
    
#if 1
    PRINT( "Halt_1 mode sleep \n");   
    DelsyMs(1);
    LowPower_Halt_1();
    PRINT( "wake.. \n"); 
    DelsyMs(500);    
#endif    
    
#if 1
    PRINT( "Halt_2 mode sleep \n");   
    DelsyMs(1);
    LowPower_Halt_2();
    PRINT( "wake.. \n"); 
    DelsyMs(500);    
#endif    

#if 1
    PRINT( "sleep mode sleep \n");   
    DelsyMs(1);
    LowPower_Sleep( RB_PWR_RAM14K|RB_PWR_RAM2K );       //只保留14+2K SRAM 供电
    SetSysClock( CLK_SOURCE_HSI_32MHz );                // 此低功耗模式唤醒后需要切换回原时钟
    PRINT( "wake.. \n");      
    DelsyMs(500);
#endif

#if 1
    PRINT( "shut down mode sleep \n");   
    DelsyMs(1);
    LowPower_Shutdown( NULL );                          //全部断电，唤醒后复位
    SetSysClock( CLK_SOURCE_HSI_32MHz );
    PRINT( "wake.. \n");
    DelsyMs(500);
#endif

    while(1);    
}


void GPIO_IRQHandler(void)
{
    GPIOA_ClearITFlagBit( GPIO_Pin_6 );
}




