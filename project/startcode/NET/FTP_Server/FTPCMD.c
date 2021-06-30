/********************************** (C) COPYRIGHT ******************************
* File Name          : FTPCMD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH579оƬFTP�������
*                    : MDK3.36@ARM966E-S,Thumb,С��
*******************************************************************************/



/******************************************************************************/
const char FTP_SERVICE_CMD[10][60] = 
{
    "220 Microsoft FTP Service\r\n",                                            /* 0 ��½ */
    "226 Transfer complete\r\n",                                                /* 1 ������� */ 
    "331 access allowed send identity as password.\r\n",                        /* 2 �û�����ȷ */
    "230 usser logged in.\r\n",                                                 /* 3 ��½�ɹ� */
    "530 usser cannot log in.\r\n",                                             /* 4 ��½ʧ�� */
    "200 PORT command successful.\r\n",                                         /* 5 PORT����ɹ� */
    "125 Data connection already open;Transfer starting\r\n",                   /* 6 ��ȡ�б����� */
    "150 Opening ASCII mode data connection for",                               /* 7 ����/�ϴ��ļ�����ɹ� */
    "550 The ch579 cannot find the file specified",                             /* 8 �ļ������� */
    "221 Goodbye\r\n",                                                          /* 9 �˳� */
};
const char  FTP_SERVICE_CMD1[10][40] = 
{
    "200 Type set to",                                                          /* 0 TYPE�����ļ��������� */
    "215 Windows_NT\r\n",                                                       /* 1 systϵͳ���� */
    "257 \"/\" is current directory.\r\n",                                      /* 2 PWD��ӡĿ¼ */
    "250 command successful.\r\n",                                              /* 3 ����ɹ� */
    "227 Entering Passive Mode(",                                               /* 4 PASV����ɹ� */
    "213 ",                                                                     /* 5 size����ɹ� */
    "250 DELE command success\r\n",                                             /* 6 ɾ�� */
    "550 Access is denied.\r\n",                                                /* 7 ���ʱ��ܾ� */
    "200 noop command success.\r\n",                                            /* 8 ������ */
    "500 command not understood\r\n",                                           /* 9 ���֧�� */
};
const char  FTP_SERVICE_CMD2[2][50] = 
{
    "350 File exists,ready for destination name\r\n",                           
    "250 RNTO command successful\r\n",                             
};                                                 
#define SendBufLen                 500                                          /* ���ͻ��������� */ 
FTP   ftp;
char  SendBuf[SendBufLen];                                                      /* ���ڱ���Ŀ�Ķ˴��͵�IP��ַ */
UINT8 CmdPortIP[4];                                                             /* ���ڱ���ת�����ַ���IP��ַ */ 
UINT16 TemAddr;                                                                 /* ���ڷ���IP��ַ���˿ڵ���ʱ���� */
UINT16 NumPort;                                                                 /* �˿ں� */
UINT32 gLEN;                                                                    /* �ļ����� */
/* ���ڱ����ļ��� */
char  gFileName[16];                                                            /* ���ڱ����������ص��ļ��� */
char  pFileName[16];                                                            /* ���ڱ��������ϴ����ļ��� */
char  ListName[16];                                                             /* ���ڱ��浱ǰĿ¼ */
/*******************************************************************************
* Function Name  : CH57xNET_FTPGetPortNum
* Description    : ��ȡ�˿ںż�IP��ַ
* Input          : recv_buff -���յ�������
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPGetPortNum( char *recv_buff )
{
    UINT8 k,i;
        
    i = 0;
    memset((void *)CmdPortIP,0,4);
    for(k=0;k<strlen(recv_buff);k++){
        if(recv_buff[k]>='0'&&recv_buff[k]<='9'){
            if(i<4){                                                            /* ��ȡĿ��IP��ַ */ 
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
            else{                                                               /* ��ȡ�˿ں� */ 
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
* Function Name  : CH57xNET_FTPGetFileName
* Description    : ��ȡ����
* Input          : recv_buff -���յ�������
                   pBuf      -�������ֵĻ�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPGetFileName( char *recv_buff,char *pBuf )
{
     UINT8 k,i;
    
    k = 0;
    while((recv_buff[k] != ' ')&&k<strlen(recv_buff)) k++;                      /* �ҵ�Ҫ��ȡ�����ݵ�λ�� */
    k++;
    i = 0;
    while((recv_buff[k] != '\r')&&k<strlen(recv_buff)){                         /* ������������� */
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
* Function Name  : CH57xNET_FTPCmdRespond
* Description    : ����client���������������Ӧ�Ļظ�
* Input          : recv_buff -���յ��ͻ��˷��͵��������
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPCmdRespond( char *recv_buff ) 
{
    UINT8  s;
    UINT8  len;
    UINT16 port;
    
    if( strncmp("USER", recv_buff, 4) == 0 ){                                   /* �û��� */
        memset((void *)UserName,'\0',sizeof(UserName));    
        CH57xNET_FTPGetFileName( recv_buff,UserName );    
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[2],strlen(FTP_SERVICE_CMD[2]),ftp.SocketCtlConnect );
    }
    else if( strncmp("PASS", recv_buff, 4) == 0 ){                              /* ���� */
#if Access_Authflag                                                             /* �����Ҫ����û��� */
        if(strncmp(pUserName, UserName, strlen(pUserName)) == 0){               /* �û�����ȷ */
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[3],strlen(FTP_SERVICE_CMD[3]),ftp.SocketCtlConnect );
        }
        else{                                                                   /* �û������� */
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[4],strlen(FTP_SERVICE_CMD[4]),ftp.SocketCtlConnect );
        }
#else
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[3],strlen(FTP_SERVICE_CMD[3]),ftp.SocketCtlConnect );
        ftp.AuthFlag = 1;
#endif
        memset((void *)UserName,'\0',sizeof(UserName));    
        CH57xNET_FTPGetFileName( recv_buff,UserName );
        if(strncmp(pPassWord, UserName, strlen(pPassWord)) == 0){               /* ��֤�����Ƿ���ȷ��������������д�ļ�Ȩ�ޣ�*/
            ftp.AuthFlag = 1;
printf("have more authority\n");
        }    
    }
    else if( strncmp("PORT", recv_buff, 4) == 0 ){                              /* �ͻ��˷��Ϳͻ��˴�����TCP SERVER�˿ںż�IP��ַ */
        CH57xNET_FTPGetPortNum( recv_buff );
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[5],strlen(FTP_SERVICE_CMD[5]),ftp.SocketCtlConnect );
        CH57xNET_FTPClientDat( NumPort, CmdPortIP);          
    }
    else if( strncmp("LIST", recv_buff, 4) == 0 ){                              /* ��ȡ��ǰ�б���Ϣ */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[6],strlen(FTP_SERVICE_CMD[6]),ftp.SocketCtlConnect );
        CH57xNET_FTPDataReady( "0" );
    }
    else if( strncmp("STOR", recv_buff, 4) == 0){                               /* �ͻ��������ϴ����ݵ������Ҫ�������ݣ�*/
        if(ftp.AuthFlag == 1){                                                  /* �����Ȩ����֧���ϴ� */
            ftp.CmdReceDatS = 1;
            ftp.FileFlag = 0;
            memset((void *)pFileName,'\0',sizeof(pFileName));    
            CH57xNET_FTPGetFileName( recv_buff,pFileName );
            memset((void *)SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"%s %s.\r\n",FTP_SERVICE_CMD[7],pFileName);
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
        }
        else{                                                                   /* ��Ȩ�޲�֧�ֵ�����ر����� */
            CH57xNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            CH57xNET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("RETR", recv_buff, 4) == 0){                               /* �ͻ��������������ݵ������Ҫ�������ݣ�*/
        if(recv_buff[5] == '/'){
            memset((void *)SendBuf,'\0',sizeof(SendBuf));    
            CH57xNET_FTPGetFileName( recv_buff,SendBuf );
            len = strlen(ListName);
            sprintf(gFileName,&SendBuf[len]); 
        }
        else{
            memset((void *)gFileName,'\0',sizeof(gFileName));    
            CH57xNET_FTPGetFileName( recv_buff,gFileName );
        } 
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        s = CH57xNET_FTPFileOpen( gFileName,FTP_MACH_FILE );        
        if( s ){                                                                /* �ļ����ڣ���ʼ�������� */
            sprintf(SendBuf,"%s %s.\r\n",FTP_SERVICE_CMD[7],gFileName);
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            CH57xNET_FTPDataReady( gFileName );
        }
        else{                                                                   /* �ļ����ڣ����ʹ�����Ӧ */
            sprintf(SendBuf,"%s��%s).\r\n",FTP_SERVICE_CMD[8],gFileName); 
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            s = CH57xNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            mStopIfError(s); 
            ftp.SocketDatMonitor = 255;
        }
    }
    else if( strncmp("QUIT", recv_buff, 4) == 0 ){                              /* �˳���¼ */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[9],strlen(FTP_SERVICE_CMD[9]),ftp.SocketCtlConnect );
    }
    else if( strncmp("TYPE", recv_buff, 4) == 0 ){                              /* ������������� */
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        sprintf(SendBuf,"%s %c.\r\n",FTP_SERVICE_CMD1[0],recv_buff[5]);
        CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
    }
    else if( strncmp("syst", recv_buff, 4) == 0 ){                              /* ϵͳ���� */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[1],strlen(FTP_SERVICE_CMD1[1]),ftp.SocketCtlConnect );
    }
    else if( strncmp("PWD", recv_buff, 3) == 0 ){                               /* ��ӡĿ¼ */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[2],strlen(FTP_SERVICE_CMD1[2]),ftp.SocketCtlConnect );
    }
    else if( strncmp("CWD", recv_buff, 3) == 0 ){                               /* ��Ŀ¼ */
        memset((void *)ListName,'\0',sizeof(ListName));    
        CH57xNET_FTPGetFileName( recv_buff,ListName );
        s = CH57xNET_FTPFileOpen( ListName,FTP_MACH_LIST );
        if(s){
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[3],strlen(FTP_SERVICE_CMD1[3]),ftp.SocketCtlConnect ); 
        }
        else{
            memset((void *)SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"%s.\r\n",FTP_SERVICE_CMD[8]);
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect ); 
        }
    }
    else if( strncmp("PASV", recv_buff, 4) == 0 ){                              /* �ͻ������ڻ�ȡ������������TCP�˿ںż�IP��ַ */
        CH57xNET_FTPServerDat( &port );
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        sprintf(SendBuf,"%s%s%02d.%02d).\r\n",FTP_SERVICE_CMD1[4],SourIP,(port/256),(port%256));
        CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
    }
    else if( strncmp("SIZE", recv_buff, 4) == 0 ){                              /* ��ȡ�ļ���С */
        if(recv_buff[5] == '/'){
            memset((void *)SendBuf,'\0',sizeof(SendBuf));    
            CH57xNET_FTPGetFileName( recv_buff,SendBuf );
            len = strlen(ListName);
            sprintf(gFileName,&SendBuf[len]); 
        }
        else{
            memset((void *)pFileName,'\0',sizeof(gFileName));    
            CH57xNET_FTPGetFileName( recv_buff,gFileName );
        } 
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        gLEN = CH57xNET_FTPFileSize( gFileName );
        sprintf(SendBuf,"%s%d\r\n",FTP_SERVICE_CMD1[5],gLEN);
        CH57xNET_FTPSendData( (char *)SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
    }
    else if( strncmp("MKD", recv_buff, 3) == 0 ){                               /* ����Ŀ¼ */
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* �����Ȩ����֧���ϴ� */
            memset((void *)pFileName,'\0',sizeof(pFileName));    
            CH57xNET_FTPGetFileName( recv_buff,pFileName );
            memset((void *)SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"257 MKD %s.\r\n",pFileName);
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            ftp.ListFlag = 3;
            CH57xNET_FTPListRenew( ftp.ListFlag );
        }
        else{                                                                   /* ��Ȩ�޲�֧�ֵ�����ر����� */
            CH57xNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            CH57xNET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("RMD", recv_buff, 3) == 0 ){                               /* ɾ���ļ� */
        if(ftp.AuthFlag == 1){                                                  /* �����Ȩ����֧��ɾ�� */
            if(recv_buff[4] == '/'){
                memset((void *)SendBuf,'\0',sizeof(SendBuf));    
                CH57xNET_FTPGetFileName( recv_buff,SendBuf );
                len = strlen(ListName);
                sprintf(pFileName,&SendBuf[len]); 
            }
            else{
                memset((void *)pFileName,'\0',sizeof(pFileName));    
                CH57xNET_FTPGetFileName( recv_buff,pFileName );
            } 
            ftp.ListFlag = 4;
            CH57xNET_FTPListRenew( ftp.ListFlag );
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[6],strlen(FTP_SERVICE_CMD1[6]),ftp.SocketCtlConnect );
        }
        else{
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("DELE", recv_buff, 4) == 0 ){                              /* ɾ���ļ� */
        if(ftp.AuthFlag == 1){                                                  /* �����Ȩ����֧��ɾ�� */
            if(recv_buff[5] == '/'){
                memset((void *)SendBuf,'\0',sizeof(SendBuf));    
                CH57xNET_FTPGetFileName( recv_buff,SendBuf );
                len = strlen(ListName);
                sprintf(pFileName,&SendBuf[len]); 
            }
            else{
                memset((void *)pFileName,'\0',sizeof(pFileName));    
                CH57xNET_FTPGetFileName( recv_buff,pFileName );
            } 
            ftp.ListFlag = 2;
            CH57xNET_FTPListRenew( ftp.ListFlag );
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[6],strlen(FTP_SERVICE_CMD1[6]),ftp.SocketCtlConnect );
        }
        else{
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("noop", recv_buff, 4) == 0 ){
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[8],strlen(FTP_SERVICE_CMD1[8]),ftp.SocketCtlConnect );
    }
    else if( strncmp("RNFR", recv_buff, 4) == 0 ){
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* �����Ȩ����֧��ɾ�� */
            memset((void *)SendBuf,'\0',sizeof(SendBuf));    
            if(recv_buff[5] == '/'){
                CH57xNET_FTPGetFileName( recv_buff,SendBuf );
                len = strlen(ListName);
                sprintf(pFileName,&SendBuf[len]); 
            }
            else{
                memset((void *)pFileName,'\0',sizeof(pFileName));    
                CH57xNET_FTPGetFileName( recv_buff,pFileName );
            } 
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD2[0],strlen(FTP_SERVICE_CMD2[0]),ftp.SocketCtlConnect );
        }
        else{
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("RNTO", recv_buff, 4) == 0 ){
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* �����Ȩ����֧��ɾ�� */
            memset((void *)gFileName,'\0',sizeof(gFileName));    
            if(recv_buff[5] == '/'){
                memset((void *)SendBuf,'\0',sizeof(SendBuf));    
                CH57xNET_FTPGetFileName( recv_buff,SendBuf );
                len = strlen(ListName);
                sprintf(gFileName,&SendBuf[len]); 
            }
            else{
                CH57xNET_FTPGetFileName( recv_buff,gFileName );
            } 
            ftp.ListFlag = 5;
            CH57xNET_FTPListRenew( ftp.ListFlag );
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD2[1],strlen(FTP_SERVICE_CMD2[1]),ftp.SocketCtlConnect );
        }
        else{
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else{                                                                       /* ���֧�� */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[9],strlen(FTP_SERVICE_CMD1[9]),ftp.SocketCtlConnect );
    }
}

/*********************************** endfile **********************************/