/********************************** (C) COPYRIGHT ******************************
* File Name          : FTPCMD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH579芯片FTP命令代码
*                    : MDK3.36@ARM966E-S,Thumb,小端
*******************************************************************************/



/******************************************************************************/
const char FTP_SERVICE_CMD[10][60] = 
{
    "220 Microsoft FTP Service\r\n",                                            /* 0 登陆 */
    "226 Transfer complete\r\n",                                                /* 1 传输完成 */ 
    "331 access allowed send identity as password.\r\n",                        /* 2 用户名正确 */
    "230 usser logged in.\r\n",                                                 /* 3 登陆成功 */
    "530 usser cannot log in.\r\n",                                             /* 4 登陆失败 */
    "200 PORT command successful.\r\n",                                         /* 5 PORT命令成功 */
    "125 Data connection already open;Transfer starting\r\n",                   /* 6 获取列表命令 */
    "150 Opening ASCII mode data connection for",                               /* 7 下载/上传文件命令成功 */
    "550 The ch579 cannot find the file specified",                             /* 8 文件不存在 */
    "221 Goodbye\r\n",                                                          /* 9 退出 */
};
const char  FTP_SERVICE_CMD1[10][40] = 
{
    "200 Type set to",                                                          /* 0 TYPE设置文件传输类型 */
    "215 Windows_NT\r\n",                                                       /* 1 syst系统类型 */
    "257 \"/\" is current directory.\r\n",                                      /* 2 PWD打印目录 */
    "250 command successful.\r\n",                                              /* 3 命令成功 */
    "227 Entering Passive Mode(",                                               /* 4 PASV命令成功 */
    "213 ",                                                                     /* 5 size命令成功 */
    "250 DELE command success\r\n",                                             /* 6 删除 */
    "550 Access is denied.\r\n",                                                /* 7 访问被拒绝 */
    "200 noop command success.\r\n",                                            /* 8 空命令 */
    "500 command not understood\r\n",                                           /* 9 命令不支持 */
};
const char  FTP_SERVICE_CMD2[2][50] = 
{
    "350 File exists,ready for destination name\r\n",                           
    "250 RNTO command successful\r\n",                             
};                                                 
#define SendBufLen                 500                                          /* 发送缓存区长度 */ 
FTP   ftp;
char  SendBuf[SendBufLen];                                                      /* 用于保存目的端传送的IP地址 */
UINT8 CmdPortIP[4];                                                             /* 用于保存转换成字符的IP地址 */ 
UINT16 TemAddr;                                                                 /* 用于分析IP地址及端口的临时变量 */
UINT16 NumPort;                                                                 /* 端口号 */
UINT32 gLEN;                                                                    /* 文件长度 */
/* 用于保存文件名 */
char  gFileName[16];                                                            /* 用于保存请求下载的文件名 */
char  pFileName[16];                                                            /* 用于保存请求上传的文件名 */
char  ListName[16];                                                             /* 用于保存当前目录 */
/*******************************************************************************
* Function Name  : CH57xNET_FTPGetPortNum
* Description    : 获取端口号及IP地址
* Input          : recv_buff -接收到的数据
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
            if(i<4){                                                            /* 获取目的IP地址 */ 
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
            else{                                                               /* 获取端口号 */ 
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
* Description    : 获取名字
* Input          : recv_buff -接收到的数据
                   pBuf      -保存名字的缓冲区
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPGetFileName( char *recv_buff,char *pBuf )
{
     UINT8 k,i;
    
    k = 0;
    while((recv_buff[k] != ' ')&&k<strlen(recv_buff)) k++;                      /* 找到要获取的内容的位置 */
    k++;
    i = 0;
    while((recv_buff[k] != '\r')&&k<strlen(recv_buff)){                         /* 保存所需的内容 */
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
* Description    : 根据client所发的命令，返回相应的回复
* Input          : recv_buff -接收到客户端发送的命令代码
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPCmdRespond( char *recv_buff ) 
{
    UINT8  s;
    UINT8  len;
    UINT16 port;
    
    if( strncmp("USER", recv_buff, 4) == 0 ){                                   /* 用户名 */
        memset((void *)UserName,'\0',sizeof(UserName));    
        CH57xNET_FTPGetFileName( recv_buff,UserName );    
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[2],strlen(FTP_SERVICE_CMD[2]),ftp.SocketCtlConnect );
    }
    else if( strncmp("PASS", recv_buff, 4) == 0 ){                              /* 密码 */
#if Access_Authflag                                                             /* 如果需要检测用户名 */
        if(strncmp(pUserName, UserName, strlen(pUserName)) == 0){               /* 用户名正确 */
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[3],strlen(FTP_SERVICE_CMD[3]),ftp.SocketCtlConnect );
        }
        else{                                                                   /* 用户名错误 */
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[4],strlen(FTP_SERVICE_CMD[4]),ftp.SocketCtlConnect );
        }
#else
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[3],strlen(FTP_SERVICE_CMD[3]),ftp.SocketCtlConnect );
        ftp.AuthFlag = 1;
#endif
        memset((void *)UserName,'\0',sizeof(UserName));    
        CH57xNET_FTPGetFileName( recv_buff,UserName );
        if(strncmp(pPassWord, UserName, strlen(pPassWord)) == 0){               /* 验证密码是否正确（符合条件则有写文件权限）*/
            ftp.AuthFlag = 1;
printf("have more authority\n");
        }    
    }
    else if( strncmp("PORT", recv_buff, 4) == 0 ){                              /* 客户端发送客户端穿件的TCP SERVER端口号及IP地址 */
        CH57xNET_FTPGetPortNum( recv_buff );
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[5],strlen(FTP_SERVICE_CMD[5]),ftp.SocketCtlConnect );
        CH57xNET_FTPClientDat( NumPort, CmdPortIP);          
    }
    else if( strncmp("LIST", recv_buff, 4) == 0 ){                              /* 获取当前列表信息 */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[6],strlen(FTP_SERVICE_CMD[6]),ftp.SocketCtlConnect );
        CH57xNET_FTPDataReady( "0" );
    }
    else if( strncmp("STOR", recv_buff, 4) == 0){                               /* 客户端请求上传数据的命令（需要接收数据）*/
        if(ftp.AuthFlag == 1){                                                  /* 如果有权限则支持上传 */
            ftp.CmdReceDatS = 1;
            ftp.FileFlag = 0;
            memset((void *)pFileName,'\0',sizeof(pFileName));    
            CH57xNET_FTPGetFileName( recv_buff,pFileName );
            memset((void *)SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"%s %s.\r\n",FTP_SERVICE_CMD[7],pFileName);
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
        }
        else{                                                                   /* 无权限不支持的命令，关闭连接 */
            CH57xNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            CH57xNET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("RETR", recv_buff, 4) == 0){                               /* 客户端请求下载数据的命令（需要发送数据）*/
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
        if( s ){                                                                /* 文件存在，开始传输数据 */
            sprintf(SendBuf,"%s %s.\r\n",FTP_SERVICE_CMD[7],gFileName);
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            CH57xNET_FTPDataReady( gFileName );
        }
        else{                                                                   /* 文件存在，发送错误响应 */
            sprintf(SendBuf,"%s（%s).\r\n",FTP_SERVICE_CMD[8],gFileName); 
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            s = CH57xNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            mStopIfError(s); 
            ftp.SocketDatMonitor = 255;
        }
    }
    else if( strncmp("QUIT", recv_buff, 4) == 0 ){                              /* 退出登录 */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD[9],strlen(FTP_SERVICE_CMD[9]),ftp.SocketCtlConnect );
    }
    else if( strncmp("TYPE", recv_buff, 4) == 0 ){                              /* 传输的数据类型 */
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        sprintf(SendBuf,"%s %c.\r\n",FTP_SERVICE_CMD1[0],recv_buff[5]);
        CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
    }
    else if( strncmp("syst", recv_buff, 4) == 0 ){                              /* 系统类型 */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[1],strlen(FTP_SERVICE_CMD1[1]),ftp.SocketCtlConnect );
    }
    else if( strncmp("PWD", recv_buff, 3) == 0 ){                               /* 打印目录 */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[2],strlen(FTP_SERVICE_CMD1[2]),ftp.SocketCtlConnect );
    }
    else if( strncmp("CWD", recv_buff, 3) == 0 ){                               /* 打开目录 */
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
    else if( strncmp("PASV", recv_buff, 4) == 0 ){                              /* 客户端用于获取服务器创建的TCP端口号及IP地址 */
        CH57xNET_FTPServerDat( &port );
        memset((void *)SendBuf,'\0',sizeof(SendBuf));
        sprintf(SendBuf,"%s%s%02d.%02d).\r\n",FTP_SERVICE_CMD1[4],SourIP,(port/256),(port%256));
        CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
    }
    else if( strncmp("SIZE", recv_buff, 4) == 0 ){                              /* 获取文件大小 */
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
    else if( strncmp("MKD", recv_buff, 3) == 0 ){                               /* 创建目录 */
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* 如果有权限则支持上传 */
            memset((void *)pFileName,'\0',sizeof(pFileName));    
            CH57xNET_FTPGetFileName( recv_buff,pFileName );
            memset((void *)SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"257 MKD %s.\r\n",pFileName);
            CH57xNET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketCtlConnect );
            ftp.ListFlag = 3;
            CH57xNET_FTPListRenew( ftp.ListFlag );
        }
        else{                                                                   /* 无权限不支持的命令，关闭连接 */
            CH57xNET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            CH57xNET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
            CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[7],strlen(FTP_SERVICE_CMD1[7]),ftp.SocketCtlConnect );
        }
    }
    else if( strncmp("RMD", recv_buff, 3) == 0 ){                               /* 删除文件 */
        if(ftp.AuthFlag == 1){                                                  /* 如果有权限则支持删除 */
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
    else if( strncmp("DELE", recv_buff, 4) == 0 ){                              /* 删除文件 */
        if(ftp.AuthFlag == 1){                                                  /* 如果有权限则支持删除 */
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
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* 如果有权限则支持删除 */
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
        if((ftp.AuthFlag == 1)&&(ftp.ListState == 0)){                          /* 如果有权限则支持删除 */
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
    else{                                                                       /* 命令不支持 */
        CH57xNET_FTPSendData( (char *)FTP_SERVICE_CMD1[9],strlen(FTP_SERVICE_CMD1[9]),ftp.SocketCtlConnect );
    }
}

/*********************************** endfile **********************************/
