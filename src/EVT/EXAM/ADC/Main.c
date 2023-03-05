/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 		 : adc采样示例，包括温度检测、单通道检测、差分通道检测、TouchKey检测、中断方式采样。
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"


UINT16 adcBuff[40];
volatile UINT8  adclen;

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{
    UINT8 i;
    signed short  RoughCalib_Value=0;		// ADC粗调偏差值
    
/* 配置串口调试 */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );

/* 温度采样并输出， 包含数据粗校准 */ 
    PRINT( "\n1.Temperature sampling...\n");
    ADC_InterTSSampInit();
    RoughCalib_Value = ADC_DataCalib_Rough();  // 用于计算ADC内部偏差，记录到变量 RoughCalib_Value中，注意这个变量需要定义为有符号变量
    for(i=0; i<20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver() + RoughCalib_Value;      // 连续采样20次
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
    
/* 单通道采样：选择adc通道9做采样，对应 PA0引脚， 带数据校准功能 */   
    PRINT( "\n2.Single channel sampling...\n");
    GPIOA_ModeCfg(GPIO_Pin_0, GPIO_ModeIN_Floating);
    ADC_ExtSingleChSampInit( SampleFreq_3_2, ADC_PGA_0 );
    
/* 
    注意：数据校准包括 粗调和细调：
    ADC_DataCalib_Rough() 是粗调函数，调用前需保证PA5引脚配置浮空输入模式，外部没有电压信号，在ADC初始化之后调用，一般调用一次，除非更改了ADC模式（增益改变）
    ADC_DataCalib_Fine() 是细调函数，对于粗调过的数据进行拟合算法运算，得到更精确数据
*/    
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_Floating);
    RoughCalib_Value = ADC_DataCalib_Rough();  // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中  
    PRINT("RoughCalib_Value=%d \n", RoughCalib_Value);    
    
    ADC_ChannelCfg( 9 );        
    for(i=0; i<20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver() + RoughCalib_Value;      // 连续采样20次
        ADC_DataCalib_Fine( &adcBuff[i], ADC_PGA_0 );
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
    
/* 差分通道采样：选择adc通道0做采样，对应 PA4(AIN0)、PA12(AIN2) */  
    PRINT( "\n3.Diff channel sampling...\n");    
    GPIOA_ModeCfg(GPIO_Pin_4|GPIO_Pin_12, GPIO_ModeIN_Floating);    
    ADC_ExtDiffChSampInit( SampleFreq_3_2, ADC_PGA_0 );
    ADC_ChannelCfg( 0 );
    for(i=0; i<20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver();      // 连续采样20次
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
    
/* TouchKey采样：选择adc通道 2 做采样，对应 PA12 */ 
    PRINT( "\n4.TouchKey sampling...\n");    
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_Floating);
    TouchKey_ChSampInit();
    ADC_ChannelCfg( 2 );    

    for(i=0; i<20; i++)
    {
        adcBuff[i] = TouchKey_ExcutSingleConver(0x20);      // 连续采样20次
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
        
    
/* 单通道采样：中断方式,选择adc通道1做采样，对应 PA5引脚， 不带数据校准功能 */
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
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
    
    while(1);    
}


void ADC_IRQHandler(void)			//adc中断服务程序
{
    if(ADC_GetITStatus())
    {
        adcBuff[adclen] = ADC_ReadConverValue();
        ADC_StartUp(); // 作用清除中断标志并开启新一轮采样
        adclen ++;
    }
}




