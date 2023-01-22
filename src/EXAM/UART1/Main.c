/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 		 : ����1�շ���ʾ
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"


UINT8 TxBuff[]="This is a tx exam\r\n";
UINT8 RxBuff[100];
UINT8 trigB;


int main()
{
    UINT8 len;
    
/* ���ô���1��������IO��ģʽ�������ô��� */   
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);			// RXD-������������
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);		// TXD-�������������ע������IO������ߵ�ƽ
    UART1_DefInit();
    
#if 1       // ���Դ��ڷ����ַ���
    UART1_SendString( TxBuff, sizeof(TxBuff) );

#endif   

#if 1       // ��ѯ��ʽ���������ݺ��ͳ�ȥ
    while(1)
    {
        len = UART1_RecvString(RxBuff);
        if( len )
        {
            UART1_SendString( RxBuff, len );            
        }
    }
    
#endif    

#if 0      // �жϷ�ʽ���������ݺ��ͳ�ȥ
    UART1_ByteTrigCfg( UART_7BYTE_TRIG );
    trigB = 7;
    UART1_INTCfg( ENABLE, RB_IER_RECV_RDY|RB_IER_LINE_STAT );
    NVIC_EnableIRQ( UART1_IRQn );
#endif    

    while(1);    
}


void UART1_IRQHandler(void)
{
    UINT8 i;
    
    switch( UART1_GetITFlag() )
    {
        case UART_II_LINE_STAT:        // ��·״̬����
            UART1_GetLinSTA();
            break;
        
        case UART_II_RECV_RDY:          // ���ݴﵽ���ô�����
            for(i=0; i!=trigB; i++)
            {
                RxBuff[i] = UART1_RecvByte();
                UART1_SendByte(RxBuff[i]);
            }
            break;
        
        case UART_II_RECV_TOUT:         // ���ճ�ʱ����ʱһ֡���ݽ������
            i = UART1_RecvString(RxBuff);
            UART1_SendString( RxBuff, i ); 
            break;
        
        case UART_II_THR_EMPTY:         // ���ͻ������գ��ɼ�������
            break;
        
        case UART_II_MODEM_CHG:         // ֻ֧�ִ���0
            break;
        
        default:
            break;
    }
}




