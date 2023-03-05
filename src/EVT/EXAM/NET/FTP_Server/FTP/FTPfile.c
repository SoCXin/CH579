/********************************** (C) COPYRIGHT ******************************
 * File Name          : FTPFILE.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/09/15
 * Description        : WCHNET FTP command code
 *                    : MDK3.36@ARM966E-S,Thumb,Ð¡¶Ë
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "FTPINC.h"
    
/******************************************************************************
*This program simulates files
* The initial state in the root directory is one folder and two text documents;
* The folder name is USER and contains a text document;
* The root directory supports up to four files, if more than four, the upload data will not be received;
* A file is supported in the USER directory, and the original file is directly overwritten after uploading the file;
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
* Description    : Initialization lists and files, used to simulate file and directory listings
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
            strlen(RootBuf1),FileNature,strlen(RootBuf1),ListNature);           /* Current directory */
    memset((void *)ListUser,'\0',sizeof(ListUser));    
    sprintf(ListUser,"%s%4d C.txt\r\n",FileNature,strlen(UserBuf));             /* sub-directory*/
    memset((void *)NameFile,'\0',sizeof(NameFile));    
    sprintf(NameFile,"USER\r\n");
    memset((void *)NameList,'\0',sizeof(NameList));    
    sprintf(NameList,"A.txt#0B.txt#1");
    ftp.BufStatus |= 0x03;                                                      /* buf1 and buf2 are not null */
}
                                               
/*******************************************************************************
* Function Name  : WCHNET_FTPListRenew
* Description    : update catalog
* Input          : index - the status to be updated (0- means that a new file is 
                    added, others only need to update the file size information)
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPListRenew( uint8_t index )
{
     uint8_t i,k;

    PRINT("pFileName:\n%s\n",pFileName);
    if(ftp.ListState == 0){                                                     /* Root directory */
        if(index == 2||index == 4){                                             /* Delete Files */
            i = 0;
            while((strncmp(pFileName, &ListRoot[i], strlen(pFileName))) && (i<strlen(ListRoot))) i++;
            if(i>=strlen(ListRoot)) return;                                     /* Exit if filename not found */
            k = i;
            while((ListRoot[k] != '\n')&&(k<strlen(ListRoot))) k++;             /* move to the end of the file properties */
            if(k>=strlen(ListRoot)) return;                                     /* exit with error */
            k++;
            while((ListRoot[i] != '\n')&&i) i--;                                /* Move to the beginning of this file attribute (end of previous file attribute)*/
            if(i) ListRoot[i+1] = '\0';
            else ListRoot[i] = '\0';                                            /* This file is the first file */  
            memset(SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,"%s%s",ListRoot,&ListRoot[k]);                      /* Delete deleted file attribute information */
            memset(ListRoot,'\0',sizeof(ListRoot));
            sprintf(ListRoot,SendBuf);
            if(index == 2){
                i = 0;
                while((strncmp(pFileName,&NameList[i], strlen(pFileName))) && (i<strlen(NameList))) i++;
                if(i>=strlen(NameList)) return;
                NameList[i] = '\0';
                i += strlen(pFileName);
                switch(NameList[i+1]){                                          /* clear buffer*/
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
        else if(index == 1||index == 3){                                        /* add new file */
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
        else if(index == 0||index == 5){                                        /* Replace or rename files */
            i = 0;
            while((pFileName[0] != ListRoot[i])&&i<strlen(ListRoot)) i++;
            if(i>=strlen(ListRoot)) return;
            if(index == 0){                                                     /* overwrite file */
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
            else{                                                               /* Rename */
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
                 if(i<strlen(pFileName)){                                       /* rename file */
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
                else{                                                           /* rename folder */
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
    else if(ftp.ListState == 1){                                                /* The next level directory (for the USER directory, the current directory name is stored in ListName) */
        if(index == 2){                                                         /* Delete Files */
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
        else if(index == 1){                                                    /* replace file */
            memset((void *)ListUser,'\0',sizeof(ListUser));    
            sprintf(ListUser,"%s%4d %s\r\n",FileNature,FileLen,pFileName);      /* sub-directory */
        }
        else return;                
        PRINT("ListUser:\n%s\n",ListUser);
    }
}
/*******************************************************************************
* Function Name  : WCHNET_FTPFileSize
* Description    : get file size
* Input          : pName -file name
* Output         : None
* Return         : None
*******************************************************************************/
uint32_t WCHNET_FTPFileSize( char *pName )
{
    uint8_t i;

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
* Function Name  : WCHNET_FTPFileOpen
* Description    : open file
* Input          : pName - file name
                   index - Operation object
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t WCHNET_FTPFileOpen( char *pName,uint8_t index )
{
    uint8_t k;
    char  *pList;

    if(index == FTP_MACH_LIST){                                                 /* Open the directory and check if the directory name exists */
        if(strlen(pName) == 0 ){
            ftp.ListState = 0;                                                  /* open root directory */
            return TRUE;
        }
        else if(strncmp("USER", pName,strlen(pName)) == 0 ){
            ftp.ListState = 1;                                                  /* Open the directory named USER */
            return TRUE;
        }
        else{
            k = 0;
            while(strncmp(pName,&NameFile[k],strlen(pName))){
                k++;
                if(k>=strlen(NameFile)){
                    ftp.ListState = 0xFF;                                       /* directory does not exist */ 
                    return FALSE;                                              
                }
            }
            ftp.ListState = 2;                                                  /* Open the directory named USER */
            return TRUE;
        }
    }
    else if(index == FTP_MACH_FILE){                                            /* Open the file and check if the file exists */
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
* Function Name  : WCHNET_FTPFileRead
* Description    : read name
* Input          : pName - file name
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPFileRead( char *pName )
{
    UINT32 len;

    memset((void *)SendBuf,'\0',sizeof(SendBuf));
      if(pName[0] == '0' ){                                                     /* Open list information */
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
        len = WCHNET_FTPFileSize( pName );                                    /* Get the length of the file to read */
        if(len < SendBufLen){
            memset(SendBuf,'\0',sizeof(SendBuf));
            sprintf(SendBuf,pFile);
        }
    } 
}        
/*******************************************************************************
* Function Name  : WCHNET_FTPFileWrite
* Description    : Receive file data and store in buffer
* Input          : recv_buff - data buff
                   LEN       - length
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPFileWrite( char *recv_buff, uint16_t len ) 
{
    uint8_t i,k;
    
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
        if(i>= strlen(NameList)){                                               /* new file */
            ftp.ListFlag = 1;
            for(k=0;k<4;k++){
                if(((ftp.BufStatus>>k)&0x01) == 0) break;
            }
        }
        else{
            ftp.ListFlag = 0;                                                   /* overwrite file */
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
        ftp.ListFlag = 1;                                                       /* Replace the original file directly in the next directory */
        memset((void *)UserBuf,'\0',sizeof(UserBuf));    
        sprintf(UserBuf,recv_buff);
    }
}

/*********************************** endfile **********************************/
