/********************************** (C) COPYRIGHT ******************************
* File Name          : TCP_server.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/01
* Description        : CH57xNET库演示文件
*                      (1)、CH579 Examples by KEIL;
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序用于演示FTP 客户端
*******************************************************************************/



/******************************************************************************/
/* 头文件包含*/
#include <stdio.h>
#include <string.h>
#include "CH57x_common.h"
#include "core_cm0.h"
#include "CH57xNET.H"
#include "FTPINC.H"

#define KEEPLIVE_ENABLE                      1                                  /* 开启KEEPLIVE功能 */

/* 下面的缓冲区和全局变量必须要定义，库中调用 */
__align(16)UINT8    CH57xMACRxDesBuf[(RX_QUEUE_ENTRIES )*16];                   /* MAC接收描述符缓冲区，16字节对齐 */
__align(4) UINT8    CH57xMACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];                /* MAC接收缓冲区，4字节对齐 */
__align(4) SOCK_INF SocketInf[CH57xNET_MAX_SOCKET_NUM];                         /* Socket信息表，4字节对齐 */
const UINT16 MemNum[8] = {CH57xNET_NUM_IPRAW,
                         CH57xNET_NUM_UDP,
                         CH57xNET_NUM_TCP,
                         CH57xNET_NUM_TCP_LISTEN,
                         CH57xNET_NUM_TCP_SEG,
                         CH57xNET_NUM_IP_REASSDATA,
                         CH57xNET_NUM_PBUF,
                         CH57xNET_NUM_POOL_BUF
                         };
const UINT16 MemSize[8] = {CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_IPRAW_PCB),
                          CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_UDP_PCB),
                          CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_PCB),
                          CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_PCB_LISTEN),
                          CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_SEG),
                          CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_IP_REASSDATA),
                          CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_PBUF) + CH57xNET_MEM_ALIGN_SIZE(0),
                          CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_PBUF) + CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_POOL_BUF)
                         };
__align(4)UINT8 Memp_Memory[CH57xNET_MEMP_SIZE];
__align(4)UINT8 Mem_Heap_Memory[CH57xNET_RAM_HEAP_SIZE];
__align(4)UINT8 Mem_ArpTable[CH57xNET_RAM_ARP_TABLE_SIZE];

#define CH57xNET_TIMEPERIOD                   10                                /* 定时器定时周期，单位mS*/

/******************************************************************************/
/* 本演示程序的相关宏 */
#define RECE_BUF_LEN                          536                               /* 接收缓冲区的大小 */
#define READ_RECV_BUF_MODE                    0                                 /* socket接收缓冲区读取模式，1：复制，0：不复制 */

UINT8 SocketRecvBuf[CH57xNET_MAX_SOCKET_NUM][RECE_BUF_LEN];                     /* socket接收缓冲区 */
/* CH579相关定义 */                                                                   
UINT8 MACAddr[6]  = {0x84,0x21,0x04,0x05,0x06,0x07};                            /* CH579MAC地址 */ 
const UINT8 IPAddr[4]   = {192,168,1,200};                                       /* CH579IP地址 */ 
const UINT8 GWIPAddr[4] = {192,168,1,1};                                        /* CH579网关 */ 
const UINT8 IPMask[4]   = {255,255,255,0};                                      /* CH579子网掩码 */ 
const UINT8 DestIP[4]   = {192,168,1,100};                                      /* 目的IP */ 
const UINT8 *pUserName  = "qinheng";                                            /* 匿名登陆 */
const UINT8 *pPassword  = "123";                                                /* e-mail 名字作为登陆口令 */
UINT16 DESPORT = 3000;                                                            /* 目标端口号 */
char  ListName[24];                                                             /* 用于保存目录名 */
char  ListMkd[24];                                                              /* 用于保存新创建的目录名 */
char  FileName[24];                                                             /* 用于保存文件名 */
char  CharIP[17];                                                               /* 用于保存转换成字符的IP地址 */

UINT8 SocketId;                                                                 /* 保存socket索引，可以不用定义 */
UINT8 gPort;                                                                    /* 用于改变端口值 */
/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_IRQHandler( void )							/* 以太网中断 */
{
	CH57xNET_ETHIsr();								/* 以太网中断中断服务函数 */
}

void TMR0_IRQHandler( void ) 						/* 定时器中断 */
{
	CH57xNET_TimeIsr(CH57xNETTIMEPERIOD);           /* 定时器中断服务函数 */
	R8_TMR0_INT_FLAG |= 0xff;						/* 清除定时器中断标志 */
}

/*******************************************************************************
* Function Name  : SysTimeInit
* Description    : 系统定时器初始化，CH579@100MHZ TIME0 10ms，根据CH57xNET_TIMEPERIOD
*                ：来初始化定时器。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer0Init(UINT32 time)
{
	R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;		//清除所有计数值
	R8_TMR0_CTRL_MOD = 0;						//设置定时器模式
	R32_TMR0_CNT_END = FREQ_SYS/1000000*time;	//设置定时时间
	R8_TMR0_INT_FLAG = R8_TMR0_INT_FLAG;		//清除标志
	R8_TMR0_INTER_EN = RB_TMR_IE_CYC_END;		//定时中断
	R8_TMR0_CTRL_MOD |= RB_TMR_COUNT_EN;
	NVIC_EnableIRQ(TMR0_IRQn);	
}

/*******************************************************************************
* Function Name  : mStopIfError
* Description    : 调试使用，显示错误代码
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mStopIfError(UINT8 iError)
{
    if (iError == CH57xNET_ERR_SUCCESS) return;                                 /* 操作成功 */
    PRINT("mStopIfError: %02X\r\n", (UINT16)iError);                           /* 显示错误 */
}

/*******************************************************************************
* Function Name  : CH57xNET_LibInit
* Description    : 库初始化操作
* Input          : ip      ip地址指针
*                ：gwip    网关ip地址指针
*                : mask    掩码指针
*                : macaddr MAC地址F指针 
* Output         : None
* Return         : 执行状态
*******************************************************************************/
UINT8 CH57xNET_LibInit(const UINT8 *ip,const UINT8 *gwip,const UINT8 *mask,const UINT8 *macaddr)
{
    UINT8 i;
    struct _CH57x_CFG cfg;
    if(CH57xNET_GetVer() != CH57xNET_LIB_VER)return 0xfc;                       /* 获取库的版本号，检查是否和头文件一致 */
    CH57xNETConfig = LIB_CFG_VALUE;                                             /* 将配置信息传递给库的配置变量 */
    cfg.RxBufSize = RX_BUF_SIZE; 
    cfg.TCPMss   = CH57xNET_TCP_MSS;
    cfg.HeapSize = CH57x_MEM_HEAP_SIZE;
    cfg.ARPTableNum = CH57xNET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = CH57xNET_MISC_CONFIG0;
    CH57xNET_ConfigLIB(&cfg);
    i = CH57xNET_Init(ip,gwip,mask,macaddr);                                                         
    PRINT("CH57xNET_Config: %x\n",CH57xNETConfig);                                
    return (i);                                                                 /* 库初始化 */

}

/*******************************************************************************
* Function Name  : CH57xNET_HandleSokcetInt
* Description    : socket中断处理函数
* Input          : sockeid  socket索引
*                ：initstat 中断状态
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_HandleSokcetInt(UINT8 sockeid,UINT8 initstat)
{
    UINT32 len;

    PRINT("sockeid##= %02x\r\n",(UINT16)sockeid);
    if(initstat & SINT_STAT_RECV)                                               /* 接收中断 */
    {
        PRINT("ftp sockid:%d\r\n",ftp.SocketCtl);
        len = CH57xNET_SocketRecvLen(sockeid,NULL);                             /* 查询长度 */
        if(len){

        ftp.RecDatLen = len; 
        CH57xNET_SocketRecv(sockeid,SocketRecvBuf[sockeid],&len);               /* 将接收缓冲区的数据读到MyBuf中*/
        PRINT("check type= %02x\r\n",(UINT16)ftp.FileCmd);
        PRINT("len= %08d\n",ftp.RecDatLen);
        PRINT("MyBuffer=\r\n\n%s\r\n",(char *)SocketRecvBuf[sockeid]);
        CH57xNET_FTPProcessReceDat((char *)SocketRecvBuf[sockeid],ftp.FileCmd,sockeid);
        memset((void *)SocketRecvBuf[sockeid],'\0',sizeof(SocketRecvBuf[sockeid]));    
        
        }          
    }
    if(initstat & SINT_STAT_CONNECT)                                            /* TCP连接中断 */
    {                                                                /* 产生此中断表示TCP已经连接，可以进行收发数据 */
        PRINT("TCP Connect Success\r\n");                           
        //ftp.TcpStatus = CH57xNET_FTPGetSockeID( );
			  if( (sockeid != ftp.DatMonitor) && (sockeid!= ftp.SocketCtl))
					{
                ftp.DatTransfer = sockeid;
								PRINT("CH57xNET_FTPGetSockeID:%d!\r\n",ftp.DatTransfer); 
                CH57xNET_ModifyRecvBuf(sockeid,(UINT32)SocketRecvBuf[sockeid],RECE_BUF_LEN);  
					}
					
    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP断开中断 */
    {                                                               /* 产生此中断，CH57xNET_库内部会将此socket清除，置为关闭*/
        PRINT("TCP Disconnect\r\n");                                             /* 应用曾需可以重新创建连接 */
        if(sockeid == ftp.DatTransfer){
            ftp.TcpStatus = 0;
            ftp.CmdDataS = FTP_MACH_DATAOVER;
            if(ftp.DatMonitor != 255){
            CH57xNET_SocketClose( ftp.DatMonitor,TCP_CLOSE_NORMAL );
            }
        }
        
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP超时中断 */
    {                                                                       /* 产生此中断，CH57xNET_库内部会将此socket清除，置为关闭*/
        PRINT("TCP Timout\r\n");                                                 /* 应用曾需可以重新创建连接 */
    }
}

/*******************************************************************************
* Function Name  : CH57xNET_HandleGlobalInt
* Description    : 全局中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_HandleGlobalInt( void )
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    
    initstat = CH57xNET_GetGlobalInt();                                         /* 读全局中断状态并清除 */
    if(initstat & GINT_STAT_UNREACH)                                            /* 不可达中断 */
    {
        PRINT("UnreachCode ：%d\n",CH579Inf.UnreachCode);                      /* 查看不可达代码 */
        PRINT("UnreachProto ：%d\n",CH579Inf.UnreachProto);                    /* 查看不可达协议类型 */
        PRINT("UnreachPort ：%d\n",CH579Inf.UnreachPort);                      /* 查询不可达端口 */       
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP冲突中断 */
   {
       PRINT("IP interrupt\n");
   }
   if(initstat & GINT_STAT_PHY_CHANGE)                                          /* PHY改变中断 */
   {
       i = CH57xNET_GetPHYStatus();                                             /* 获取PHY状态 */
       PRINT("GINT_STAT_PHY_CHANGE %02x\n",i); 
   }
   if(initstat & GINT_STAT_SOCKET)                                              /* Socket中断 */
   {
       for(i = 0; i < CH57xNET_MAX_SOCKET_NUM; i ++)                     
       {
           socketinit = CH57xNET_GetSocketInt(i);                               /* 读socket中断并清零 */
           if(socketinit)CH57xNET_HandleSokcetInt(i,socketinit);                /* 如果有中断则清零 */
       }    
   }
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPCtlClient
* Description    : 创建TCP Client socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPCtlClient(void)
{
   UINT8 i;                                                             
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,DestIP,4);                                /* 设置目的IP地址 */
   TmpSocketInf.DesPort = DESPORT;                                              /* 设置目的端口 */
   TmpSocketInf.SourPort = 4000;                                                /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[0];                      /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = CH57xNET_SocketCreat( &SocketId,&TmpSocketInf );                         /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
   ftp.SocketCtl = SocketId;    
   PRINT("SocketId TCP FTP = %02x\n",(UINT16)ftp.SocketCtl);
   i = CH57xNET_SocketConnect(SocketId);                                        /* TCP连接 */
   mStopIfError(i);                                                             /* 检查错误 */
   memset((void *)SocketRecvBuf[SocketId],'\0',sizeof(SocketRecvBuf[SocketId]));              
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPDatClient
* Description    : 创建TCP Client socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPDatClient( UINT16 *port)
{
   UINT8 i;                                                             
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   if(gPort > 200) gPort = 0;
   gPort++;
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,DestIP,4);                                /* 设置目的IP地址 */
   TmpSocketInf.DesPort =  27;                                                  /* 设置目的端口 */
   TmpSocketInf.SourPort = 4001;                                                /* 设置源端口 */
//  TmpSocketInf.SourPort = 4000+gPort;                                         /* 设置源端口 */
   *port = TmpSocketInf.SourPort; 
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[1];                      /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = CH57xNET_SocketCreat( &SocketId,&TmpSocketInf );                         /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
   PRINT("SocketId FTP DATA = %02x\n",(UINT16)ftp.SocketCtl);   
   i = CH57xNET_SocketConnect(SocketId);                                        /* TCP连接 */
   mStopIfError(i);                                                             /* 检查错误 */
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPDatServer
* Description    : 创建TCP Server socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPDatServer( UINT16 *port )
{
   UINT8 i;
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */
 
   if(gPort > 200) gPort = 0;
   gPort++;
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   TmpSocketInf.SourPort = 4000 + gPort;                                        /* 设置源端口 */
   *port = TmpSocketInf.SourPort; 
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
   i = CH57xNET_SocketCreat(&SocketId,&TmpSocketInf);                           /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
   ftp.DatMonitor = SocketId; 
   i = CH57xNET_SocketListen(SocketId);                                         /* TCP监听 */
   PRINT("SocketId dat server:%d\n",(UINT16)SocketId);    
   mStopIfError(i);                                                             /* 检查错误 */
   memset((void *)SocketRecvBuf[SocketId],'\0',sizeof(SocketRecvBuf[SocketId]));              
}

/*******************************************************************************
* Function Name  : CH57xNET_ftp_SendData
* Description    : 发送数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPSendData( char *PSend, UINT32 Len,UINT8 index  )
{
    UINT32 length;
    UINT8 i,*p,count;

    p = (UINT8 *)PSend;
    length = Len;
    count = 0;
    while(1){
        Len = length;
        i = CH57xNET_SocketSend(index,p,&Len);                                  /* 将MyBuf中的数据发送 */
        mStopIfError(i);                                                        /* 检查错误 */
        if(Len == 0){
            count++;
            if(count>2){
    PRINT("Send Data  fail\n");   
                return;
            } 
        }
        length -= Len;                                                          /* 将总长度减去以及发送完毕的长度 */
        p += Len;                                                               /* 将缓冲区指针偏移*/
        if(length)continue;                                                     /* 如果数据未发送完毕，则继续发送*/
        break;                                                                  /* 发送完毕，退出 */
    }
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPGetSockeID
* Description    : 获取数据连接socketid
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH57xNET_FTPGetSockeID(  )
{
    UINT8 i;
    for(i=0;i<CH57xNET_MAX_SOCKET_NUM;i++){
        if(SocketInf[i].DesPort == DESPORT){
            if( (i != ftp.DatMonitor) && (i!= ftp.SocketCtl)){
                ftp.DatTransfer = i;
								PRINT("CH57xNET_FTPGetSockeID:i!\r\n",ftp.DatTransfer); 
                CH57xNET_ModifyRecvBuf(i,(UINT32)SocketRecvBuf[i],RECE_BUF_LEN);  
                return TRUE;
            }
        }
    }
    return FALSE; 
}

/*******************************************************************************
* Function Name  : CH57xNET__FTPInitVari
* Description    : 变量初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET__FTPInitVari( )
{
    UINT8 i,j;
    FTP   *pFTP;

    pFTP = &ftp;
    memset((void *)pFTP,0,sizeof(FTP));                   
    ftp.SocketCtl = 255; 
    ftp.DatMonitor = 255;
    ftp.DatTransfer = 255;
    ftp.FileCmd = FTP_CMD_LOGIN;                                                /* 执行登陆命令 */
    memset((void *)CharIP,'\0',sizeof(CharIP));                   
    j = 0;
    for(i=0;i<4;i++){                                                           /* 将十进制的IP地址转换所需要的字符格式 */ 
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

/*******************************************************************************
* Function Name  : CH57xNET_FTPInit
* Description    : ch579初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPInit( void ) 
{

    UINT8 i = 0;
    CH57xNET__FTPInitVari( );                                                   /* 初始化变量 */
    i = CH57xNET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* 库初始化 */
    mStopIfError(i);                                                            /* 检查错误 */
    PRINT("CH57xNET_LibInit Success\n");   
    DelsyMs(50);
    Timer0Init( 10000 );		// 初始化定时器:10ms
	NVIC_EnableIRQ(ETH_IRQn);
}

/*******************************************************************************
* Function Name  : SystemClock_UART1_init
* Description    : 系统时钟和串口1初始化，串口1用于打印输出
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SystemClock_UART1_init(void)
{
    PWR_UnitModCfg(ENABLE, UNIT_SYS_PLL);                                      /* PLL上电 */
    DelsyMs(3); 
    SetSysClock(CLK_SOURCE_HSE_32MHz);                                          /* 外部晶振 PLL 输出32MHz */
    GPIOA_SetBits( GPIO_Pin_9 );
    GPIOA_ModeCfg( GPIO_Pin_9, GPIO_ModeOut_PP_5mA );                           /* 串口1的IO口设置 */
	UART1_DefInit( );                                                            /* 串口1初始化 */
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void )
{
	SystemClock_UART1_init();
    CH57xNET_FTPInit( );                                                        /* 初始化 */
    while(1)
    {
        CH57xNET_MainTask( );                                                   /* CH579IP库主任务函数，需要在主循环中不断调用 */
        if(CH57xNET_QueryGlobalInt( ))CH57xNET_HandleGlobalInt();               /* 查询中断，如果有中断，则调用全局中断处理函数 */
        CH57xNET_FTPClientCmd( );                                               /* 查询状态，执行子命令 */
    }
}

/*********************************** endfile **********************************/
