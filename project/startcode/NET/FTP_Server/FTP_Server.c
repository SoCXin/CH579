/********************************** (C) COPYRIGHT ******************************
* File Name          : FTP_SERVER.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH579芯片FTP接口命令
*                    : MDK3.36@ARM966E-S,Thumb,小端
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH57x_common.h"
#include "core_cm0.h"
#include "CH57xNET.H"
#include "FTPINC.H"
#include "FTPCMD.C"
#include "FTPfile.c"

/*******************************************************************************
* Function Name  : CH57xNET_FTPSendData
* Description    : 准备好要发送的数据
* Input          : pName-文件名
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPDataReady( char *pName )
{
    ftp.CmdDataS = FTP_MACH_SENDDATA;                                           /* 数据传输状态置为发送数据 */
    CH57xNET_FTPFileRead( pName );                                              /* 将要发送的数据写入发送缓冲区 */
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPHandleDatRece
* Description    : 处理接收到的数据
* Input          : recv_buff -握手信息
                   sockeid   -socket索引
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPHandleDatRece( char *recv_buff,UINT8 sockeid )
{
    if(sockeid == ftp.SocketDatConnect){
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){                                  /* 接受文件数据 */
            CH57xNET_FTPFileWrite(recv_buff,(UINT16)strlen(recv_buff));
        }
    }
    if(sockeid == ftp.SocketCtlConnect){                                        /* 接收命令 */            
        CH57xNET_FTPCmdRespond(recv_buff);                                      /* 处理相应的命令 */
    }    
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPServerCmd
* Description    : 查询状态，执行相应的命令
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPServerCmd( void )
{
    UINT8 i;

    if(ftp.TcpCtlSta == FTP_MACH_CONNECT){                                      /* 建立TCP FTP控制连接 */
        ftp.TcpCtlSta = FTP_MACH_KEPCONT; 
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[0],strlen(FTP_SERVICE_CMD[0]),ftp.SocketCtlConnect );
    }
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){                                      /* 数据传输完成 */
        ftp.CmdDataS = 0;    
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[1],strlen(FTP_SERVICE_CMD[1]),ftp.SocketCtlConnect );
        if( ftp.SocketDatMonitor != 255 ){                                      /* 关闭数据连接（TCP Server）*/
            i = CH57xNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            mStopIfError(i); 
            ftp.SocketDatMonitor = 255;
        }
    }
    if(ftp.CmdDataS == FTP_MACH_SENDDATA){                                      /* 发送数据 */
        if(ftp.TcpDatSta >= FTP_MACH_CONNECT){ 
            ftp.TcpDatSta = FTP_MACH_KEPCONT;
            if(strlen(SendBuf))CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketDatConnect );
            if(ftp.DataOver ){                                                  /* 发送完毕 */
                ftp.CmdDataS = FTP_MACH_DATAOVER; 
                i = CH57xNET_SocketClose( ftp.SocketDatConnect,TCP_CLOSE_NORMAL );/* 关闭数据连接 */
                mStopIfError(i);
            }
        }
    }
    if(ftp.CmdReceDatS == 1){                                                   /* 需要接收数据 */
        if(ftp.TcpDatSta == FTP_MACH_CONNECT){
            ftp.CmdReceDatS = 0;
            ftp.CmdDataS = FTP_MACH_RECEDATA;                                   /* 数据传输状态置为接收数据 */
        }
    }
    if(ftp.TcpDatSta == FTP_MACH_DISCONT){                                      /* 接收数据完毕 */
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){
            ftp.CmdDataS = FTP_MACH_DATAOVER;
            CH57xNET_FTPListRenew( ftp.ListFlag );                              /* 更新目录 */
        }
    } 
    if((ftp.TimeCount>20)&&(ftp.SocketDatMonitor != 255||ftp.SocketCtlConnect != 255)){
        if(ftp.SocketDatMonitor != 255) CH57xNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
        if(ftp.SocketCtlConnect != 255) CH57xNET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
    }
}
/*********************************** endfile **********************************/
