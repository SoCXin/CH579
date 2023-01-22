/********************************** (C) COPYRIGHT *********************************
 * File Name          : FTPINC.H
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/09/15
 * Description        : Define for WCHNET FTP.c
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/



/*********************************************************************************/
#ifndef _FTPINC_H_
#define _FTPINC_H_
#include "ch579SFR.h"
#include "CH57x_common.h"

#ifndef Access_Authflag
#define Access_Authflag                0            /* Set specified username access (other usernames cannot access the server)*/
#endif
typedef struct _FTP
{
    uint8_t TimeCount;                   // ��ʱ������
    uint8_t FileFlag;                    // �ж��ļ����ȴ���64�ֽڱ�־λ��������֧������ϴ��ļ�����64��
    uint8_t BufStatus;                   // ��������־λ��0-3λ�ֱ��Ӧ�ĸ�������״̬��0Ϊ��1Ϊ��ʹ�ã�
    uint8_t AuthFlag;                    // �û�Ȩ�ޱ�־λ
    uint8_t ListFlag;                    // 
    uint8_t CmdDataS;                    // ���ݴ���״̬
    uint8_t DataOver;                    // ���ݶ�ȡ���
    uint8_t ListState;                   // 0Ϊ��ǰĿ¼ 1Ϊ��һ��Ŀ¼
    uint8_t TcpCtlSta;                   // TCP��������״̬
    uint8_t TcpDatSta;                   // TCP��������״̬
    uint8_t CmdReceDatS;                 // ��������״̬
    uint8_t SocketCtlMonitor;            // ftp�������ӣ�����socket����
    uint8_t SocketCtlConnect;            // ftp�������ӣ�����socket����
    uint8_t SocketCtlSecond;             // ftp�������ӣ�����socket����
    uint8_t SocketDatMonitor;            // ftp�������ӣ�����socket����
    uint8_t SocketDatConnect;            // ftp�������ӣ�����socket����
}FTP;

#define SendBufLen                 500                                      

extern FTP ftp;
extern char  UserName[16];         
extern char  SourIP[17];          
#if Access_Authflag
extern char  *pUserName;          
#endif
extern char  *pPassWord;           
/* CH579��ض��� */                                                                   
extern uint8_t MACAddr[6];                        
extern uint8_t IPAddr[4];                   
extern uint8_t GWIPAddr[4];               
extern uint8_t IPMask[4];                  

extern char FTP_SERVICE_CMD2[2][50];
extern char FTP_SERVICE_CMD1[11][40];
extern char FTP_SERVICE_CMD[10][60];
extern char SendBuf[SendBufLen]; 
extern char gFileName[16];                                                            
extern char pFileName[16];                                                           
extern char ListName[16];
	
extern const char *Atest;
extern const char *Btest;
extern const char *Ctest;
extern const char *FileNature;
extern const char *ListNature;


#define FTP_MACH_DISCONT             0x01        // TCP not connected
#define FTP_MACH_CONNECT             0x02        // on a TCP connection
#define FTP_MACH_KEPCONT             0x04        // TCP keepalive

#define FTP_MACH_SENDDATA            0x01        // send data
#define FTP_MACH_RECEDATA            0x02        // Receive data
#define FTP_MACH_DATAOVER            0x04        // data transfer completed
#define FTP_MACH_CLOSECTL            0x08        // close control connection

#define FTP_MACH_LIST                0x00        // get list
#define FTP_MACH_FILE                0x01        // get file
//==============================================================================
void  mInitFtpList( void );                                                  
 
void WCHNET_FTPCtlServer(void);                                             

void  WCHNET_FTPConnect( void );                                         

void  WCHNET_FTPInitVari( void );                                          

void WCHNET_FTPServerCmd( void );                                             

void mStopIfError(uint8_t iError);

void WCHNET_FTPFileRead( char *pName );                                       

void WCHNET_FTPListRenew( uint8_t index );                                   

void WCHNET_FTPDataReady( char *pSendDat );                               

void WCHNET_FTPServerDat( uint16_t *port );                                  

uint32_t WCHNET_FTPFileSize ( char *pName );                                

void WCHNET_FTPCmdRespond( char *recv_buff );                                 

void WCHNET_FTPGetPortNum( char *recv_buff );                                 

void WCHNET_FTPGetSockeID( uint8_t socketid );                                

void WCHNET_FTPGetFileName( char *recv_buff,char *pBuf );                   

uint8_t WCHNET_FTPFileOpen(  char *pName,uint8_t index  );                      

void WCHNET_FTPFileWrite( char *recv_buff, uint16_t LEN );                      

void WCHNET_FTPClientDat(  uint16_t port,uint8_t *pAddr );                        

void WCHNET_FTPHandleDatRece( char *recv_buff,uint8_t index );                  

void WCHNET_FTPHandleDatRece( char *recv_buff,UINT8 sockeid );

void  WCHNET_FTPSendData( char *PSend, uint32_t Len,uint8_t index  );             

#endif    // __ftpinc_H__
 //================================================================================ 
