/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_lcd.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : LCD_DefInit
* Description    : LCD��ʽ������Ĭ�ϳ�ʼ������
* Input          : None			   				
* Return         : None
*******************************************************************************/
void LCD_DefInit( void )
{	
    /* 3.3V 1KHZ  COM0~COM3  1/3BISA */
    R8_LCD_CTRL_MOD = RB_SYS_POWER_ON			\
                    |RB_LCD_POWER_ON			\
                    |(1<<2)				        \
                    |(2<<3)                     \
                    |(2<<5)                     \
                    |(0<<7) ;   
}
