/********************************** (C) COPYRIGHT ******************************
* File Name          : FTPFILE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH579芯片FTP命令代码
*                    : MDK3.36@ARM966E-S,Thumb,小端
*******************************************************************************/



/******************************************************************************/
const char *Atest = "0123456789abcdefghijklmnopqrstuvwxyz";
const char *Btest = "ch579 FTP server demo";
const char *Ctest = "www.wch.cn 2013-04-21";
const char *FileNature = "04-18-13 10:10PM         ";
const char *ListNature = "04-10-13 11:00PM  <DIR>       ";
    
/******************************************************************************
*本程序模拟文件
* 根目录下初始状态为一个文件夹两个文本文档；
* 文件夹名为USER，包含一个文本文档；
* 根目录最多支持四个文件，超过四个则不接收上传数据；
* USER目录下支持一个文件，上传文件后直接覆盖原文件；
******************************************************************************/                
char  RootBuf1[64];                                                    
char  RootBuf2[64];                            
char  RootBuf3[64];            
char  RootBuf4[64];            
char  UserBuf[64];            
char  NameList[200];        
char  NameFile[100];        
char  ListUser[50];                
char  ListRoot[450];            
UINT16 FileLen;
char *pFile;

/*******************************************************************************
* Function Name  : mInitFtpVari
* Description    : 初始化列表以及文件,用于模拟文件及目录列表
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  mInitFtpList( void )
{
    memset((void *)RootBuf1,'\0',sizeof(RootBuf1));    
    sprintf(RootBuf1,Atest);
    memset((void *)RootBuf2,'\0',sizeof(RootBuf2));    
    sprintf(RootBuf2,Btest);
    memset((void *)RootBuf3,'\0',sizeof(RootBuf3));
    memset((void *)RootBuf4,'\0',sizeof(RootBuf4));
    memset((void *)UserBuf,'\0',sizeof(UserBuf));
    sprintf(UserBuf,Ctest);
    memset((void *)ListRoot,'\0',sizeof(ListRoot));
    sprintf(ListRoot,"%s%4d A.txt\r\n%s%4d B.txt\r\n%sUSER\r\n",FileNature,
            strlen(RootBuf1),FileNature,strlen(RootBuf1),ListNature);           /* 当前目录 */
    memset((void *)ListUser,'\0',sizeof(ListUser));    
    sprintf(ListUser,"%s%4d H.txt\r\n",FileNature,strlen(UserBuf));             /* 下一级目录 */
    memset((void *)NameFile,'\0',sizeof(NameFile));    
    sprintf(NameFile,"USER\r\n");
    memset((void *)NameList,'\0',sizeof(NameList));    
    sprintf(NameList,"A.txt#0B.txt#1");
    ftp.BufStatus |= 0x03;                                                      /* buf1和buf2不为空 */
}
                                               
/*******************************************************************************
* Function Name  : CH57xNET_FTPListRenew
* Description    : 更新目录
* Input          : index -要更新的状态（0-则为添加了新文件，其他只需更新文件大小信息）
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPListRenew( UINT8 index )
{
     UINT8 i,k;

    PRINT("pFileName:\n%s\n",pFileName);
    if(ftp.ListState == 0){                                                     /* 根目录 */
        if(index == 2||index == 4){                                             /* 删除文件 */
            i = 0;
            while((strncmp(pFileName, &ListRoot[i], strlen(pFileName))) && (i<strlen(ListRoot))) i++;
            if(i>=strlen(ListRoot)) return;                                     /* 没有找到文件名退出 */
            k = i;
            while((ListRoot[k] != '\n')&&(k<strlen(ListRoot))) k++;             /* 移动到该文件属性末尾 */
            if(k>=strlen(ListRoot)) return;                                     /* 出错退出 */
            k++;
            while((ListRoot[i] != '\n')&&i) i--;                                /* 移动到该文件属性开头（上一个文件属性末尾）*/
            if(i) ListRoot[i+1] = '\0';
            else ListRoot[i] = '\0';                                            /* 此文件是第一个文件 */  
            memset(SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"%s%s",ListRoot,&ListRoot[k]);                      /* 删除已删除的文件属性信息 */
            memset(ListRoot,'\0',sizeof(ListRoot));
            sprintf(ListRoot,SendBuf);
            if(index == 2){
                i = 0;
                while((strncmp(pFileName,&NameList[i], strlen(pFileName))) && (i<strlen(NameList))) i++;
                if(i>=strlen(NameList)) return;
                NameList[i] = '\0';
                i += strlen(pFileName);
                switch(NameList[i+1]){                                          /* 清除缓冲区 */
                    case '0':
                        ftp.BufStatus = ftp.BufStatus&0xfe;
                        break; 
                    case '1':
                        ftp.BufStatus = ftp.BufStatus&0xfd;
                        break; 
                    case '2':
                        ftp.BufStatus = ftp.BufStatus&0xfb;
                        break; 
                    case '3':
                        ftp.BufStatus = ftp.BufStatus&0xf7;
                        break; 
                }
                i += 2;
                sprintf(SendBuf,"%s%s",NameList,&NameList[i]);
                memset(NameList,'\0',sizeof(NameList));
                sprintf(NameList,SendBuf);
            }
            else{
                i = 0;
                while((strncmp(pFileName,&NameFile[i], strlen(pFileName))) && (i<strlen(NameFile))) i++;
                if(i>=strlen(NameFile)) return;
                NameFile[i] = '\0';
                i += strlen(pFileName);
                i += 2;
                sprintf(SendBuf,"%s%s",NameFile,&NameFile[i]);
                memset(NameFile,'\0',sizeof(NameFile));
                sprintf(NameFile,SendBuf);
                PRINT("NameFile:\n%s\n",NameFile);
            }
        }
        else if(index == 1||index == 3){                                        /* 添加新文件 */
            memset(SendBuf,'\0',sizeof(SendBuf));
            if(index == 1){
                sprintf(SendBuf,"%s%s%4d %s\r\n",ListRoot,FileNature,FileLen,pFileName);
            }
            else{
                sprintf(SendBuf,"%s%s%s\r\n",ListRoot,ListNature,pFileName);
                strcat(NameFile,pFileName);
                strcat(NameFile,"\r\n");
            }
            memset(ListRoot,'\0',sizeof(ListRoot));
            sprintf(ListRoot,SendBuf);
        }
        else if(index == 0||index == 5){                                        /* 替换文件或者重命名文件 */
            i = 0;
            while((pFileName[0] != ListRoot[i])&&i<strlen(ListRoot)) i++;
            if(i>=strlen(ListRoot)) return;
            if(index == 0){                                                     /* 覆盖文件 */
                k = i;
                i -= 2;
                while(ListRoot[i] != ' ') {
                    i--;
                    if(i < 2) return;
                }
                ListRoot[i] = '\0';
                memset(SendBuf,'\0',sizeof(SendBuf));
                sprintf(SendBuf,"%s %4d %s",ListRoot,FileLen,&ListRoot[k]);
                memset(ListRoot,'\0',sizeof(ListRoot));
                sprintf(ListRoot,SendBuf);
            }
            else{                                                               /* 重命名 */
                k = i;
                while(ListRoot[k] != '\r'){
                    k++;
                    if(k >= strlen(ListRoot)) return;
                }
                ListRoot[i] = '\0';
                memset(SendBuf,'\0',sizeof(SendBuf));
                sprintf(SendBuf,"%s%s%s",ListRoot,gFileName,&ListRoot[k]);
                memset(ListRoot,'\0',sizeof(ListRoot));
                sprintf(ListRoot,SendBuf);
                 i = 0;
                while(pFileName[i] != '.'){
                    i++;
                    if(i >= strlen(pFileName)) break;
                }
                 if(i<strlen(pFileName)){                                       /* 重命名为文件 */
                     i = 0;
                    while((strncmp(pFileName,&NameList[i], strlen(pFileName)))){
                        i++;
                        if(i >= strlen(NameList)) return;
                    }
                    k = i;
                    while(NameList[k] != '#'){
                        k++;
                        if(k >= strlen(NameList)) return;
                    }
                    NameList[i] = '\0';
                    sprintf(SendBuf,"%s%s%s",NameList,gFileName,&NameList[k]);
                    memset(NameList,'\0',sizeof(NameList));
                    sprintf(NameList,SendBuf);
                }
                else{                                                           /* 重命名为文件夹 */
                     i = 0;
                    while((strncmp(pFileName,&NameFile[i], strlen(pFileName)))){
                        i++;
                        if(i >= strlen(NameFile)) return;
                    }
                    k = i;
                    while(NameFile[k] != '\r'){
                        k++;
                        if(k >= strlen(NameFile)) return;
                    }
                    NameFile[i] = '\0';
                    sprintf(SendBuf,"%s%s%s",NameFile,gFileName,&NameFile[k]);
                    memset(NameFile,'\0',sizeof(NameFile));
                    sprintf(NameFile,SendBuf);
                }
            }
        }
        else return;                
    }
    else if(ftp.ListState == 1){                                                /* 下一级目录（为USER目录，当前目录名保存在ListName中） */
        if(index == 2){                                                         /* 删除文件 */
            i = 0;
            while((strncmp(pFileName, &ListUser[i], strlen(pFileName))) && (i<strlen(ListUser))) i++;
            if(i>=strlen(ListUser)) return;
            k = i;
            while((ListUser[k] != '\n')&&(k<strlen(ListUser))) k++;
            if(k>=strlen(ListUser)) return;
            k++;
            while((ListUser[i] != '\n')&&i) i--;
            if(i) ListUser[i+1] = '\0';
            else ListUser[i] = '\0';
            memset(SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"%s%s",ListUser,&ListUser[k]);
            memset(ListUser,'\0',sizeof(ListUser));
            sprintf(ListUser,SendBuf);
        }
        else if(index == 1){                                                    /* 替换文件 */
            memset((void *)ListUser,'\0',sizeof(ListUser));    
            sprintf(ListUser,"%s%4d %s\r\n",FileNature,FileLen,pFileName);      /* 下一级目录 */
        }
        else return;                
        PRINT("ListUser:\n%s\n",ListUser);
    }
}
/*******************************************************************************
* Function Name  : CH57xNET_FTPFileSize
* Description    : 获取文件长度
* Input          : pName -文件名
* Output         : None
* Return         : None
*******************************************************************************/
UINT32 CH57xNET_FTPFileSize( char *pName )
{
    UINT8 i;

    i = 0;
    if( ftp.ListState == 0 ){
        while((strncmp(pName, &NameList[i], strlen(pName)))){
            i++;
            if(i>=strlen(NameList)){
                return FALSE;
            }
        }
        while(NameList[i] != '#'){
            i++;
            if(i>=strlen(NameList)){
                return FALSE; 
            }
        }
        switch(NameList[i+1]){
            case '0':
                pFile = RootBuf1;
                break; 
            case '1':
                pFile = RootBuf2;
                break; 
            case '2':
                pFile = RootBuf3;
                break; 
            case '3':
                pFile = RootBuf4;
                break; 
            default:
                return FALSE;            
        }    
    } 
    else{
        while((strncmp(pName, &ListUser[i], strlen(pName)))){
            i++;
            if(i >= strlen(ListUser)){
                return FALSE;
            } 
        }
        pFile = UserBuf;
    }
    return strlen(pFile); 
}
/*******************************************************************************
* Function Name  : CH57xNET_FTPFileOpen
* Description    : 打开文件
* Input          : pName -文件名
                   index -判断打开的为文件夹还是文件
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH57xNET_FTPFileOpen( char *pName,UINT8 index )
{
    UINT8 k;
    char  *pList;

    if(index == FTP_MACH_LIST){                                                 /* 打开目录，检测目录名是否存在 */
        if(strlen(pName) == 0 ){
            ftp.ListState = 0;                                                  /* 打开根目录 */
            return TRUE;
        }
        else if(strncmp("USER", pName,strlen(pName)) == 0 ){
            ftp.ListState = 1;                                                  /* 打开名为USER目录 */
            return TRUE;
        }
        else{
            k = 0;
            while(strncmp(pName,&NameFile[k],strlen(pName))){
                k++;
                if(k>=strlen(NameFile)){
                    ftp.ListState = 0xFF;                                       /* 目录不存在 */ 
                    return FALSE;                                               /* 目录不存在 */
                }
            }
            ftp.ListState = 2;                                                  /* 打开名为USER目录 */
            return TRUE;
        }
    }
    else if(index == FTP_MACH_FILE){                                            /* 打开文件，检测文件是否存在 */
        if(ftp.ListState == 0) pList = NameList;
        else pList = ListUser;
        for(k=0;k<strlen(pList);k++){
            if(strncmp(pName,&pList[k],strlen(pName)) == 0 ){
                return TRUE;
            }    
        }
        return FALSE;
    }
    else return FALSE;
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPFileRead
* Description    : 读文件
* Input          : pName -文件名
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPFileRead( char *pName )
{
    UINT32 len;

    memset((void *)SendBuf,'\0',sizeof(SendBuf));
      if(pName[0] == '0' ){                                                     /* 打开列表信息 */
        if( ftp.ListState == 0 ){
            ftp.DataOver = TRUE;
            sprintf(SendBuf,"%s",ListRoot);
        } 
        else if( ftp.ListState == 1 ){
            ftp.DataOver = TRUE;
            sprintf(SendBuf,"%s",ListUser);
        } 
        else if(ftp.ListState == 2 ){
            ftp.DataOver = TRUE;
        }
    }
    else{
        len = CH57xNET_FTPFileSize( pName );                                    /* 获取要读取文件长度 */
        if(len < SendBufLen){
            memset(SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,pFile);
        }
    } 
}        
/*******************************************************************************
* Function Name  : CH57xNET_FTPFileWrite
* Description    : 接收文件数据，存入缓冲区
* Input          : recv_buff -数据
                   LEN       -长度
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPFileWrite( char *recv_buff, UINT16 len ) 
{
    UINT8 i,k;
    
    FileLen = len;
    i = 0;
    if(ftp.FileFlag) return;
    if(FileLen > 64) {
        ftp.FileFlag = 1;
        memset(recv_buff,'\0',strlen(recv_buff));
        sprintf(recv_buff,"only support the size of file less than 64 bytes");
        FileLen = strlen(recv_buff);    
    }
    if(ftp.ListState == 0){
        while((strncmp(pFileName, &NameList[i], strlen(pFileName)))&&(i < strlen(NameList))) i++;
        if(i>= strlen(NameList)){                                               /* 新文件 */
            ftp.ListFlag = 1;
            for(k=0;k<4;k++){
                if(((ftp.BufStatus>>k)&0x01) == 0) break;
            }
        }
        else{
            ftp.ListFlag = 0;                                                   /* 覆盖文件 */
            i = i +strlen(pFileName) + 1;
            k = NameList[i] - '0';
        } 
        switch(k){
            case 0:
                memset((void *)RootBuf1,'\0',sizeof(RootBuf1));    
                sprintf(RootBuf1,recv_buff);
                ftp.BufStatus = ftp.BufStatus|0x01;
                if(ftp.ListFlag){
                    strcat(NameList,pFileName);
                    strcat(NameList,"#0");
                }
                break;  
            case 1:
                memset((void *)RootBuf2,'\0',sizeof(RootBuf2));    
                sprintf(RootBuf2,recv_buff);
                ftp.BufStatus = ftp.BufStatus|0x02;
                if(ftp.ListFlag){
                    strcat(NameList,pFileName);
                    strcat(NameList,"#1");
                }
                break;  
            case 2:
                memset((void *)RootBuf3,'\0',sizeof(RootBuf3));    
                sprintf(RootBuf3,recv_buff);
                ftp.BufStatus = ftp.BufStatus|0x04;
                if(ftp.ListFlag){
                    strcat(NameList,pFileName);
                    strcat(NameList,"#2");
                }
                break;  
            case 3:
                memset((void *)RootBuf4,'\0',sizeof(RootBuf4));    
                sprintf(RootBuf4,recv_buff);
                ftp.BufStatus = ftp.BufStatus|0x08;
                if(ftp.ListFlag){
                    strcat(NameList,pFileName);
                    strcat(NameList,"#3");
                }
                break; 
            default:
                ftp.ListFlag = 0xff;
                break;  
        }    
    }
    else{
        ftp.ListFlag = 1;                                                       /* 下一级目录下直替换原文件 */
        memset((void *)UserBuf,'\0',sizeof(UserBuf));    
        sprintf(UserBuf,recv_buff);
    }
}

/*********************************** endfile **********************************/
