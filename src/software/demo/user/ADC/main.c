/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : 
* Version            : 
* Date               : 2018/12/15
* Description 		 	 : adc采样示例，包括温度检测、单通道检测、差分通道检测、TouchKey检测、中断方式采样。
*******************************************************************************/

#include "CH57x_common.h"


UINT16 abcBuff[40];
volatile UINT8  adclen;

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{
    UINT8 i;
    
/* 配置串口调试 */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );

/* 温度采样并输出 */ 
    PRINT( "\n1.Temperature sampling...\n");
    ADC_InterTSSampInit();
    for(i=0; i<20; i++)
    {
        abcBuff[i] = ADC_ExcutSingleConver();      // 连续采样20次
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", abcBuff[i]);
    }PRINT("\n");
    
/* 单通道采样：选择adc通道1做采样，对应 PA5引脚 */   
    PRINT( "\n2.Single channel sampling...\n");
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_Floating);
    ADC_ExtSingleChSampInit( SampleFreq_3_2, ADC_PGA_0 );
    ADC_ChannelCfg( 1 );
        
    for(i=0; i<20; i++)
    {
        abcBuff[i] = ADC_ExcutSingleConver();      // 连续采样20次
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", abcBuff[i]);
    }PRINT("\n");
    
/* 差分通道采样：选择adc通道0做采样，对应 PA4(AIN0)、PA12(AIN2) */  
    PRINT( "\n3.Diff channel sampling...\n");    
    GPIOA_ModeCfg(GPIO_Pin_4|GPIO_Pin_12, GPIO_ModeIN_Floating);    
    ADC_ExtDiffChSampInit( SampleFreq_3_2, ADC_PGA_0 );
    ADC_ChannelCfg( 0 );
    for(i=0; i<20; i++)
    {
        abcBuff[i] = ADC_ExcutSingleConver();      // 连续采样20次
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", abcBuff[i]);
    }PRINT("\n");
    
/* TouchKey采样：选择adc通道 2 做采样，对应 PA12 */ 
    PRINT( "\n4.TouchKey sampling...\n");    
    GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeIN_Floating);
    TouchKey_ChSampInit();
    ADC_ChannelCfg( 2 );    

    for(i=0; i<20; i++)
    {
        abcBuff[i] = TouchKey_ExcutSingleConver(0x20);      // 连续采样20次
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", abcBuff[i]);
    }PRINT("\n");
        
    
/* 单通道采样：中断方式,选择adc通道1做采样，对应 PA5引脚 */
    PRINT( "\n5.Single channel sampling in interrupt mode...\n");    
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_Floating);
    ADC_ExtSingleChSampInit( SampleFreq_3_2, ADC_PGA_0 );
    ADC_ChannelCfg( 1 );
    NVIC_EnableIRQ(ADC_IRQn);
    adclen = 0;
    
    ADC_StartUp();  
    while(adclen < 20);
    NVIC_DisableIRQ(ADC_IRQn);
    for(i=0; i<20; i++)
    {
        PRINT("%d ", abcBuff[i]);
    }PRINT("\n");
    
    while(1);    
}


void ADC_IRQHandler(void)			//adc中断服务程序
{
    if(ADC_GetITStatus())
    {
        abcBuff[adclen] = ADC_ReadConverValue();
        ADC_ClearITFlag();
        adclen ++;
    }
}




