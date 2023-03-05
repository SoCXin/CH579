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
    uint8_t TcpStatus;                // TCP����״̬
    uint8_t CmdStatus;                // �ӿ�����ִ��״̬
    uint8_t InterCmdS;                // �ӿ�����ִ�б�־λ
    uint8_t CmdFirtS;                 // �����һ��ִ�б�־λ
    uint8_t FindList;                 // �ҵ�ָ�����ļ���
    uint8_t FindFile;                 // �ҵ�ָ�����ļ���
    uint8_t CmdDataS;                 // ��������״̬
    uint8_t SocketCtl;                // ftp��������socket����
    uint8_t DatMonitor;               // ftp������������socket����
    uint8_t DatTransfer;              // ftp�������Ӵ���socket����
    uint8_t FileCmd;                  // �ӿ���������    
    UINT16 SouPort;                   // �˿ں�
    UINT16 RecDatLen;                 // ���յ������ݳ���
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

/* TCP����״̬ */
#define FTP_MACH_DISCONT            0x00          // TCP�Ͽ�����
#define FTP_MACH_CONNECT            0x01          // TCP����

/* ����״̬ */
#define FTP_MACH_QUIT               0x01          // ��½ǰ����
#define FTP_MACH_CLOSE              0x02          // ��½�����

/* ���ݴ���״̬ */
#define FTP_MACH_SENDDATA           0x01          // ��Ҫ��������
#define FTP_MACH_RECEDATA           0x02          // ��Ҫ��������
#define FTP_MACH_DATAOVER           0x04          // ���ݴ������

/* �ӿ���ִ��״̬ */
#define FTP_MACH_LOGINSUC           0x01          // ��¼�ɹ�
#define FTP_MACH_SEARCHSUC          0x02          // Ѱ���ļ����
#define FTP_MACH_GETSUC             0x04          // �����ļ��ɹ�
#define FTP_MACH_PUTSUC             0x08          // �ϴ��ļ��ɹ�

/*  ����ִ�й��̱�־λ */
#define FTP_MACH_FINDLIST           0x01          // Ѱ��Ŀ¼��־λ
#define FTP_MACH_FINDFILE           0x02          // Ѱ���ļ���־λ
#define FTP_MACH_GETFILE            0x04          // �����ļ���־λ
#define FTP_MACH_MKDLIST            0x08          // ����Ŀ¼��־λ
#define FTP_MACH_PUTFILE            0x10          // �ϴ��ļ���־λ

/* ����״̬��Ϣ */ 
#define FTP_CHECK_NULL              0x00          // û�����źŷ����򲻽��з�����Ϣ�˶� 
#define FTP_CHECK_SUCCESS           0x00          // ���������Ϣ��ȷ  
#define FTP_COMMAND_SUCCESS         0x00          // �ӿ�����ִ�гɹ�
#define FTP_COMMAND_CONTINUE        0x01          // �ӿ������ִ�й�����

/* ������ӿ��������� */
#define FTP_CMD_LOGIN               0x01          // ��¼
#define FTP_CMD_SEARCH              0x02          // Ѱ���ļ�
#define FTP_CMD_GETFILE             0x03          // �����ļ�
#define FTP_CMD_PUTFILE             0x04          // �ϴ��ļ�
/* ********************************************************************************
* �����źź˶Դ������
**********************************************************************************/
#define FTP_ERR_LOGIN               0x20         // ��¼
#define FTP_ERR_SEARCH              0x21         // Ѱ���ļ�
#define FTP_ERR_GETFILE             0x22         // �����ļ�
#define FTP_ERR_PUTFILE             0x23         // �ϴ��ļ�
#define FTP_ERR_UNLIST              0x2a         // δ�ҵ�Ŀ¼ 
#define FTP_ERR_UNFILE              0x2b         // δ�ҵ��ļ�
#define FTP_ERR_UNKW                0x2f         // δ֪����

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
