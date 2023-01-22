/********************************** (C) COPYRIGHT ******************************
 * File Name          : FTPFILE.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/09/15
 * Description        : WCHNET FTP Client - file read and write
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "FTPINC.h"

/*******************************************************************************
* Function Name  : WCHNET_FTPFileOpen
* Description    : open a file   
* Input          : pFileName - FileName
* Output         : None               
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPFileOpen ( char *pFileName)
{
    return FTP_CHECK_SUCCESS;
}

/*******************************************************************************
* Function Name  : WCHNET_FTPFileWrite
* Description    : read download data
* Input          : recv_buff - data buff
                   len       - data length
* Output         : None               
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPFileWrite ( char *recv_buff ,uint16_t len)
{
    memcpy(send_buff,recv_buff,len);        
    return (FTP_CHECK_SUCCESS);
}

/*******************************************************************************
* Function Name  : WCHNET_FTPFileRead
* Description    : transmit data  
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void WCHNET_FTPFileRead( void )
{
    memset( send_buff, '\0', sizeof(send_buff));
    if(strlen(pContent)<536) {
        sprintf( send_buff, "%s\r\n",pContent);             
        ftp.CmdDataS = FTP_MACH_DATAOVER;                  
    }
}

/*********************************** endfile **********************************/
