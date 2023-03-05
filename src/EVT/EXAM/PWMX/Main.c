/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 		 : PWM4-11������ʾ
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"


int main()
{
    
/* ����GPIO */   
    GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeOut_PP_5mA); // PA6 - PWM4
    GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA); // PA7 - PWM5
    GPIOB_ModeCfg(GPIO_Pin_0, GPIO_ModeOut_PP_5mA); // PB0 - PWM6
    GPIOB_ModeCfg(GPIO_Pin_1, GPIO_ModeOut_PP_5mA); // PB1 - PWM7
    GPIOB_ModeCfg(GPIO_Pin_2, GPIO_ModeOut_PP_5mA); // PB2 - PWM8
    GPIOB_ModeCfg(GPIO_Pin_3, GPIO_ModeOut_PP_5mA); // PB3 - PWM9
    GPIOB_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA); // PB14 - PWM10
//    GPIOB_ModeCfg(GPIO_Pin_23, GPIO_ModeOut_PP_5mA); // PB23 - PWM11 �˽Ÿ���Ϊ�ⲿ��λ�ţ���Ҫ�رմ˹��ܲ��ܿ���PWM����
    
    PWMX_CLKCfg( 4 );                   // cycle = 4/Fsys
    PWMX_CycleCfg( PWMX_Cycle_64 );     // ���� = 64*cycle
    PWMX_ACTOUT( CH_PWM4, 64/4, Low_Level, ENABLE);     // 25% ռ�ձ�
    PWMX_ACTOUT( CH_PWM5, 64/4, Low_Level, ENABLE);     // 25% ռ�ձ�
    PWMX_ACTOUT( CH_PWM6, 64/4, Low_Level, ENABLE);     // 25% ռ�ձ�
    PWMX_ACTOUT( CH_PWM7, 64/4, Low_Level, ENABLE);     // 25% ռ�ձ�
    PWMX_ACTOUT( CH_PWM8, 64/4, Low_Level, ENABLE);     // 25% ռ�ձ�
    PWMX_ACTOUT( CH_PWM9, 64/4, Low_Level, ENABLE);     // 25% ռ�ձ�
    PWMX_ACTOUT( CH_PWM10, 64/4, Low_Level, ENABLE);     // 25% ռ�ձ�

    while(1);    
}






