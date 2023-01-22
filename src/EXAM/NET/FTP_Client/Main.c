/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/05/31
 * Description        : Main program body.
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
/*
 *@Note
 The FTP example demonstrates obtaining the specified file content from the FTP server folder, 
 creating a new folder on the server, creating a new file under the folder, and writing the 
 specified content.
 */
#include "CH57x_common.h"
#include "string.h"
#include "eth_driver.h"
#include "FTPINC.h"

uint8_t MACAddr[6];                                              	//MAC address
uint8_t IPAddr[4] = { 192, 168, 1, 10 };                         	//IP address
uint8_t GWIPAddr[4] = { 192, 168, 1, 1 };                        	//Gateway IP address
uint8_t IPMask[4] = { 255, 255, 255, 0 };                        	//subnet mask
uint8_t DESIP[4] = { 192, 168, 1, 100 };                         	//destination IP address

uint8_t gPort;                                                   
uint8_t SocketId;	
uint8_t SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];      	//socket receive buffer
uint8_t MyBuf[RECE_BUF_LEN];

char  ListName[24];                                              	//Used to save the directory name
char  ListMkd[24];                                               	//Save the newly created directory name
char  FileName[24];                                              	//used to save the filename
char  CharIP[17];                                                	//Used to save the IP address converted to characters

char *pUserName  = "WCHNET";                            			//username
char *pPassword  = "123";                               			//Password
char *pSearchDir = "WCHNET";                                        //directory to be searched
char *pSearchFile = "WCHNET.txt";									//file to be searched
char *pMkDir = "FTP_TEST";											//newly created directory
char *pTouchFile = "FTP_TEST.txt";									//newly created file
char *pContent = "WCHNET FTP EXAMPLE;";                    			//Content to be uploaded


/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError(uint8_t iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (uint16_t) iError);
}
	
/*******************************************************************************
* Function Name  : ETH_IRQHandler
* Description    : This function handles ETH exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_IRQHandler( void )						                             	
{
    WCHNET_ETHIsr();							                              	
}

/*******************************************************************************
* Function Name  : TMR0_IRQHandler
* Description    : This function handles TMR0 exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TMR0_IRQHandler( void ) 					                             	
{
	WCHNET_TimeIsr(WCHNETTIMERPERIOD);                                     
	R8_TMR0_INT_FLAG |= 0xff;					                      
}

/*******************************************************************************
* Function Name  : Timer0Init
* Description    : Timer 0 initialization
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer0Init(void)
{
	R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;		                               
	R8_TMR0_CTRL_MOD = 0;						                             
	R32_TMR0_CNT_END = FREQ_SYS/1000*WCHNETTIMERPERIOD;	               
	R8_TMR0_INT_FLAG = R8_TMR0_INT_FLAG;		                          
	R8_TMR0_INTER_EN = RB_TMR_IE_CYC_END;	                              
	R8_TMR0_CTRL_MOD |= RB_TMR_COUNT_EN;
	NVIC_EnableIRQ(TMR0_IRQn);	
}

/*******************************************************************************
* Function Name  : SystemClock_UART1_init
* Description    : System clock and serial port 1 initialization
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SystemClock_UART1_init(void)
{
    PWR_UnitModCfg(ENABLE, UNIT_SYS_PLL);                                      
    DelayMs(3); 
    SetSysClock(CLK_SOURCE_HSE_32MHz);                                          
    GPIOA_SetBits( GPIO_Pin_9 );
    GPIOA_ModeCfg( GPIO_Pin_9, GPIO_ModeOut_PP_5mA );                           
	UART1_DefInit( );                                                            
}

/*********************************************************************
 * @fn      WCHNET_CreateFTPCtrlClient
 *
 * @brief   Create FTP Ctrl Client socket
 *
 * @return  none
 */
void WCHNET_CreateFTPCtrlClient(void)
{
	uint8_t i;
	SOCK_INF TmpSocketInf;

	memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                           
	memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);                               
	TmpSocketInf.DesPort = 21;                                            
	TmpSocketInf.SourPort = 4000;                                          
	TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     
	TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[0];                     
	TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     
	i = WCHNET_SocketCreat( &SocketId,&TmpSocketInf );                           
	mStopIfError(i);                                                           
	ftp.SocketCtl = SocketId;    
	printf("SocketId TCP FTP = %02x\n",(UINT16)ftp.SocketCtl);
	i = WCHNET_SocketConnect(SocketId);                                     
	mStopIfError(i);                                                             
	memset((void *)SocketRecvBuf[SocketId],'\0',sizeof(SocketRecvBuf[SocketId]));              
}

/*******************************************************************************
* Function Name  : WCHNET_CreateFTPDatClient
* Description    : Create FTP Data Client socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_CreateFTPDatClient( UINT16 *port)
{
	uint8_t i;                                                             
	SOCK_INF TmpSocketInf;                                                       

	if(gPort > 200) gPort = 0;
	gPort++;
	memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                           
	memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);                              
	TmpSocketInf.DesPort =  20;                                                 
	TmpSocketInf.SourPort = 4001;                                              
	*port = TmpSocketInf.SourPort; 
	TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                   
	TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[1];                      
	TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                    
	i = WCHNET_SocketCreat( &SocketId,&TmpSocketInf );                         
	mStopIfError(i);                                                          
	ftp.DatTransfer = SocketId; 
	printf("SocketId FTP DATA = %02x\n",(UINT16)ftp.DatTransfer);   
	i = WCHNET_SocketConnect(SocketId);                                    
	mStopIfError(i);                                                          
}

/*******************************************************************************
* Function Name  : WCHNET_FTPDatServer
* Description    : Create FTP Server socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPDatServer( UINT16 *port )
{
	uint8_t i;
	SOCK_INF TmpSocketInf;                                                       

	if(gPort > 200) gPort = 0;
	gPort++;
	memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                           
	TmpSocketInf.SourPort = 4000 + gPort;                                        
	*port = TmpSocketInf.SourPort; 
	TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                    
	i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                          
	mStopIfError(i);                                                          
	i = WCHNET_SocketListen(SocketId);                                       
	printf("SocketId dat server:%d\n",(UINT16)SocketId);    
	ftp.DatMonitor = SocketId;
    mStopIfError(i);                                                                    
}

/*******************************************************************************
* Function Name  : WCHNET_FTPSendData
* Description    : FTP send data
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPSendData( char *PSend, uint32_t Len,uint8_t index  )
{
    uint32_t length;
    uint8_t i,*p,count;

    p = (uint8_t *)PSend;
    length = Len;
    count = 0;
    while(1){
        Len = length;
        i = WCHNET_SocketSend(index,p,&Len);                                 
        mStopIfError(i);                                                     
        if(Len == 0){
            count++;
            if(count>2){
				printf("Send Data fail\n");   
                return;
            } 
        }
        length -= Len;                                                          
        p += Len;                                                            
        if(length)continue;                                                  
        break;                                                         
    }
}

/*******************************************************************************
* Function Name  : WCHNET_FTPInitVari
* Description    : variable initialization
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPInitVari(void)
{
    uint8_t i,j;

    memset((void *)&ftp,0,sizeof(ftp));                   
    ftp.SocketCtl = 255; 
    ftp.DatMonitor = 255;
    ftp.DatTransfer = 255;
    ftp.TcpStatus = 255;
    ftp.FileCmd = FTP_CMD_LOGIN;                                    //set login command
    memset((void *)CharIP,'\0',sizeof(CharIP));                   
    j = 0;
    for(i=0;i<4;i++){                                               //Convert the decimal IP address to the required character format
        if( IPAddr[i]/100 ){
            CharIP[j++] = IPAddr[i]/100 + '0';
            CharIP[j++] = (IPAddr[i]%100)/10 + '0' ;
            CharIP[j++] = IPAddr[i]%10 + '0';
        } 
        else if( IPAddr[i]/10 ){
            CharIP[j++] = IPAddr[i]/10 + '0';
            CharIP[j++] = IPAddr[i]%10 + '0';
        } 
        else CharIP[j++] = IPAddr[i]%10 + '0';
         CharIP[j++] = ',';
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt(uint8_t socketid, uint8_t intstat)
{
	uint32_t len;

    if (intstat & SINT_STAT_RECV)                              			//receive data
    {
        PRINT("ftp sockid:%d\r\n",socketid);
        len = WCHNET_SocketRecvLen(socketid,NULL);                      //query length
        if(len){
			ftp.RecDatLen = len; 
			WCHNET_SocketRecv(socketid,SocketRecvBuf[socketid],&len);   //Read the data of the receive buffer into Buff
			PRINT("check type= %02x\r\n",(UINT16)ftp.FileCmd);
			PRINT("len= %08d\n",ftp.RecDatLen);
			PRINT("LOG:\r\n%s\r\n",(char *)SocketRecvBuf[socketid]);
			WCHNET_FTPProcessReceDat((char *)SocketRecvBuf[socketid],ftp.FileCmd,socketid);
			memset((void *)SocketRecvBuf[socketid],'\0',sizeof(SocketRecvBuf[socketid]));            
        }  
    }
    if(intstat & SINT_STAT_CONNECT)                                        //TCP connect
    {                                                               
        PRINT("TCP Connect Success\r\n");                           
        if( (socketid != ftp.DatMonitor) && (socketid != ftp.SocketCtl))
        {
            ftp.DatTransfer = socketid;
            ftp.TcpStatus = FTP_MACH_CONNECT;
            PRINT("WCHNET_FTPGetSockeID:%d\r\n",ftp.DatTransfer); 
            WCHNET_ModifyRecvBuf(socketid,(UINT32)SocketRecvBuf[socketid],RECE_BUF_LEN);  
        }			
    }
    if(intstat & SINT_STAT_DISCONNECT)                                      //TCP disconnect
    {                                                               
        PRINT("TCP Disconnect\r\n");                                       
        if(socketid == ftp.DatTransfer){
            ftp.TcpStatus = FTP_MACH_DISCONT;
            ftp.CmdDataS = FTP_MACH_DATAOVER;
            if(ftp.DatMonitor != 255){
            WCHNET_SocketClose( ftp.DatMonitor,TCP_CLOSE_NORMAL );
            }
        }
    }
    if (intstat & SINT_STAT_TIM_OUT)                             			//timeout disconnect
    {
		PRINT("TCP Timout\r\n");   
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void)
{
    uint8_t intstat;
    uint16_t i;
    uint8_t socketint;

    intstat = WCHNET_GetGlobalInt();                              //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                              //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                            //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (intstat & GINT_STAT_PHY_CHANGE)                           //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if (intstat & GINT_STAT_SOCKET)                               //socket related interrupt
    {                             
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    uint8_t i;
	
	SystemClock_UART1_init();                                    //USART initialize
    printf("FTP Test\r\n");
    printf("net version:%x\n", WCHNET_GetVer());
    if ( WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                  //get the chip MAC address
    printf("mac addr:");
    for ( i = 0; i < 6; i++)
        printf("%x ", MACAddr[i]);
    printf("\n");
    Timer0Init(); 
	WCHNET_FTPInitVari();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);          //Ethernet library initialize
    mStopIfError(i);
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
                        
    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        WCHNET_MainTask();
        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        if(WCHNET_QueryGlobalInt())
        {
            WCHNET_HandleGlobalInt();
        }
		WCHNET_FTPClientCmd( );                                   //Query status, execute commands
    }
}
