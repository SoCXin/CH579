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
 TCP Client example, demonstrating that the TCP client receives data after 
 connecting to the server and then sends it back
 */
#include "CH57x_common.h"
#include "string.h"
#include "eth_driver.h"

#define KEEPLIVE_ENABLE         1                                //Enable keeplive function

uint8_t MACAddr[6];                                              //MAC address
uint8_t IPAddr[4] = { 192, 168, 1, 10 };                         //IP address
uint8_t GWIPAddr[4] = { 192, 168, 1, 1 };                        //Gateway IP address
uint8_t IPMask[4] = { 255, 255, 255, 0 };                        //subnet mask
uint8_t DESIP[4] = { 192, 168, 1, 100 };                         //destination IP address
uint16_t desport = 1000;                                         //destination port
uint16_t srcport = 1000;                                         //source port

uint8_t SocketId;
uint8_t socket[WCHNET_MAX_SOCKET_NUM];                           //Save the currently connected socket
uint8_t SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];      //socket receive buffer
uint8_t MyBuf[RECE_BUF_LEN];

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
 * @fn      WCHNET_CreateTcpSocket
 *
 * @brief   Create TCP Socket
 *
 * @return  none
 */
void WCHNET_CreateTcpSocket(void)
{
    uint8_t i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    memcpy((void *) TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport++;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    printf("WCHNET_SocketCreate %d\r\n", SocketId);
    mStopIfError(i);
    i = WCHNET_SocketConnect(SocketId);                        //make a TCP connection
    mStopIfError(i);
}

/*********************************************************************
 * @fn      WCHNET_DataLoopback
 *
 * @brief   Data loopback function.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void WCHNET_DataLoopback(uint8_t id)
{
#if 1
    uint8_t i;
    uint32_t len;
    uint32_t endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;         //Receive buffer end address

    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr) {           //Calculate the length of the received data
        len = endAddr - SocketInf[id].RecvReadPoint;
    }
    else {
        len = SocketInf[id].RecvRemLen;
    }
    i = WCHNET_SocketSend(id, (uint8_t *) SocketInf[id].RecvReadPoint, &len);           //send data
    if (i == WCHNET_ERR_SUCCESS) {
        WCHNET_SocketRecv(id, NULL, &len);                                              //Clear sent data
    }
#else
    uint32_t len, totallen;
    uint8_t *p = MyBuf;

    len = WCHNET_SocketRecvLen(id, NULL);                                               //query length
    WCHNET_SocketRecv(id, MyBuf, &len);                                                 //Read the data of the receive buffer into MyBuf
    totallen = len;
    while(1){
        len = totallen;
        WCHNET_SocketSend(id, p, &len);                                                 //Send the data
        totallen -= len;                                                                //Subtract the sent length from the total length
        p += len;                                                                       //offset buffer pointer
        if(totallen) continue;                                                          //If the data is not sent, continue to send
        break;                                                                          //After sending, exit
    }
#endif
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
    uint8_t i;

    if (intstat & SINT_STAT_RECV)                              //receive data
    {
        WCHNET_DataLoopback(socketid);                         //Data loopback
    }
    if (intstat & SINT_STAT_CONNECT)                           //connect successfully
    {
#if KEEPLIVE_ENABLE
        WCHNET_SocketSetKeepLive(socketid, ENABLE);
#endif
        WCHNET_ModifyRecvBuf(socketid, (uint32_t) SocketRecvBuf[socketid], RECE_BUF_LEN);
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            if (socket[i] == 0xff) {                           //save connected socket id
                socket[i] = socketid;
                break;
            }
        }
        printf("TCP Connect Success\r\n");
        printf("socket id: %d\r\n", socket[i]);
    }
    if (intstat & SINT_STAT_DISCONNECT)                         //disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {           //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                             //timeout disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {            //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        printf("TCP Timeout\r\n");
        WCHNET_CreateTcpSocket();
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
	
	SystemClock_UART1_init();                                   //USART initialize
    printf("TcpClient Test\r\n");
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
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);          //Ethernet library initialize
    mStopIfError(i);
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
#if KEEPLIVE_ENABLE                                              //Configure keeplive parameters
    {
        struct _KEEP_CFG cfg;

        cfg.KLIdle = 20000;
        cfg.KLIntvl = 15000;
        cfg.KLCount = 9;
        WCHNET_ConfigKeepLive(&cfg);
    }
#endif
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        WCHNET_CreateTcpSocket();                                //Create TCP Socket
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
    }
}
