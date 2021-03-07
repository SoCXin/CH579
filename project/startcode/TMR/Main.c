/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2019/4/28
* Description 		 : 定时器例子
*******************************************************************************/

#include "CH57x_common.h"

__align(4) UINT32 CapBuf[100];
volatile UINT8 capFlag = 0;

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{     
    UINT8  i;
		UINT32 x = 0;
  
/* 配置串口调试 */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );
    
#if 0       /* 定时器0，设定100ms定时器进行IO口闪灯， PB3-LED */
    
    GPIOB_SetBits( GPIO_Pin_3 );
    GPIOB_ModeCfg( GPIO_Pin_3, GPIO_ModeOut_PP_5mA );
    
    TMR0_TimerInit( FREQ_SYS/10 );                  // 设置定时时间 100ms
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);          // 开启中断
    NVIC_EnableIRQ( TMR0_IRQn );
    
#endif 

#if 1       /* 定时器3，PWM输出 */
    
    GPIOA_ResetBits( GPIO_Pin_2 );            // 配置PWM口 PA2
    GPIOA_ModeCfg( GPIO_Pin_2, GPIO_ModeOut_PP_5mA );
    
    TMR3_PWMInit( High_Level, PWM_Times_1 );
    TMR3_PWMCycleCfg( 3200 );        // 周期 100ms
    TMR3_PWMActDataWidth( 1000 );              // 占空比 50%
    
#endif   

#if 1       /* 定时器1，CAP捕捉， */
    PWR_UnitModCfg( DISABLE, UNIT_SYS_LSE );     // 注意此引脚是LSE晶振引脚，要保证关闭才能使用其他功能
    GPIOA_ResetBits( GPIO_Pin_10 );             // 配置PWM口 PA10
    GPIOA_ModeCfg( GPIO_Pin_10, GPIO_ModeIN_PU );

    TMR1_CapInit( Edge_To_Edge );
    TMR1_CAPTimeoutCfg( 0xFFFFFFFF );   // 设置捕捉超时时间
    TMR1_DMACfg( ENABLE, (UINT16)&CapBuf[0], (UINT16)&CapBuf[100], Mode_Single );
    TMR1_ITCfg(ENABLE, TMR1_2_IT_DMA_END);          // 开启DMA完成中断
    NVIC_EnableIRQ( TMR1_IRQn );
    
    while( capFlag == 0 );
    capFlag = 0;
    for( i=0; i<100; i++ )
    {
        printf("%08ld ", CapBuf[i]&0x1ffffff);      // bit26 最高位表示 高电平还是低电平
    }printf("\n");

#endif
#if 1       /* 定时器2，计数器 */
		GPIOB_ModeCfg( GPIO_Pin_11, GPIO_ModeIN_PD );
		GPIOPinRemap( ENABLE, RB_PIN_TMR2 );
		
		TMR2_CountInit( FallEdge_To_FallEdge );
		TMR2_CountOverflowCfg( 1000 );                  // 设置计数上限1000
		
		/* 开启计数溢出中断，计慢1000个周期进入中断 */   
		TMR2_ClearITFlag( TMR0_3_IT_CYC_END );
		NVIC_EnableIRQ(TMR2_IRQn);
		TMR2_ITCfg( ENABLE, TMR0_3_IT_CYC_END);

		do
		{
						/* 约0.5s打印一次当前计数值，如果送入脉冲频率较高，可能很快计数溢出，需要按实际情况修改 */
				x++;
				if( !( x & 0xfffff ) )	printf("=%ld ", TMR2_GetCurrentCount());			
		}while(1); 

#endif
    while(1);    
}



void TMR0_IRQHandler( void )        // TMR0 定时中断
{
    if( TMR0_GetITFlag( TMR0_3_IT_CYC_END ) )
    {
        TMR0_ClearITFlag( TMR0_3_IT_CYC_END );      // 清除中断标志
        GPIOB_InverseBits( GPIO_Pin_3 );        
    }
}

void TMR1_IRQHandler( void )        // TMR1 定时中断
{
    if( TMR1_GetITFlag( TMR1_2_IT_DMA_END ) )
    {
        TMR1_ITCfg(DISABLE, TMR1_2_IT_DMA_END);       // 使用单次DMA功能+中断，注意完成后关闭此中断使能，否则会一直上报中断。
        TMR1_ClearITFlag( TMR1_2_IT_DMA_END );      // 清除中断标志  
        capFlag = 1;
        printf("*");
    }
}

void TMR2_IRQHandler(void)
{
   if( TMR2_GetITFlag(TMR0_3_IT_CYC_END) ) 
   {
       TMR2_ClearITFlag( TMR0_3_IT_CYC_END );
       /* 计数器计满，硬件自动清零，重新开始计数 */
       /* 用户可自行添加需要的处理 */
   }
   
}




