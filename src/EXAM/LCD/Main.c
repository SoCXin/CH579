/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 		 : LCD��ʾ���������������0-7
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"


//�����ʾС����+0x80.
unsigned char const lcd[10]={0x7d,0x60,0x3e,0x7a,0x63,0x5b,0x5f,0x70,0x7f,0x9b};
/*     4
     |----|
    0|    |5
     |-1--|
    2|    |6
     |----| . 7
       3
*/
/* ע�⣺ʹ�ô����ӣ�����ʱ��ر��ⲿ�ֶ���λ���� */

int main()
{    
    LCD_WriteData0( lcd[0] );
    LCD_WriteData1( lcd[1] );
    LCD_WriteData2( lcd[2] );
    LCD_WriteData3( lcd[3] );
    LCD_WriteData4( lcd[4] );
    LCD_WriteData5( lcd[5] );
    LCD_WriteData6( lcd[6] );
    LCD_WriteData7( lcd[7] );  
    
    //  ��ʾ8������
    GPIOAGPPCfg( ENABLE, RB_PIN_SEG0_3_IE
                        |RB_PIN_SEG4_7_IE
                        |RB_PIN_SEG8_11_IE
                        |RB_PIN_SEG12_15_IE
                        /*|RB_PIN_SEG16_19_IE
                        |RB_PIN_SEG20_23_IE*/ );
    LCD_DefInit();
    
    while(1);    
}






