/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2019/4/29
 * Description 		 : SPI1演示 Master 模式数据收发
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

/*  
引脚定义：

PA0  <->    SCK1
PA1  <->    MOSI1
PA2  <->    MISO1
PA4  <->    SCS1

*/


__align(4) UINT8 spiBuff[]={1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6};
__align(4) UINT8 spiBuffrev[16];

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
    
/* 配置串口调试 */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );

#if 1        
/* 主机模式 */ 
    PRINT( "1.spi1 mul master mode send data ...\n");   
    DelayMs( 100 );
    
    GPIOA_SetBits( GPIO_Pin_4 );
    GPIOA_ModeCfg(GPIO_Pin_4|GPIO_Pin_0|GPIO_Pin_1, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg(GPIO_Pin_2, GPIO_ModeIN_PU);
    SPI1_MasterDefInit( );
    
// 单字节发送/接收
    GPIOA_ResetBits( GPIO_Pin_4 );    
    SPI1_MasterSendByte(0x55);
    GPIOA_SetBits( GPIO_Pin_4 );
    DelayMs( 1 );
    GPIOA_ResetBits( GPIO_Pin_4 );
    i = SPI1_MasterRecvByte();
    GPIOA_SetBits( GPIO_Pin_4 );
    DelayMs( 2 );
    
// FIFO 连续发送/接收 
    GPIOA_ResetBits( GPIO_Pin_4 ); 
    SPI1_MasterTrans( spiBuff, 9 );
    GPIOA_SetBits( GPIO_Pin_4 );
    DelayMs( 1 );
    GPIOA_ResetBits( GPIO_Pin_4 ); 
    SPI1_MasterRecv( spiBuffrev, 12 );
    GPIOA_SetBits( GPIO_Pin_4 );
    DelayMs( 1 );
 
    PRINT( "END ...\n");
    while(1);
#endif


    while(1);    
     
}





