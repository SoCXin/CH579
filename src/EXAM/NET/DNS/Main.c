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
 DNS example, demonstrating that after DHCP automatically obtains an IP,
 then requesting domain name resolution
 */
#include "CH57x_common.h"
#include "string.h"
#include "eth_driver.h"

uint8_t MACAddr[6];                                      //MAC address
uint8_t IPAddr[4]   = {0, 0, 0, 0};                      //IP address
uint8_t GWIPAddr[4] = {0, 0, 0, 0};                      //Gateway IP address
uint8_t IPMask[4]   = {0, 0, 0, 0};                      //subnet mask
uint8_t DESIP[4]    = {255, 255, 255, 255};              //destination IP address
uint16_t DnsPort = 53;
__attribute__((__aligned__(4))) uint8_t RemoteIp[4];

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
    if (intstat & SINT_STAT_RECV)                              //receive data
    {
    }
    if (intstat & SINT_STAT_CONNECT)                           //connect successfully
    {
        printf("TCP Connect Success\r\n");
    }
    if (intstat & SINT_STAT_DISCONNECT)                        //disconnect
    {
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                           //timeout disconnect
    {
        printf("TCP Timeout\r\n");
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
 * @fn      WCHNET_DNSCallBack
 *
 * @brief   DNSCallBack
 *
 * @return  none
 */
void WCHNET_DNSCallBack(const char *name, uint8_t *ipaddr, void *callback_arg)
{
    if(ipaddr == NULL)
    {
        printf("DNS Fail\r\n");
        return;
    }
    printf("Host Name = %s\r\n", name);
    printf("IP= %d.%d.%d.%d\r\n", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    if(callback_arg != NULL)
    {
        printf("callback_arg = %02x\r\n", (*(uint8_t *)callback_arg));
    }
    WCHNET_DNSStop();                                                          //stop DNS,and release socket
}

/*********************************************************************
 * @fn      WCHNET_DHCPCallBack
 *
 * @brief   DHCPCallBack
 *
 * @param   status - status returned by DHCP
 *          arg - Data returned by DHCP
 *
 * @return  DHCP status
 */
uint8_t WCHNET_DHCPCallBack(uint8_t status, void *arg)
{
    uint8_t *p;

    if(!status)
    {
        p = arg;
        printf("DHCP Success\r\n");
        memcpy(IPAddr, p, 4);
        memcpy(GWIPAddr, &p[4], 4);
        memcpy(IPMask, &p[8], 4);
        printf("IPAddr = %d.%d.%d.%d \r\n", (uint16_t)IPAddr[0], (uint16_t)IPAddr[1],
               (uint16_t)IPAddr[2], (uint16_t)IPAddr[3]);
        printf("GWIPAddr = %d.%d.%d.%d \r\n", (uint16_t)GWIPAddr[0], (uint16_t)GWIPAddr[1],
               (uint16_t)GWIPAddr[2], (uint16_t)GWIPAddr[3]);
        printf("IPAddr = %d.%d.%d.%d \r\n", (uint16_t)IPMask[0], (uint16_t)IPMask[1],
               (uint16_t)IPMask[2], (uint16_t)IPMask[3]);
        printf("DNS1: %d.%d.%d.%d \r\n", p[12], p[13], p[14], p[15]);            	//DNS server address provided by the router
        printf("DNS2: %d.%d.%d.%d \r\n", p[16], p[17], p[18], p[19]);

        WCHNET_InitDNS(&p[12], DnsPort);                                         	//Set DNS server IP address, and DNS server port is 53
        WCHNET_HostNameGetIp("www.wch.cn", RemoteIp, WCHNET_DNSCallBack, NULL);  	//Start DNS
        return SUCCESS;
    }
    else
    {
        printf("DHCP Fail %02x \r\n", status);
        return FAILED;
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
    printf("DNS Test\r\n");
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
    WCHNET_DHCPStart(WCHNET_DHCPCallBack);                       //Start DHCP
	
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
