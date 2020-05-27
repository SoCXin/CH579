/********************************** (C) COPYRIGHT ******************************
* File Name          : FTP_CLIENT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/01
* Description        : CH579芯片FTP接口命令                    
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH57x_common.h"
#include "core_cm0.h"
#include "CH57xNET.h"
#include "FTPINC.h"
#include "FTPCMD.c"
#include "FTPfile.c"

/*******************************************************************************
* Function Name  : CH57xNET_FTPProcessReceDat
* Description    : 读取下载数据   
* Input          : recv_buff  -579接收到的数据
*                  check_type -当前命令类型
*                  socketid   -socket索引
* Output         : None               
* Return         : None
*******************************************************************************/
void CH57xNET_FTPProcessReceDat( char *recv_buff,UINT8 check_type,UINT8 socketid )
{
    UINT8 S;
    if( socketid == ftp.DatTransfer ){
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){                                  /* 接受文件数据 */
            if( ftp.InterCmdS == FTP_MACH_GETFILE ){ 
                S = CH57xNET_FTPFileWrite(recv_buff,strlen(recv_buff));
            }
            else if(ftp.InterCmdS == FTP_MACH_FINDLIST ){
                S = CH57xNET_FTPFindList( recv_buff );                          /* 验证传输数据，用于查找指定的目录名 */
                if( S == FTP_CHECK_SUCCESS ) ftp.FindList = 1;                  /* 查到指定的目录名 */
            }
            else if(ftp.InterCmdS == FTP_MACH_FINDFILE ){
                S = CH57xNET_FTPFindFile( recv_buff );                          /* 查找文件 */    
                if( S == FTP_CHECK_SUCCESS ) ftp.FindFile = 1;                  /* 找到文件 */
            }
        }
    }
    else if( socketid == ftp.SocketCtl ){   
        /* 接收为命令应答 */           
        S = CH57xNET_FTPCheckRespond(recv_buff,check_type);
    }
}

/*******************************************************************************
* Function Name  : CH395_FTPSendFile
* Description    : 发送数据
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPSendFile( void )
{
    UINT8 S;    
    
    S = CH57xNET_FTPFileOpen( FileName );
    if(S == FTP_CHECK_SUCCESS)    CH57xNET_FTPFileRead( );
    CH57xNET_FTPSendData( send_buff, strlen(send_buff),ftp.DatTransfer );
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){
        CH57xNET_SocketClose( ftp.DatTransfer,TCP_CLOSE_NORMAL );
    }    
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPCheckRespond
* Description    : 查询状态，执行相应命令
* Input          : recv_buff  -握手信息
                   check_type -检测的类型
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPClientCmd( void )
{
    
    if(ftp.CmdDataS == FTP_MACH_SENDDATA){//printf("client1\r\n"); 
        if(ftp.TcpStatus == FTP_MACH_CONNECT) CH57xNET_FTPSendFile( );          /* 向服务器发送数据 */
        return ;     
    }
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){//printf("client2\r\n"); 
        if(ftp.FindList == 1){                                                  /* 找到目录名 */
            ftp.FindList = 0;
            CH57xNET_FTPCwd( 0 );
        }
    }
    if(ftp.FileCmd){//printf("client3\r\n"); 
        CH57xNET_FTPInterCmd( );                                                /* 执行对应的接口命令 */
    }
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPInterCmd
* Description    : 执行对应的命令，可自行调整执行顺序
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPInterCmd( void )
{
    UINT8 i;
    switch(ftp.FileCmd){
        case FTP_CMD_LOGIN:                                                     /* 登陆 */
           i= CH57xNET_FTPLogin( ); //printf("CH57xNET_FTPLogin %02x\r\n",i); 
           if( /*CH57xNET_FTPLogin( )*/i == FTP_COMMAND_SUCCESS ){                  /* 登陆成功,可进行其他操作 */             
               printf("login!\r\n"); 
						 CH57xNET_FTPSearch("C:\Users\FJY\Desktop\192","FILELIST.txt" );                     /* 开始查询指定的目录 */   
             
            }
            break;
        case FTP_CMD_SEARCH:                                                    /* 搜查文件（参数：目录名，文件名）*/
            if( CH57xNET_FTPSearch("USER","FILELIST.txt" ) == FTP_COMMAND_SUCCESS ){        
                if( ftp.FindFile )CH57xNET_FTPGetFile("FILELIST.txt" );         /* 搜查到指定的目录下的文件则开始下载文件 */
                else CH57xNET_FTPQuit( );                                       /* 没搜查到指定的目录下的文件则退出（也可进行其他操作，如上传）*/
            }
            break;
        case FTP_CMD_GETFILE:                                                   /* 下载文件（参数：文件名）*/ 
            if(CH57xNET_FTPGetFile("FILELIST.txt" ) == FTP_COMMAND_SUCCESS ){   /* 下载文件成功，可进行其他操作 */
                CH57xNET_FTPPutFile("TEXT","abc.txt");                          /* 上传文件 */
            }
            break;
        case FTP_CMD_PUTFILE:                                                   /* 上传文件（参数：目录名，文件名）*/
            if( CH57xNET_FTPPutFile("TEXT","abc.txt")== FTP_COMMAND_SUCCESS ){  /* 上传文件成功，可进行其他操作 */
                CH57xNET_FTPQuit( );                                            /* 退出 */
            }
            break;
        default:
            break;
    }
}

/*********************************** endfile **********************************/
