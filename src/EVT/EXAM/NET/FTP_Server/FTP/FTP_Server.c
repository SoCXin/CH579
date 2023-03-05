/********************************** (C) COPYRIGHT ******************************
 * File Name          : FTP_SERVER.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/09/15
 * Description        : WCHNET FTP interface commands
 *                    : MDK3.36@ARM966E-S,Thumb,–°∂À
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/



/******************************************************************************/
/* Â§¥Êñá‰ª∂ÂåÖÂê? */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH57x_common.h"
#include "core_cm0.h"
#include "FTPINC.H"
#include "wchnet.h"

/*******************************************************************************
* Function Name  : WCHNET_FTPSendData
* Description    : data ready to be sent
* Input          : pName - file name 
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPDataReady( char *pName )
{
    ftp.CmdDataS = FTP_MACH_SENDDATA;                                           /* Data transmission status is set to send data */
    WCHNET_FTPFileRead( pName );                                              /* Write the data to be sent into the send buffer */
}

/*******************************************************************************
* Function Name  : WCHNET_FTPHandleDatRece
* Description    : Process the received data
* Input          : recv_buff - data buff
                   sockeid   
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPHandleDatRece( char *recv_buff,uint8_t sockeid )
{
    if(sockeid == ftp.SocketDatConnect){
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){                                  /* Accept file data */
            WCHNET_FTPFileWrite(recv_buff,(uint16_t)strlen(recv_buff));
        }
    }
    if(sockeid == ftp.SocketCtlConnect){                                        /* receive command */            
        WCHNET_FTPCmdRespond(recv_buff);                                      /* process the corresponding command */
    }    
}

/*******************************************************************************
* Function Name  : WCHNET_FTPServerCmd
* Description    : Query the status and execute the corresponding command
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPServerCmd( void )
{
    uint8_t i;

    if(ftp.TcpCtlSta == FTP_MACH_CONNECT){                                      /* Establish a TCP FTP control connection */
        ftp.TcpCtlSta = FTP_MACH_KEPCONT; 
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[0],strlen(FTP_SERVICE_CMD[0]),ftp.SocketCtlConnect );
    }
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){                                      /* data transfer completed */
        ftp.CmdDataS = 0;    
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[1],strlen(FTP_SERVICE_CMD[1]),ftp.SocketCtlConnect );
        if( ftp.SocketDatMonitor != 255 ){                                      /* Close the data connection (TCP Server) */
            i = WCHNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            mStopIfError(i); 
            ftp.SocketDatMonitor = 255;
        }
    }
    if(ftp.CmdDataS == FTP_MACH_SENDDATA){                                      /* send data */
        if(ftp.TcpDatSta >= FTP_MACH_CONNECT){ 
            ftp.TcpDatSta = FTP_MACH_KEPCONT;
            if(strlen(SendBuf))WCHNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketDatConnect );
            if(ftp.DataOver ){                                                  /* send completed */
                ftp.CmdDataS = FTP_MACH_DATAOVER; 
                i = WCHNET_SocketClose( ftp.SocketDatConnect,TCP_CLOSE_NORMAL );/* close data connection */
                mStopIfError(i);
            }
        }
    }
    if(ftp.CmdReceDatS == 1){                                                   /* need to receive data */
        if(ftp.TcpDatSta == FTP_MACH_CONNECT){
            ftp.CmdReceDatS = 0;
            ftp.CmdDataS = FTP_MACH_RECEDATA;                                   /* Data transmission status is set to receive data */
        }
    }
    if(ftp.TcpDatSta == FTP_MACH_DISCONT){                                      /* Finished receiving data */
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){
            ftp.CmdDataS = FTP_MACH_DATAOVER;
            WCHNET_FTPListRenew( ftp.ListFlag );                              /* update directory */
        }
    } 
    if((ftp.TimeCount>20)&&(ftp.SocketDatMonitor != 255||ftp.SocketCtlConnect != 255)){
        if(ftp.SocketDatMonitor != 255) WCHNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
        if(ftp.SocketCtlConnect != 255) WCHNET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
    }
}
/*********************************** endfile **********************************/
