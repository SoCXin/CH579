/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/09/15
 * Description        : Main program body.
 * Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
/*
 *@Note
 FTP server example
 */
#include "CH57x_common.h"
#include "string.h"
#include "eth_driver.h"
#include "FTPINC.h"

uint8_t MACAddr[6];                                              	//MAC address
uint8_t IPAddr[4] = { 192, 168, 1, 10 };                         	//IP address
uint8_t GWIPAddr[4] = { 192, 168, 1, 1 };                       	//Gateway IP address
uint8_t IPMask[4] = { 255, 255, 255, 0 };                        	//subnet mask

uint8_t SocketId;    
uint8_t SerPort;                                                 
uint8_t SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];      	//socket receive buffer
   
char  UserName[16];                                              
char  SourIP[17];                                                 
#if Access_Authflag
char  *pUserName = "admin";                        					//User Name
#endif
char  *pPassWord = "123";                             				//PassWord
const char *Atest = "Here is a WCHNET FTP server example;";			//Contents of Atest.txt
const char *Btest = "WCHNET FTP server demo";						//Contents of Btest.txt
const char *Ctest = "www.wch.cn 2022-09-15";						//Contents of Ctest.txt
const char *FileNature = "09-15-22 10:10PM         ";				//file properties
const char *ListNature = "09-10-22 11:00PM  <DIR>       ";			//directory properties

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

/*******************************************************************************
* Function Name  : WCHNET_FTPCtlServer
* Description    : Create TCP Server socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPCtlServer(void)
{
   uint8_t i;                                                             
   SOCK_INF TmpSocketInf;                                                      

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            
   TmpSocketInf.SourPort = 21;                                               
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                           
   mStopIfError(i);                                                             
   ftp.SocketCtlMonitor = SocketId; 
   i = WCHNET_SocketListen(SocketId);                                      
   PRINT("SocketId ctl:%d\n",(UINT16)SocketId);
   mStopIfError(i);                                                          
}

/*******************************************************************************
* Function Name  : WCHNET_FTPServerDat
* Description    : Create TCP Server socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPServerDat( UINT16 *port )
{
   uint8_t i;
   SOCK_INF TmpSocketInf;                                                      
 
   if(SerPort < 100||SerPort > 200) SerPort = 100;
   SerPort++;
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            
   TmpSocketInf.SourPort = 256*5 + SerPort;                                     
   *port = TmpSocketInf.SourPort; 
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                           
   mStopIfError(i);                                                          
   ftp.SocketDatMonitor = SocketId; 
   i = WCHNET_SocketListen(SocketId);                                        
   PRINT("SocketId dat server:%d\n",(uint16_t)SocketId);
   mStopIfError(i);                                                            
}

/*******************************************************************************
* Function Name  : WCHNET_FTPClientDat
* Description    : Create TCP Client socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPClientDat( uint16_t port,uint8_t *pAddr )
{
   uint8_t i;                                                             
   SOCK_INF TmpSocketInf;                                                       

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                           
   memcpy((void *)TmpSocketInf.IPAddr,pAddr,4);
   TmpSocketInf.SourPort = 20;                                                 
   TmpSocketInf.DesPort = port;
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                    
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                           
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[SocketId];               
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     
   ftp.SocketDatConnect = SocketId;
   PRINT("SocketId dat client:%d\n",(uint16_t)SocketId);
   mStopIfError(i);                                                           
   i = WCHNET_SocketConnect(SocketId);                                        
   mStopIfError(i);                                                             
   memset((void *)&SocketRecvBuf[SocketId],'\0',sizeof(SocketRecvBuf[SocketId]));              
}

/*******************************************************************************
* Function Name  : WCHNET_FTPSendData
* Description    : FTP send Data
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPSendData( char *PSend, uint32_t Len,uint8_t index )
{
    uint32_t length;
    uint8_t i, *p, count;

    p = (uint8_t *)PSend;
    PRINT("send buf:\n%s\n",PSend);
    length = Len;
    count = 0;
    while(1){
        Len = length;
        i = WCHNET_SocketSend(index,p,&Len);                                 
        mStopIfError(i);                                                    
        if(Len == 0){
            count++;
            if(count>2){
                PRINT("Send Data  fail\n");
                return;
            } 
        }
        length -= Len;                                                          
        p += Len;                                                              
        if(length)continue;                                                     
        break;                                                                  
    }
    PRINT("Send Data Success\n");
    ftp.TimeCount = 0;
}

/*******************************************************************************
* Function Name  : WCHNET_FTPGetSockeID
* Description    : socketid
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPGetSockeID( uint8_t socketid )
{
	uint8_t i;

	if(SocketInf[socketid].SourPort == 21){
		if(ftp.TcpCtlSta <= FTP_MACH_DISCONT){
			ftp.SocketCtlConnect = socketid;
			ftp.TcpCtlSta = FTP_MACH_CONNECT;
		}
		else{
			WCHNET_SocketClose( socketid,TCP_CLOSE_NORMAL );
		}
	}
	if(SocketInf[socketid].SourPort == 20||SocketInf[socketid].SourPort == (256*5 + SerPort)){
		if(ftp.TcpDatSta <= FTP_MACH_DISCONT){
			ftp.TcpDatSta = FTP_MACH_CONNECT;
			ftp.SocketDatConnect = socketid;
		}
		else{
			i = WCHNET_SocketClose( socketid,TCP_CLOSE_NORMAL );             
			mStopIfError( i ); 
			PRINT("ERROR: only support a socket connected\n");
		}
	}
}
/*******************************************************************************
* Function Name  : WCHNET_FTPInitVari
* Description    : variable initialization
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_FTPInitVari( )
{
    uint8_t i,j;

    memset((void *)&ftp,'\0',sizeof(ftp));
    ftp.SocketDatMonitor = 255;
    ftp.SocketDatConnect = 255;
    ftp.SocketCtlMonitor = 255;
    ftp.SocketCtlConnect = 255; 
    memset((void *)SourIP,'\0',sizeof(SourIP));                   
    j = 0;
    for(i=0;i<4;i++){                                                //Convert the decimal IP address to the required character format
        if( IPAddr[i]/100 ){
            SourIP[j++] = IPAddr[i]/100 + '0';
            SourIP[j++] = (IPAddr[i]%100)/10 + '0' ;
            SourIP[j++] = IPAddr[i]%10 + '0';
        } 
        else if( IPAddr[i]/10 ){
            SourIP[j++] = IPAddr[i]/10 + '0';
            SourIP[j++] = IPAddr[i]%10 + '0';
        } 
        else SourIP[j++] = IPAddr[i]%10 + '0';
         SourIP[j++] = ',';
    }
    mInitFtpList( );
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

	if(intstat & SINT_STAT_RECV)                                               //receive data
	{
        printf("ftp sockid:%d\r\n",socketid);
        len = WCHNET_SocketRecvLen(socketid,NULL);                             //query length
        WCHNET_SocketRecv(socketid,SocketRecvBuf[socketid],&len);              //Read the data of the receive buffer into Buff
        printf("LOG:\r\n%s\r\n",(char *)SocketRecvBuf[socketid]);
        WCHNET_FTPHandleDatRece((char *)SocketRecvBuf[socketid], socketid);
        memset((void *)SocketRecvBuf[socketid],'\0',sizeof(SocketRecvBuf[socketid]));            
	}
	if(intstat & SINT_STAT_CONNECT)                                            //TCP connect
	{                                                                           
		printf("TCP Connect Success\n"); 
        WCHNET_ModifyRecvBuf(socketid, (uint32_t) SocketRecvBuf[socketid], RECE_BUF_LEN);  
        WCHNET_FTPGetSockeID( socketid );        
	}
	if(intstat & SINT_STAT_DISCONNECT)                                         //TCP disconnect
	{                                                                           
		printf("TCP Disconnect\n");                                           
        if( socketid == ftp.SocketCtlConnect ){
            ftp.AuthFlag = 0;
            ftp.TcpCtlSta = FTP_MACH_DISCONT; 
            ftp.SocketCtlConnect = 255;
        }
        else if( socketid == ftp.SocketCtlMonitor ){
            ftp.AuthFlag = 0;
            WCHNET_FTPCtlServer( );
        } 
        else if( socketid == ftp.SocketDatConnect ){
            ftp.TcpDatSta = FTP_MACH_DISCONT;
            ftp.SocketDatConnect = 255;
        } 
        else if( socketid == ftp.SocketDatMonitor ){
            ftp.SocketDatConnect = 255;
        } 
	}
	if(intstat & SINT_STAT_TIM_OUT)                                            //timeout disconnect
	{                                                                          
		printf("TCP Timout\n");                                                  
        if( socketid == ftp.SocketCtlConnect ){
            ftp.TcpCtlSta = FTP_MACH_DISCONT; 
            ftp.AuthFlag = 0;
            ftp.SocketCtlConnect = 255;
        }
        else if( socketid == ftp.SocketCtlMonitor ){
            ftp.AuthFlag = 0;
            WCHNET_FTPCtlServer( );
        } 
        else if( socketid == ftp.SocketDatConnect ){
            ftp.TcpDatSta = FTP_MACH_DISCONT;
            ftp.SocketDatConnect = 255;
        } 
        else if( socketid == ftp.SocketDatMonitor ){
            ftp.SocketDatConnect = 255;
        }
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
	uint16_t i;
    uint8_t intstat, socketint;

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
    if (intstat & GINT_STAT_SOCKET) {                             //socket related interrupt
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
	
	SystemClock_UART1_init();                                   //USART initialize
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
	WCHNET_FTPCtlServer( );
                        
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
		WCHNET_FTPServerCmd( );                                    //Query status, execute commands
    }
}
