/********************************** (C) COPYRIGHT *********************************
 * File Name          : FTPCMD.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/09/15
 * Description        :  WCHNET FTP CMD                   
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "wchnet.h"
#include "FTPINC.h"

FTP  ftp;
char send_buff[536];    // send data buffer
uint32_t ListNum = 0;

/*******************************************************************************
* Function Name  : WCHNET_FTPLoginUser
* Description    : Authentication username   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPLoginUser( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "USER %s\r\n", pUserName);
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPLoginPass
* Description    : Authentication password  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPLoginPass( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "PASS %s\r\n", pPassword);
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPSyst
* Description    : Query server system type  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPSyst( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "SYST\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPList
* Description    : Get file list, file name and time  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPList( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "LIST\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPNlst
* Description    : get file list
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPNlst( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "NLIST\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPType
* Description    : Set file transfer type   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPType( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "TYPE A\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPSize
* Description    : Get file information  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPSize( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "SIZE %s\r\n",FileName);
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPRetr
* Description    : download file  
* Input          : None
* Output         : None                      
* Return         : None
*******************************************************************************/
void WCHNET_FTPRetr( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "RETR %s\r\n",FileName);
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPCdup
* Description    : Return to the previous directory  
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void WCHNET_FTPCdup( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "CDUP\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPMkd
* Description    : Create a directory 
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void WCHNET_FTPMkd(uint8_t ReName )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    if(ReName){
        ListNum++;
        sprintf( send_buff, "MKD %s%d\r\n",ListName,ListNum);
    }
    else sprintf( send_buff, "MKD %s\r\n",ListName);
    sprintf( ListMkd, &send_buff[4] );                                            //save filename
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPRmd
* Description    : delete directory
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void WCHNET_FTPRmd( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "RMD %s\r\n",ListName); 
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPAppe
* Description    : Store files 
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void WCHNET_FTPAppe( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "APPE %s\r\n",FileName);
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPAllo
* Description    : Allocate storage space for files on the server 
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void WCHNET_FTPAllo( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "ALLO 500000\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPNoop
* Description    : Check if the server is working  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPNoop( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "NOOP\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPPasv
* Description    : Passively obtain the connection port number?  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPPasv( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "PASV\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl);
}

/*******************************************************************************
* Function Name  : WCHNET_FTPPort
* Description    : Set the connection port number  
* Input          : port 
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPPort( UINT16 port)
{
    memset( send_buff, '\0', sizeof(send_buff));
    sprintf( send_buff, "PORT %s%d,%d\r\n",CharIP,(uint8_t)(port/256),(uint8_t)(port%256));
#if DEBUG
    printf("PORT :%s\n", send_buff);
#endif
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPCwd
* Description    : change to another directory  
* Input          : index -0 Current directory, 1 sub-directory
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPCwd( uint8_t index )
{                                                     
    memset( send_buff, '\0', sizeof(send_buff) );
    if(index==1) sprintf( send_buff, "CWD /%s\r\n",ListMkd);
    else sprintf( send_buff, "CWD /%s\r\n",ListName);
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPQuit
* Description    : sign out
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPQuit( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "QUIT\r\n");
    WCHNET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : WCHNET_FTPCheckLogin
* Description    : Login handshake information
* Input          : recv_buff- handshake information
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPCheckLogin( char *recv_buff )
{
    if( strncmp("220", recv_buff, 3) == 0 ){                                    //successfully connected
        WCHNET_FTPLoginUser( );
    }
    else if( strncmp("331", recv_buff, 3) == 0 ){                               //Username is correct
        WCHNET_FTPLoginPass( );                   
    }
    else if( strncmp("230", recv_buff, 3) == 0 ){                               //login successful
        ftp.CmdStatus = FTP_MACH_LOGINSUC;
    }
    else if( strncmp("530", recv_buff, 3) == 0 ){                               //Login failed
        WCHNET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               //sign out
        WCHNET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 	//close the connection
    }
    else return FTP_ERR_LOGIN;
    return FTP_CHECK_SUCCESS; 
}

/*******************************************************************************
* Function Name  : WCHNET_FTPCheckSearch
* Description    : Check file handshake information
* Input          : recv_buff- handshake information
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPCheckSearch( char *recv_buff )
{

	if( strncmp("200 PORT", recv_buff, 8 ) == 0){                         		 //PORT 
        WCHNET_FTPList( );
    }
    else if( strncmp("200 ", recv_buff, 3) == 0 ){                         		 //TYPE
        WCHNET_FTPDatServer( &ftp.SouPort );
        WCHNET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("150 ", recv_buff, 3) == 0 ){                               //file status is OK
        ftp.CmdDataS = FTP_MACH_RECEDATA;
    }
    else if( strncmp("226 ", recv_buff, 3) == 0 ){                               //close data connection
        if(ftp.InterCmdS == FTP_MACH_FINDFILE) ftp.CmdStatus = FTP_MACH_SEARCHSUC;
        ftp.InterCmdS = 0;
    }
    else if( strncmp("425 ", recv_buff, 3) == 0 ){                               //Could not open data connection
        WCHNET_FTPQuit( );
    }                                                            
    else if( strncmp("250 ", recv_buff, 3) == 0 ){                               //Request file action succeeded
        ftp.InterCmdS = FTP_MACH_FINDFILE;
        WCHNET_FTPType( );
    }
    else if( strncmp("221 ", recv_buff, 3) == 0 ){                               //service shut down
        WCHNET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 	 //close the connection
    }
    else return FTP_ERR_SEARCH;
    return FTP_CHECK_SUCCESS;
}

/*******************************************************************************
* Function Name  : WCHNET_FTPCheckGetfile
* Description    : Check download file handshake information
* Input          : recv_buff - handshake information
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPCheckGetfile( char *recv_buff )
{ 
	if( strncmp("200 PORT", recv_buff, 8) == 0 ){                          		//PORT
        WCHNET_FTPRetr( );
    }	
    else if( strncmp("213", recv_buff, 3) == 0 ){                               //file status
        WCHNET_FTPDatServer( &ftp.SouPort );
        WCHNET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("200 ", recv_buff, 3) == 0 ){                              //TYPE
        WCHNET_FTPSize( );
    }
    else if( strncmp("150", recv_buff, 3) == 0 ){                               //file status is OK
        ftp.CmdDataS = FTP_MACH_RECEDATA;
        memset( send_buff, '\0', sizeof(send_buff) );
    }
    else if(strncmp("550", recv_buff, 3) == 0 ){                                //Action not completed; file not available
        WCHNET_FTPQuit( );
    }
    else if(strncmp("226", recv_buff,3) == 0 ){                                 //transfer completed
        ftp.CmdStatus = FTP_MACH_GETSUC;                     
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               //service shut down
        WCHNET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 	//close the connection
    }
    else return FTP_ERR_GETFILE;
    return (FTP_CHECK_SUCCESS);

}

/*******************************************************************************
* Function Name  : WCHNET_FTPCheckPutfile
* Description    : Check upload file handshake information
* Input          : recv_buff- handshake information
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPCheckPutfile( char *recv_buff )
{
    if( strncmp("250", recv_buff, 3) == 0 ){                                    //Request file action succeeded
        if(ftp.InterCmdS == FTP_MACH_MKDLIST){
            WCHNET_FTPMkd( 0 );
        }
        else if(ftp.InterCmdS == FTP_MACH_PUTFILE){
            WCHNET_FTPType( );
        }
    }
    else if( strncmp("257", recv_buff, 3) == 0 ){                               //Created successfully
        ftp.InterCmdS = FTP_MACH_PUTFILE;                                       //upload file
        WCHNET_FTPCwd( 1 );                                                   	//open Directory
    }
    else if( strncmp("550", recv_buff, 3) == 0 ){                               //The created directory name exists
#if    1
	/* If the directory name exists, create another directory name 
		(add a number to the original directory name, such as: TEST1)*/
        WCHNET_FTPMkd( 1 );                                                   
#else
	/* If the directory name exists, open it directly */
        WCHNET_FTPCwd( 0 );                                                   
#endif                                          
    }
    if( strncmp("200 PORT", recv_buff, 8 ) == 0){                          		//PORT
        WCHNET_FTPAppe( );                                    
    }
    else if( strncmp("200 ", recv_buff, 3) == 0 ){                              //TYPE
        WCHNET_FTPDatServer( &ftp.SouPort );
        WCHNET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("150", recv_buff, 3) == 0 ){                               //The request to upload the file command succeeded
        ftp.CmdDataS = FTP_MACH_SENDDATA;                                       //send data
    }
    else if( strncmp("226", recv_buff, 3) == 0 ){                               //upload finished
        ftp.CmdStatus = FTP_MACH_PUTSUC;             
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               //sign out
        WCHNET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                   //close the connection
    }
    else return FTP_ERR_PUTFILE;
    return (FTP_CHECK_SUCCESS);
}

/*******************************************************************************
* Function Name  : WCHNET_FTPCheckRespond
* Description    : Check the Respond message
* Input          : recv_buff- data buff
                   check_type - cmd type
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPCheckRespond( char *recv_buff,uint8_t check_type )
{
    uint8_t s;
    
    if( strncmp("421", recv_buff, 3) == 0 ){                        //Server disconnects unexpectedly
        return (FTP_CHECK_SUCCESS);
    }
    switch(check_type){
        case FTP_CMD_LOGIN:                                         //Check the response information during the login command process
            s = WCHNET_FTPCheckLogin( recv_buff );
            return s;
        case FTP_CMD_SEARCH:                                        //Check the response information during the search file process
            s = WCHNET_FTPCheckSearch( recv_buff );
            return s;
        case FTP_CMD_GETFILE:                                       //Check the response information in the process of downloading the file
            s = WCHNET_FTPCheckGetfile( recv_buff );
            return s;
        case FTP_CMD_PUTFILE:                                       //Check the response information in the process of uploading the file
            s = WCHNET_FTPCheckPutfile( recv_buff );
            return s;
        default:
            return FTP_ERR_UNKW;
    }
}

/*******************************************************************************
* Function Name  : WCHNET_FTPFindList
* Description    : Check directory name
* Input          : None
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPFindList( char *pReceiveData ) 
{
    UINT32 len,i;

    len = ftp.RecDatLen;
    printf("ListName:%s\r\n",ListName);
    for(i=0;i<len;i++){
        if(strncmp(ListName,&pReceiveData[i],strlen(ListName))==0){
            printf("*********\nfind list\n*********\n");
            return FTP_CHECK_SUCCESS;                                            
        } 
    }    
    return FTP_ERR_UNLIST;
}

/*******************************************************************************
* Function Name  : WCHNET_FTPFindFile
* Description    : Check filename
* Input          : None
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPFindFile( char *pReceiveData ) 
{
    UINT32 len,i;
    len = ftp.RecDatLen;
    for(i=0;i<len;i++){
        if(strncmp(FileName,&pReceiveData[i],strlen(FileName))==0){
#if DEBUG
    printf("*********\nfind file\n*********\n");
#endif
            return FTP_CHECK_SUCCESS;                                   
        } 
    }    
    return FTP_ERR_UNFILE;
}

/*******************************************************************************
* Function Name  : WCHNET_FTPLogin
* Description    : Log in
* Input          : None
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPLogin( void )
{
    if(ftp.CmdFirtS == 0){
        ftp.CmdFirtS  = 1;
        ftp.FileCmd = FTP_CMD_LOGIN;                                         
        WCHNET_CreateFTPCtrlClient( );
    }
    if( ftp.CmdStatus == FTP_MACH_LOGINSUC ){                             
#if DEBUG
    printf("************\nlogin success\n*********\n");
#endif
        ftp.CmdFirtS = 0;
        ftp.CmdStatus = 0;
        ftp.FileCmd = 0;      
        return FTP_COMMAND_SUCCESS;    
    }    
    return FTP_COMMAND_CONTINUE;    
}

/*******************************************************************************
* Function Name  : WCHNET_FTPSearch
* Description    : search file
* Input          : pListNAme - The name of the directory where the file is located
                   pFileName - file name
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPSearch(char *pListNAme,char *pFileName )
{
    if( ftp.CmdFirtS == 0 ){
        ftp.CmdFirtS = 1;
        ftp.FileCmd = FTP_CMD_SEARCH;                                           //Enter the search file state
        ftp.InterCmdS = FTP_MACH_FINDLIST;                                      //find directory
        memset((void *)ListName,0,sizeof(ListName));        
        sprintf( ListName,pListNAme );                                         
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                     
        WCHNET_FTPType( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_SEARCHSUC){                               
		printf("**********\nsearch success\n*********\n");
		ftp.CmdFirtS = 0;
		ftp.CmdStatus = 0;
		ftp.FileCmd = 0;     
		return FTP_COMMAND_SUCCESS;
        }
    }
    return FTP_COMMAND_CONTINUE; 
}

/*******************************************************************************
* Function Name  : WCHNET_FTPGetFile
* Description    : get file
* Input          : pFileName
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPGetFile( char *pFileName )
{
    if(ftp.CmdFirtS == 0){
        ftp.CmdFirtS  = 1;
        ftp.FileCmd = FTP_CMD_GETFILE;                                          //Enter the state of getting files
        ftp.InterCmdS = FTP_MACH_GETFILE;                                       
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                         
        WCHNET_FTPType( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_GETSUC){                                 
#if DEBUG
    printf("*********\ngetfile success\n*********\n");
#endif
            ftp.CmdFirtS = 0;
            ftp.CmdStatus = 0;
            ftp.FileCmd = 0;       
            return FTP_COMMAND_SUCCESS;
        }
    }    
    return FTP_COMMAND_CONTINUE; 
}

/*******************************************************************************
* Function Name  : WCHNET_FTPPutFile
* Description    : upload files
* Input          : pListNAme - The name of the directory where the file is located
                   pFileName - file name
* Output         : None
* Return         : function execution status
*******************************************************************************/
uint8_t WCHNET_FTPPutFile(char *pListNAme, char *pFileName)
{
    if(ftp.CmdFirtS ==0){
        ftp.CmdFirtS = 1;
        ftp.FileCmd = FTP_CMD_PUTFILE;                                          //Enter the state of uploading files
        ftp.InterCmdS = FTP_MACH_MKDLIST;                                    
        memset((void *)ListName,'\0',sizeof(ListName));               
        sprintf( ListName,pListNAme );                                         
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                         
        WCHNET_FTPCdup( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_PUTSUC){                                  
#if DEBUG
    printf("*********\nputfile success\n*********\n");
#endif
            ftp.CmdFirtS = 0;
            ftp.CmdStatus = 0;
            ftp.FileCmd = 0;
            return FTP_COMMAND_SUCCESS;
        }
    }    
    return FTP_COMMAND_CONTINUE;     
}

/*********************************** endfile **********************************/
