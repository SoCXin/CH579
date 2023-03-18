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
 MQTT example, this program is used to demonstrate TCP/IP-based MQTT protocol
 communication. After the MCU connects to Ethernet and MQTT server,
 it will publish a topic, then subscribe to this topic, publish messages to this
 topic, and finally receive messages sent by itself .
 */
#include "CH57x_common.h"
#include "string.h"
#include "eth_driver.h"
#include "MQTTPacket.h"

uint8_t MACAddr[6];                                //MAC address
uint8_t IPAddr[4]   = {192,168,1,10};              //IP address
uint8_t GWIPAddr[4] = {192,168,1,1};               //Gateway IP address
uint8_t IPMask[4]   = {255,255,255,0};             //subnet mask
uint8_t DESIP[4]    = {124,221,183,170};                         //MQTT server IP address,!!need to be modified manually

uint8_t SocketId;                                  //socket id
uint8_t SocketRecvBuf[RECE_BUF_LEN];               //socket receive buffer
uint8_t MyBuf[RECE_BUF_LEN];
uint16_t desport = 1883;                           //MQTT server port
uint16_t srcport = 4200;                           //source port

int pub_qos = 0;                                   //Publish quality of service
int sub_qos = 0;                                   //Subscription quality of service
char *username  = "ch32";                         //Device name, unique for each device, available "/" for classification
char *password  = "123456";                         //Server login password
char *sub_topic = "topic/1";                       //subscribed session name
char *pub_topic = "topic/1";                       //Published session name
char *payload = "WCHNET MQTT";                     //Publish content

uint8_t con_flag  = 0;                             //Connect MQTT server flag
uint8_t pub_flag  = 0;                             //Publish session message flag/
uint8_t sub_flag  = 0;                             //Subscription session flag
uint8_t tout_flag = 0;                             //time-out flag
uint16_t packetid = 5;                             //package id
uint8_t publishValid = 0;
uint16_t timeCnt = 0;
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
	if(!publishValid){            //Set the publishValid flag every 20s
		timeCnt += 10;
		if(timeCnt > 20000){
			publishValid = 1;
			timeCnt = 0;
		}
	}
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
 * @fn      Transport_Open
 *
 * @brief   open the TCP connection.
 *
 * @return  socket id
 */
uint8_t Transport_Open(void)
{
    uint8_t i;
    SOCK_INF TmpSocketInf;

    memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
    memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);
    mStopIfError(i);

    i = WCHNET_SocketConnect(SocketId);
    mStopIfError(i);
    return SocketId;
}

/*********************************************************************
 * @fn      Transport_Close
 *
 * @brief   close the TCP connection.
 *
 * @return  @ERR_T
 */
uint8_t Transport_Close(void)
{
    uint8_t i;
    i = WCHNET_SocketClose(SocketId,TCP_CLOSE_NORMAL);
    mStopIfError(i);
    return i;
}

/*********************************************************************
 * @fn      Transport_SendPacket
 *
 * @brief   send data.
 *
 * @param   buf - data buff
 *          len - data length
 *
 * @return  none
 */
void Transport_SendPacket(uint8_t *buf, uint32_t len)
{
	WCHNET_SocketSend(SocketId, buf, &len);
	printf("%d bytes uploaded!\r\n",len);
}

/*********************************************************************
 * @fn      MQTT_Connect
 *
 * @brief   Establish MQTT connection.
 *
 * @param   username - user name.
 *          password - password
 *
 * @return  none
 */
void MQTT_Connect(char *username, char *password)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    uint32_t len;
    uint8_t buf[200];

    data.clientID.cstring = "admin1";
    data.keepAliveInterval = 2000;
    data.cleansession = 1;
    data.username.cstring = username;
    data.password.cstring = password;

    len = MQTTSerialize_connect(buf,sizeof(buf),&data);
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      MQTT_Subscribe
 *
 * @brief   MQTT subscribes to a topic.
 *
 * @param   topic - Topic name to subscribe to
 *          req_qos - quality of service
 *
 * @return  none
 */
void MQTT_Subscribe( char *topic,int req_qos)
{
    MQTTString topicString = MQTTString_initializer;
    uint32_t len;
    uint32_t msgid = 1;
    uint8_t buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_subscribe(buf,sizeof(buf),0,msgid,1,&topicString,&req_qos);
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      MQTT_Unsubscribe
 *
 * @brief   MQTT unsubscribe from a topic.
 *
 * @param   topic - Topic name to unsubscribe to.
 *
 * @return  none
 */
void MQTT_Unsubscribe(char *topic)
{
    MQTTString topicString = MQTTString_initializer;
    uint32_t len;
    uint32_t msgid = 1;
    uint8_t buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_unsubscribe(buf,sizeof(buf),0,msgid,1,&topicString);
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      MQTT_Publish
 *
 * @brief   MQTT publishes a topic.
 *
 * @param   topic - Published topic name.
 *          qos - quality of service
 *          payload - data buff
 *
 * @return  none
 */
void MQTT_Publish(char *topic, int qos, char *payload)
{
    MQTTString topicString = MQTTString_initializer;
    uint32_t payloadlen;
    uint32_t len;
    uint8_t buf[512];

    topicString.cstring = topic;
    payloadlen = strlen(payload);
    len = MQTTSerialize_publish(buf,sizeof(buf),0,qos,0,packetid++,topicString,(unsigned char*)payload,payloadlen);
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      MQTT_Pingreq
 *
 * @brief   MQTT sends heartbeat packet
 *
 * @return  none
 */
void MQTT_Pingreq(void)
{
    uint32_t len;
    uint8_t buf[200];

    len = MQTTSerialize_pingreq(buf,sizeof(buf));
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      MQTT_Disconnect
 *
 * @brief   Disconnect the MQTT connection
 *
 * @return  none
 */
void MQTT_Disconnect(void)
{
    uint32_t len;
    uint8_t buf[50];
    len = MQTTSerialize_disconnect(buf,sizeof(buf));
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      msgDeal
 *
 * @brief   Dealing with subscription information.
 *
 * @param   msg - data buff
 *          len - data length
 *
 * @return  none
 */
void msgDeal(unsigned char *msg, int len)
{
    unsigned char *ptr = msg;
	uint8_t i;

    printf("payload len = %d\r\n",len);
    printf("payload: ");
    for( i = 0; i < len; i++)
    {
        printf("%c ",(uint16_t)*ptr);
        ptr++;
    }
    printf("\r\n");
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
    int qos, payloadlen;
    MQTTString topicName;
    unsigned short packetid;
    unsigned char retained, dup;
    unsigned char *payload;

    if(intstat & SINT_STAT_RECV)                                                //receive data
    {
        len = WCHNET_SocketRecvLen(socketid,NULL);
        WCHNET_SocketRecv(socketid,MyBuf,&len);
        switch(MyBuf[0]>>4)
        {
            case CONNACK:
                printf("CONNACK\r\n");
                con_flag = 1;
                MQTT_Subscribe(sub_topic, sub_qos);
                break;

            case PUBLISH:
                MQTTDeserialize_publish(&dup,&qos,&retained,&packetid,&topicName,
                                        &payload,&payloadlen,MyBuf,len);
                msgDeal(payload, payloadlen);
                break;

            case SUBACK:
                sub_flag = 1;
                printf("SUBACK\r\n");
                break;

            default:
                break;
        }
        memset(MyBuf, 0 ,sizeof(MyBuf));
    }
    if(intstat & SINT_STAT_CONNECT)                                             //connect successfully
    {
        WCHNET_ModifyRecvBuf(socketid, (uint32_t)SocketRecvBuf, RECE_BUF_LEN);
        MQTT_Connect(username, password);
        printf("TCP Connect Success\r\n");
    }
    if(intstat & SINT_STAT_DISCONNECT)                                          //disconnect
    {
        con_flag = 0;
        printf("TCP Disconnect\r\n");
    }
    if(intstat & SINT_STAT_TIM_OUT)                                             //timeout disconnect
    {
       con_flag = 0;
       printf("TCP Timeout\r\n");
       Transport_Open();
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
    uint8_t intstat, socketint;
    uint16_t i;

    intstat = WCHNET_GetGlobalInt();                                    //get global interrupt flag
    if(intstat & GINT_STAT_UNREACH)                                     //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if(intstat & GINT_STAT_IP_CONFLI)                                   //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if(intstat & GINT_STAT_PHY_CHANGE)                                  //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if(i&PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if(intstat & GINT_STAT_SOCKET)                                      //socket related interrupt
    {
        for(i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        {
            socketint = WCHNET_GetSocketInt(i);
            if(socketint) WCHNET_HandleSockInt(i, socketint);
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
    printf("MQTT Test\r\n");
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
    Transport_Open();                                            //open the TCP connection.

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
        if (publishValid == 1) {
            publishValid = 0;
            if(con_flag) MQTT_Publish(pub_topic, pub_qos, payload);
//            if(con_flag) MQTT_Pingreq();                                   //heartbeat packet
        }
    }
}
