/********************************** (C) COPYRIGHT *********************************
 * File Name          : FTPINC.H
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/09/15
 * Description        : Define for CH579FTP.Cs
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __FTPINC_H__
#define __FTPINC_H__
#include "CH579SFR.h"
#include "CH57x_common.h"

#define DEBUG                        1       

typedef struct _FTP
{
    uint8_t TcpStatus;                // TCP连接状态
    uint8_t CmdStatus;                // 接口命令执行状态
    uint8_t InterCmdS;                // 接口命令执行标志位
    uint8_t CmdFirtS;                 // 命令第一次执行标志位
    uint8_t FindList;                 // 找到指定的文件夹
    uint8_t FindFile;                 // 找到指定的文件名
    uint8_t CmdDataS;                 // 传输数据状态
    uint8_t SocketCtl;                // ftp控制连接socket索引
    uint8_t DatMonitor;               // ftp数据连接侦听socket索引
    uint8_t DatTransfer;              // ftp数据连接传输socket索引
    uint8_t FileCmd;                  // 接口命令类型    
    UINT16 SouPort;                   // 端口号
    UINT16 RecDatLen;                 // 接收到的数据长度
}FTP;

extern FTP ftp;
extern char ListName[24];      
extern char ListMkd[24];       
extern char FileName[24];     
extern char CharIP[17];        
extern char *pUserName;  
extern char *pPassword;  
extern char *pSearchDir;
extern char *pSearchFile;
extern char *pMkDir;
extern char *pTouchFile;
extern char *pContent;

                                                                 
extern uint8_t MACAddr[6];                         
extern uint8_t IPAddr[4];                    
extern uint8_t GWIPAddr[4];                  
extern uint8_t IPMask[4];                   
extern uint8_t DESIP[4];
extern char  send_buff[536];

/* TCP连接状态 */
#define FTP_MACH_DISCONT            0x00          // TCP断开连接
#define FTP_MACH_CONNECT            0x01          // TCP连接

/* 错误状态 */
#define FTP_MACH_QUIT               0x01          // 登陆前错误
#define FTP_MACH_CLOSE              0x02          // 登陆后错误

/* 数据传输状态 */
#define FTP_MACH_SENDDATA           0x01          // 需要发送数据
#define FTP_MACH_RECEDATA           0x02          // 需要接收数据
#define FTP_MACH_DATAOVER           0x04          // 数据传输完成

/* 接口名执行状态 */
#define FTP_MACH_LOGINSUC           0x01          // 登录成功
#define FTP_MACH_SEARCHSUC          0x02          // 寻找文件完成
#define FTP_MACH_GETSUC             0x04          // 下载文件成功
#define FTP_MACH_PUTSUC             0x08          // 上传文件成功

/*  命令执行过程标志位 */
#define FTP_MACH_FINDLIST           0x01          // 寻找目录标志位
#define FTP_MACH_FINDFILE           0x02          // 寻找文件标志位
#define FTP_MACH_GETFILE            0x04          // 下载文件标志位
#define FTP_MACH_MKDLIST            0x08          // 创建目录标志位
#define FTP_MACH_PUTFILE            0x10          // 上传文件标志位

/* 返回状态信息 */ 
#define FTP_CHECK_NULL              0x00          // 没握手信号返回则不进行返回信息核对 
#define FTP_CHECK_SUCCESS           0x00          // 检测握手信息正确  
#define FTP_COMMAND_SUCCESS         0x00          // 接口命令执行成功
#define FTP_COMMAND_CONTINUE        0x01          // 接口命令还在执行过程中

/* 主程序接口命令类型 */
#define FTP_CMD_LOGIN               0x01          // 登录
#define FTP_CMD_SEARCH              0x02          // 寻找文件
#define FTP_CMD_GETFILE             0x03          // 下载文件
#define FTP_CMD_PUTFILE             0x04          // 上传文件
/* ********************************************************************************
* 握手信号核对错误代码
**********************************************************************************/
#define FTP_ERR_LOGIN               0x20         // 登录
#define FTP_ERR_SEARCH              0x21         // 寻找文件
#define FTP_ERR_GETFILE             0x22         // 下载文件
#define FTP_ERR_PUTFILE             0x23         // 上传文件
#define FTP_ERR_UNLIST              0x2a         // 未找到目录 
#define FTP_ERR_UNFILE              0x2b         // 未找到文件
#define FTP_ERR_UNKW                0x2f         // 未知错误

/**********************************************************************************/

void WCHNET_FTPQuit( void );                                                                     

void WCHNET_FTPInit( void );                                                                     

void WCHNET_FTPInterCmd( void );                                                                 
 
void WCHNET_FTPFileRead( void );
	
void WCHNET_FTPInitVari( void );                                                                                            

void WCHNET_FTPClientCmd( void );                                                                

void mStopIfError( uint8_t iError );

void WCHNET_FTPCwd( uint8_t index );

void WCHNET_HandleGloableInt( void );

void WCHNET_CreateFTPCtrlClient(void);                                                                  

void WCHNET_FTPDatServer( UINT16 *port );

void WCHNET_FTPCommand( uint8_t choiceorder);                                                     

void WCHNET_CreateFTPDatClient( UINT16 *port );                                                        

void WCHNET_FTPSendData( char *PSend, uint32_t Len,uint8_t index );                                   

void WCHNET_FTPProcessReceDat( char *recv_buff,uint8_t check_type,uint8_t socketid );                

uint8_t WCHNET_FTPLogin( void );                                                                    

uint8_t WCHNET_FTPGetFile( char *pFileName );                                                      

uint8_t WCHNET_FTPMkdirCheck( char *rece_buf);                                                      

uint8_t WCHNET_FTPFileOpen ( char *pFileName);
	
uint8_t WCHNET_FTPFindFile( char *pReceiveData );                                                  

uint8_t WCHNET_FTPFindList( char *pReceiveData );                                                  

uint8_t WCHNET_FTPGetFileData( char *recv_buff );                                                   

uint8_t WCHNET_FTPSearch(char *pListNAme,char *pFileName );                                        

uint8_t WCHNET_FTPPutFile(char *pListNAme, char *pFileName);                                        

uint8_t WCHNET_FTPFileWrite ( char *recv_buff ,uint16_t len);
	
uint8_t WCHNET_FTPCheckRespond( char *recv_buff,uint8_t check_type );                                




#endif    
