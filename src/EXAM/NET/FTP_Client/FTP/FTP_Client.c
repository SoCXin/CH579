/********************************** (C) COPYRIGHT ******************************
 * File Name          : FTP_CLIENT.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/09/15
 * Description        : WCHNET FTP data processing                  
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH57x_common.h"
#include "core_cm0.h"
#include "WCHNET.h"
#include "FTPINC.h"

/*******************************************************************************
* Function Name  : WCHNET_FTPProcessReceDat
* Description    : read download data  
* Input          : recv_buff  -Buffer for receiving data
*                  check_type -current command type
*                  socketid   -socket id
* Output         : None               
* Return         : None
*******************************************************************************/
void WCHNET_FTPProcessReceDat( char *recv_buff,uint8_t check_type,uint8_t socketid )
{
    uint8_t S;
    if( socketid == ftp.DatTransfer ){
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){                           //Receive file data
            if( ftp.InterCmdS == FTP_MACH_GETFILE ){ 
                S = WCHNET_FTPFileWrite(recv_buff,strlen(recv_buff));
            }
            else if(ftp.InterCmdS == FTP_MACH_FINDLIST ){
                S = WCHNET_FTPFindList( recv_buff );                     //Validate transfer data for finding the specified directory name
                if( S == FTP_CHECK_SUCCESS ) ftp.FindList = 1;           //Find the specified directory name
            }
            else if(ftp.InterCmdS == FTP_MACH_FINDFILE ){
                S = WCHNET_FTPFindFile( recv_buff );                     //Find files   
                if( S == FTP_CHECK_SUCCESS ) ftp.FindFile = 1;           //file found
            }
        }
    }
    else if( socketid == ftp.SocketCtl ){                                //The received data is the command response          
        S = WCHNET_FTPCheckRespond(recv_buff,check_type);
    }
}

/*******************************************************************************
* Function Name  : CH395_FTPSendFile
* Description    : send data
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPSendFile( void )
{
    uint8_t S;    
    
    S = WCHNET_FTPFileOpen( FileName );
    if(S == FTP_CHECK_SUCCESS)    WCHNET_FTPFileRead( );
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.DatTransfer );
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){
        WCHNET_SocketClose( ftp.DatTransfer,TCP_CLOSE_NORMAL );
    }    
}

/*******************************************************************************
* Function Name  : WCHNET_FTPCheckRespond
* Description    : Query the status and execute the corresponding command
* Input          : recv_buff  -Buffer for receiving data
                   check_type -current command type
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPClientCmd( void )
{
    
    if(ftp.CmdDataS == FTP_MACH_SENDDATA){
        if(ftp.TcpStatus == FTP_MACH_CONNECT) WCHNET_FTPSendFile( );          //send data to server
        return;     
    }
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){ 
        if(ftp.FindList == 1){                                                //Directory name found
            ftp.FindList = 0;
            WCHNET_FTPCwd( 0 );
        }
    }
    if(ftp.FileCmd){
        WCHNET_FTPInterCmd( );                                                //Execute the corresponding interface command
    }
}

/*******************************************************************************
* Function Name  : WCHNET_FTPInterCmd
* Description    : Execute the corresponding interface command
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPInterCmd( void )
{
    uint8_t i;
    switch(ftp.FileCmd){
        case FTP_CMD_LOGIN:                                                 //Log in
           i= WCHNET_FTPLogin( ); 
           if( i == FTP_COMMAND_SUCCESS ){                             		//Login successfully, execute other commands
				printf("login!\r\n"); 
				WCHNET_FTPSearch(pSearchDir,pSearchFile);                   //Start querying the specified directory
            }
            break;
        case FTP_CMD_SEARCH:                                                //search file (parameters: directory name, file name)
            if( WCHNET_FTPSearch(pSearchDir,pSearchFile) == FTP_COMMAND_SUCCESS ){   				
                if( ftp.FindFile )WCHNET_FTPGetFile(pSearchFile);         	//start downloading file
                else WCHNET_FTPQuit( );                                     //quit
            }
            break;
        case FTP_CMD_GETFILE:                                               //download file (parameter: filename)
            if(WCHNET_FTPGetFile(pSearchFile) == FTP_COMMAND_SUCCESS ){   	//The file is downloaded successfully
                WCHNET_FTPPutFile(pMkDir,pTouchFile);                       //Start uploading files
            }
            break;
        case FTP_CMD_PUTFILE:                                               //upload files
            if( WCHNET_FTPPutFile(pMkDir,pTouchFile) == FTP_COMMAND_SUCCESS ){  //Uploaded file successfully
                WCHNET_FTPQuit( );                                          //quit
            }
            break;
        default:
            break;
    }
}

/*********************************** endfile **********************************/
