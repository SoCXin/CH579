/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 		 : adc����ʾ���������¶ȼ�⡢��ͨ����⡢���ͨ����⡢TouchKey��⡢�жϷ�ʽ������
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
    signed short  RoughCalib_Value=0;		// ADC�ֵ�ƫ��ֵ
    
/* ���ô��ڵ��� */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );

/* �¶Ȳ���������� �������ݴ�У׼ */ 
    PRINT( "\n1.Temperature sampling...\n");
    ADC_InterTSSampInit();
    RoughCalib_Value = ADC_DataCalib_Rough();  // ���ڼ���ADC�ڲ�ƫ���¼������ RoughCalib_Value�У�ע�����������Ҫ����Ϊ�з��ű���
    for(i=0; i<20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver() + RoughCalib_Value;      // ��������20��
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
    
/* ��ͨ��������ѡ��adcͨ��9����������Ӧ PA0���ţ� ������У׼���� */   
    PRINT( "\n2.Single channel sampling...\n");
    GPIOA_ModeCfg(GPIO_Pin_0, GPIO_ModeIN_Floating);
    ADC_ExtSingleChSampInit( SampleFreq_3_2, ADC_PGA_0 );
    
/* 
    ע�⣺����У׼���� �ֵ���ϸ����
    ADC_DataCalib_Rough() �Ǵֵ�����������ǰ�豣֤PA5�������ø�������ģʽ���ⲿû�е�ѹ�źţ���ADC��ʼ��֮����ã�һ�����һ�Σ����Ǹ�����ADCģʽ������ı䣩
    ADC_DataCalib_Fine() ��ϸ�����������ڴֵ��������ݽ�������㷨���㣬�õ�����ȷ����
*/    
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_Floating);
    RoughCalib_Value = ADC_DataCalib_Rough();  // ���ڼ���ADC�ڲ�ƫ���¼��ȫ�ֱ��� RoughCalib_Value��  
    PRINT("RoughCalib_Value=%d \n", RoughCalib_Value);    
    
    ADC_ChannelCfg( 9 );        
    for(i=0; i<20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver() + RoughCalib_Value;      // ��������20��
        ADC_DataCalib_Fine( &adcBuff[i], ADC_PGA_0 );
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
    
/* ���ͨ��������ѡ��adcͨ��0����������Ӧ PA4(AIN0)��PA12(AIN2) */  
    PRINT( "\n3.Diff channel sampling...\n");    
    GPIOA_ModeCfg(GPIO_Pin_4|GPIO_Pin_12, GPIO_ModeIN_Floating);    
    ADC_ExtDiffChSampInit( SampleFreq_3_2, ADC_PGA_0 );
    ADC_ChannelCfg( 0 );
    for(i=0; i<20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver();      // ��������20��
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
    
/* TouchKey������ѡ��adcͨ�� 2 ����������Ӧ PA12 */ 
    PRINT( "\n4.TouchKey sampling...\n");    
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_Floating);
    TouchKey_ChSampInit();
    ADC_ChannelCfg( 2 );    

    for(i=0; i<20; i++)
    {
        adcBuff[i] = TouchKey_ExcutSingleConver(0x20);      // ��������20��
    }
    for(i=0; i<20; i++)
    {
        PRINT("%d ", adcBuff[i]);
    }PRINT("\n");
        
    
/* ��ͨ���������жϷ�ʽ,ѡ��adcͨ��1����������Ӧ PA5���ţ� ��������У׼���� */
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


void ADC_IRQHandler(void)			//adc�жϷ������
{
    if(ADC_GetITStatus())
    {
        adcBuff[adclen] = ADC_ReadConverValue();
        ADC_StartUp(); // ��������жϱ�־��������һ�ֲ���
        adclen ++;
    }
}




