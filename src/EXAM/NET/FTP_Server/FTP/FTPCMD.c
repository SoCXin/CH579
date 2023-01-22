/********************************** (C) COPYRIGHT ******************************
 * File Name          : FTPCMD.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/09/15
 * Description        : WCHNET FTP command code
 *                    : MDK3.36@ARM966E-S,Thumb,Ð¡¶Ë
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "FTPINC.h"
#include "wchnet.h"

char FTP_SERVICE_CMD[10][60] = 
{
    "220 Microsoft FTP Service\r\n",                                            /* 0 login */
    "226 Transfer complete\r\n",                                                /* 1 transfer completed*/ 
    "331 access allowed send identity as password.\r\n",                        /* 2 Username is correct */
    "230 user logged in.\r\n",                                                 /* 3 login successful */
    "530 user cannot log in.\r\n",                                             /* 4 Login failed */
    "200 PORT command successful.\r\n",                                         /* 5 PORT command succeeded */
    "125 Data connection already open;Transfer starting\r\n",                   /* 6 get list command */
    "150 Opening ASCII mode data connection for",                               /* 7 The download/upload file command succeeded */
    "550 The ch579 cannot find the file specified",                             /* 8 file does not exist */
    "221 Goodbye\r\n",                                                          /* 9 quit */
};

char FTP_SERVICE_CMD1[11][40] = 
{
    "200 Type set to",                                                          /* 0 TYPE sets the file transfer type */
    "215 Windows_NT\r\n",                                                       /* 1 SYST system type */
    "257 \"/\" is current directory.\r\n",                                      /* 2 PWD get current directory */
    "250 command successful.\r\n",                                              /* 3 command successful */
    "227 Entering Passive Mode(",                                               /* 4 PASV command succeeded */
    "213 ",                                                                     /* 5 size command succeeded */
    "250 DELE command success\r\n",                                             /* 6 delete */
    "550 Access is denied.\r\n",                                                /* 7 access denied */
    "200 noop command success.\r\n",                                            /* 8 noop command success */
    "500 command not understood\r\n",                                           /* 9 command not supported */
    "257 \"/USER/\" is current directory.\r\n",                                 /* 10 PWD get current directory */
};

char FTP_SERVICE_CMD2[2][50] = 
{
    "350 File exists,ready for destination name\r\n",                           
    "250 RNTO command successful\r\n",                             
};  

FTP   ftp;
char  SendBuf[SendBufLen];                                                 
uint8_t CmdPortIP[4];                                                             /* Used to save the IP address converted to characters*/ 
uint16_t TemAddr;                                                                 /* Temporary variables for analyzing IP addresses and ports */
uint16_t NumPort;                                                                 /* The port number */
uint32_t gLEN;                                                                    /* file length */

char  gFileName[16];                                                            /* Save the filename of the requested download */
char  pFileName[16];                                                            /* Save the filename of the requested upload */
char  ListName[16];                                                             /* Used to save the current directory */

/*******************************************************************************
* Function Name  : WCHNET_FTPGetPortNum
* Description    : Get the port number and IP address
* Input          : recv_buff - data buff
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPGetPortNum( char *recv_buff )
{
    UINT8 k,i;
        
    i = 0;
    memset((void *)CmdPortIP,0,4);
    for(k=0;k<strlen(recv_buff);k++){
        if(recv_buff[k]>='0'&&recv_buff[k]<='9'){
            if(i<4){                                                            /* Get the destination IP address */ 
                TemAddr = 0;
                while(recv_buff[k] != ','){
                    TemAddr += recv_buff[k] - '0';
                    k++;
                    TemAddr *= 10;
                }
                TemAddr /= 10;
                CmdPortIP[i] = TemAddr; 
                i++;
            }
            else{                                                               /* get port number */ 
                NumPort = 0;
                while(recv_buff[k] != ','){
                    NumPort += (recv_buff[k]-'0')*256;
                    k++;
                    NumPort *= 10;
                }
                NumPort /= 10;
                k++;
                TemAddr = 0;
                while(recv_buff[k]>='0'&&recv_buff[k]<='9'){
                    TemAddr +=  (recv_buff[k]-'0');
                    k++;
                    TemAddr *= 10;
                }
                TemAddr /= 10;
                NumPort += TemAddr;
                break;
            }
        }
    }
    PRINT("CmdPortIP: %d.%d.%d.%d\n",CmdPortIP[0],CmdPortIP[1],CmdPortIP[2],CmdPortIP[3]);
    PRINT("NumPort = %08d\n",NumPort);
}

/*******************************************************************************
* Function Name  : WCHNET_FTPGetFileName
* Description    : Get file name
* Input          : recv_buff - data buff
                   pBuf      - buffer to hold names
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPGetFileName( char *recv_buff,char *pBuf )
{
     UINT8 k,i;
    
    k = 0;
    while((recv_buff[k] != ' ')&&k<strlen(recv_buff)) k++;                    
    k++;
    i = 0;
    while((recv_buff[k] != '\r')&&k<strlen(recv_buff)){                         
        if(i > 14 ) return;
        if(recv_buff[k] != '/'){
            pBuf[i] = recv_buff[k];
            i++;
        }
        k++;
    }
    PRINT("Name: %s\n",pBuf);
}

/*******************************************************************************
* Function Name  : WCHNET_FTPCmdRespond
* Description    : According to the command sent by the client, 
                   return the corresponding reply
* Input          : recv_buff - data buff
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPCmdRespond( char *recv_buff ) 
{
    UINT8  s;
    UINT8  len;
    UINT16 port;
    
    if( strncmp("USER", recv_buff, 4) == 0 ){                                   /* user name */
        memset((void *)UserName,'\0',sizeof(UserName));    
        WCHNET_FTPGetFileName( recv_buff,UserName );    
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[2],strlen(FTP_SERVICE_CMD[2]),ftp.SocketCtlConnect );
    }
    else if( strncmp("PASS", recv_buff, 4) == 0 ){                              /* password */
#if Access_Authflag                                                             /* Verify username */
        if(strncmp(pUserName, UserName, strlen(pUserName)) == 0){               /* Username is correct */
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[3],strlen(FTP_SERVICE_CMD[3]),ftp.SocketCtlConnect );
        }
        else{                                                                   /* username error */
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[4],strlen(FTP_SERVICE_CMD[4]),ftp.SocketCtlConnect );
        }
#else
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[3],strlen(FTP_SERVICE_CMD[3]),ftp.SocketCtlConnect );
        ftp.AuthFlag = 1;
#endif
        memset((void *)UserName,'\0',sizeof(UserName));    
        WCHNET_FTPGetFileName( recv_buff,UserName );
        if(strncmp(pPassWord, UserName, strlen(pPassWord)) == 0){               /* Verify that the password is correct (if it meets the conditions, it has write file permission)*/
            ftp.AuthFlag = 1;
            printf("have more authority\n");
        }    
    }
    else if( strncmp("PORT", recv_buff, 4) == 0 ){                              /* The TCP SERVER port number and IP address that the client sends the client-side wear */
        WCHNET_FTPGetPortNum( recv_buff );
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[5],strlen(FTP_SERVICE_CMD[5]),ftp.SocketCtlConnect );
        WCHNET_FTPClientDat( NumPort, CmdPortIP);          
    }
    else if( strncmp("LIST", recv_buff, 4) == 0 ){                              /* Get current list information */
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[6],strlen(FTP_SERVICE_CMD[6]),ftp.SocketCtlConnect );
        WCHNET_FTPDataReady( "0" );
    }
    else if( strncmp("STOR", recv_buff, 4) == 0){                               /* The command for the client to request to upload data (requires to receive data) */
        if(ftp.AuthFlag == 1){                                                  /* Support upload if you have permission */
            ftp.CmdReceDatS = 1;
            ftp.FileFlag = 0;
            memset((void *)pFileName,'\0',sizeof(pFileName));    
            WCHNET_FTPGetFileName( recv_buff,pFileName );
            memset((void *)SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"%s %s.\r\n",FTP_SERVICE_CMD[7],pFileName);
            WCHNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
        }
        else{                                                                   /* Unsupported command without permission, close the connection */
            WCHNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            WCHNET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("RETR", recv_buff, 4) == 0){                               /* The client requests a command to download data (data needs to be sent) */
        if(recv_buff[5] == '/'){
            memset((void *)SendBuf,'\0',sizeof(SendBuf));    
            WCHNET_FTPGetFileName( recv_buff,SendBuf );
            len = strlen(ListName);
            sprintf(gFileName,&SendBuf[len]); 
        }
        else{
            memset((void *)gFileName,'\0',sizeof(gFileName));    
            WCHNET_FTPGetFileName( recv_buff,gFileName );
        } 
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        s = WCHNET_FTPFileOpen( gFileName,FTP_MACH_FILE );        
        if( s ){                                                                /* File exists, start transferring data */
            sprintf(SendBuf,"%s %s.\r\n",FTP_SERVICE_CMD[7],gFileName);
            WCHNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            WCHNET_FTPDataReady( gFileName );
        }
        else{                                                                   /* file exists, send error response */
            sprintf(SendBuf,"%s£¨%s).\r\n",FTP_SERVICE_CMD[8],gFileName); 
            WCHNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            s = WCHNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            mStopIfError(s); 
            ftp.SocketDatMonitor = 255;
        }
    }
    else if( strncmp("QUIT", recv_buff, 4) == 0 ){                              /* sign out */
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD[9],strlen(FTP_SERVICE_CMD[9]),ftp.SocketCtlConnect );
    }
    else if( strncmp("TYPE", recv_buff, 4) == 0 ){                              /* Type of data transferred */
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        sprintf(SendBuf,"%s %c.\r\n",FTP_SERVICE_CMD1[0],recv_buff[5]);
        WCHNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
    }
    else if( strncmp("syst", recv_buff, 4) == 0 ){                              /* system type */
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[1],strlen(FTP_SERVICE_CMD1[1]),ftp.SocketCtlConnect );
    }
    else if( strncmp("PWD", recv_buff, 3) == 0 ){                               /* show directory */
        if(strncmp("USER", ListName, strlen("USER")) == 0 ){
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[10],strlen(FTP_SERVICE_CMD1[10]),ftp.SocketCtlConnect );
        }
        else{
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[2],strlen(FTP_SERVICE_CMD1[2]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("CWD", recv_buff, 3) == 0 ){                               /* open Directory */
        memset((void *)ListName,'\0',sizeof(ListName));    
        WCHNET_FTPGetFileName( recv_buff,ListName );
        s = WCHNET_FTPFileOpen( ListName,FTP_MACH_LIST );
        if(s){
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[3],strlen(FTP_SERVICE_CMD1[3]),ftp.SocketCtlConnect ); 
        }
        else{
            memset((void *)SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"%s.\r\n",FTP_SERVICE_CMD[8]);
            WCHNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect ); 
        }
    }
    else if( strncmp("PASV", recv_buff, 4) == 0 ){                              /* The client is used to obtain the TCP port number and IP address created by the server */
        WCHNET_FTPServerDat( &port );
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        sprintf(SendBuf,"%s%s%02d,%02d).\r\n",FTP_SERVICE_CMD1[4],SourIP,(port/256),(port%256));
        WCHNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
    }
    else if( strncmp("SIZE", recv_buff, 4) == 0 ){                              /* get file size */
        if(recv_buff[5] == '/'){
            memset((void *)SendBuf,'\0',sizeof(SendBuf));    
            WCHNET_FTPGetFileName( recv_buff,SendBuf );
            len = strlen(ListName);
            sprintf(gFileName,&SendBuf[len]); 
        }
        else{
            memset((void *)pFileName,'\0',sizeof(gFileName));    
            WCHNET_FTPGetFileName( recv_buff,gFileName );
        } 
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        gLEN = WCHNET_FTPFileSize( gFileName );
        sprintf(SendBuf,"%s%d\r\n",FTP_SERVICE_CMD1[5],gLEN);
        WCHNET_FTPSendData( (char *)SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
    }
    else if( strncmp("MKD", recv_buff, 3) == 0 ){                               /* Create a directory */
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* Support upload if you have permission */
            memset((void *)pFileName,'\0',sizeof(pFileName));    
            WCHNET_FTPGetFileName( recv_buff,pFileName );
            memset((void *)SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"257 MKD %s.\r\n",pFileName);
            WCHNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            ftp.ListFlag = 3;
            WCHNET_FTPListRenew( ftp.ListFlag );
        }
        else{                                                                   /* Unsupported command without permission, close the connection */
            WCHNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            WCHNET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("RMD", recv_buff, 3) == 0 ){                               /* Delete a directory  */
        if(ftp.AuthFlag == 1){                                                  /* Support delete if you have permission */
            if(recv_buff[4] == '/'){
                memset((void *)SendBuf,'\0',sizeof(SendBuf));    
                WCHNET_FTPGetFileName( recv_buff,SendBuf );
                len = strlen(ListName);
                sprintf(pFileName,&SendBuf[len]); 
            }
            else{
                memset((void *)pFileName,'\0',sizeof(pFileName));    
                WCHNET_FTPGetFileName( recv_buff,pFileName );
            } 
            ftp.ListFlag = 4;
            WCHNET_FTPListRenew( ftp.ListFlag );
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[6],strlen(FTP_SERVICE_CMD1[6]),ftp.SocketCtlConnect );
        }
        else{
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("DELE", recv_buff, 4) == 0 ){                              /* Delete Files */
        if(ftp.AuthFlag == 1){                                                  /* Support delete if you have permission */
            if(recv_buff[5] == '/'){
                memset((void *)SendBuf,'\0',sizeof(SendBuf));    
                WCHNET_FTPGetFileName( recv_buff,SendBuf );
                len = strlen(ListName);
                sprintf(pFileName,&SendBuf[len]); 
            }
            else{
                memset((void *)pFileName,'\0',sizeof(pFileName));    
                WCHNET_FTPGetFileName( recv_buff,pFileName );
            } 
            ftp.ListFlag = 2;
            WCHNET_FTPListRenew( ftp.ListFlag );
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[6],strlen(FTP_SERVICE_CMD1[6]),ftp.SocketCtlConnect );
        }
        else{
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("noop", recv_buff, 4) == 0 ){
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[8],strlen(FTP_SERVICE_CMD1[8]),ftp.SocketCtlConnect );
    }
    else if( strncmp("RNFR", recv_buff, 4) == 0 ){
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* Support delete if you have permission */
            memset((void *)SendBuf,'\0',sizeof(SendBuf));    
            if(recv_buff[5] == '/'){
                WCHNET_FTPGetFileName( recv_buff,SendBuf );
                len = strlen(ListName);
                sprintf(pFileName,&SendBuf[len]); 
            }
            else{
                memset((void *)pFileName,'\0',sizeof(pFileName));    
                WCHNET_FTPGetFileName( recv_buff,pFileName );
            } 
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD2[0],strlen(FTP_SERVICE_CMD2[0]),ftp.SocketCtlConnect );
        }
        else{
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("RNTO", recv_buff, 4) == 0 ){
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* Support delete if you have permission */
            memset((void *)gFileName,'\0',sizeof(gFileName));    
            if(recv_buff[5] == '/'){
                memset((void *)SendBuf,'\0',sizeof(SendBuf));    
                WCHNET_FTPGetFileName( recv_buff,SendBuf );
                len = strlen(ListName);
                sprintf(gFileName,&SendBuf[len]); 
            }
            else{
                WCHNET_FTPGetFileName( recv_buff,gFileName );
            } 
            ftp.ListFlag = 5;
            WCHNET_FTPListRenew( ftp.ListFlag );
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD2[1],strlen(FTP_SERVICE_CMD2[1]),ftp.SocketCtlConnect );
        }
        else{
            WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else{                                                                       /* command not supported */
        WCHNET_FTPSendData( (char *)FTP_SERVICE_CMD1[9],strlen(FTP_SERVICE_CMD1[9]),ftp.SocketCtlConnect );
    }
}

/*********************************** endfile **********************************/
