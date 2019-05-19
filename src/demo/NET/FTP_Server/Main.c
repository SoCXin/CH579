/********************************** (C) COPYRIGHT ******************************
* File Name          : Main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/01
* Description        : CH57xNET库演示文件
*                      (1)、CH579 Examples by KEIL;
*                      (2)、串口1输出监控信息,115200bps,打开或者关闭调试信息输出在IDE宏定义中设置;
*                      (3)、本程序用于演示FTP Server通讯.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含*/
#include <stdio.h>
#include <string.h>
#include "CH57x_common.h"
#include "core_cm0.h"
#include "CH57xNET.h"
#include "FTPINC.h"

#define KEEPLIVE_ENABLE                      1                                  /* 开启KEEPLIVE功能 */

/* 下面的缓冲区和全局变量必须要定义，库中调用 */
__align(16)UINT8    CH57xMACRxDesBuf[(RX_QUEUE_ENTRIES )*16];                   /* MAC接收描述符缓冲区，16字节对齐 */
__align(4) UINT8    CH57xMACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];                /* MAC接收缓冲区，4字节对齐 */
__align(4) SOCK_INF SocketInf[CH57xNET_MAX_SOCKET_NUM];                         /* Socket信息表，4字节对齐 */

/* const*/ UINT16 MemNum[8] = {CH57xNET_NUM_IPRAW,
                         CH57xNET_NUM_UDP,
                         CH57xNET_NUM_TCP,
                         CH57xNET_NUM_TCP_LISTEN,
                         CH57xNET_NUM_TCP_SEG,
                         CH57xNET_NUM_IP_REASSDATA,
                         CH57xNET_NUM_PBUF,
                         CH57xNET_NUM_POOL_BUF
                         };
/*const*/ UINT16 MemSize[8] = {CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_IPRAW_PCB),
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
/******************************************************************************/
/* 本演示程序的相关宏 */
#define RECE_BUF_LEN                          536                               /* 接收缓冲区的大小 */
/* CH57xNET库TCP的MSS长度为536字节，即一个TCP包里的数据部分最长为536字节 */
/* TCP协议栈采用滑动窗口进行流控，窗口最大值为socket的接收缓冲区长度。在设定 */
/* RX_QUEUE_ENTRIES时要考虑MSS和窗口之间的关系，例如窗口值为4*MSS，则远端一次会发送 */
/* 4个TCP包，如果RX_QUEUE_ENTRIES小于4，则必然会导致数据包丢失，从而导致通讯效率降低 */
/* 建议RX_QUEUE_ENTRIES要大于( 窗口/MSS ),如果多个socket同时进行大批量发送数据，则 */ 
/* 建议RX_QUEUE_ENTRIES要大于(( 窗口/MSS )*socket个数) 在多个socket同时进行大批数据收发时 */
/* 为了节约RAM，请将接收缓冲区的长度设置为MSS */

/* CH579相关定义 */
UINT8 MACAddr[6] = {0x84,0xc2,0xe4,0x02,0x03,0x04};                             /* CH579MAC地址 */
const UINT8 IPAddr[4] = {192,168,111,200};                                      /* CH579IP地址 */
const UINT8 GWIPAddr[4] = {192,168,111,191};                                    /* CH579网关 */
const UINT8 IPMask[4] = {255,255,255,0};                                        /* CH579子网掩码 */
//UINT8 DESIP[4] = {192,168,111,100};                                           /* 目的IP地址 */

/* 变量相关定义 */
extern FTP ftp;        
char  UserName[16];                                                             /* 用于保存用户名 */
char  SourIP[17];                                                               /* 用于保存转换成字符的IP地址 */
#if Access_Authflag
char  *pUserName = "anonymous";                                                 /* 可以访问服务的用户名（其他用户名无法访问服务器） */
#endif
char  *pPassWord = "IEUser@";                                                   /* 有权限的密码（其他密码只支持读） */
UINT8 SocketId;    
UINT8 SerPort;                                                                  /* 保存socket索引，可以不用定义 */

UINT8 SocketRecvBuf[RECE_BUF_LEN];                                              /* socket接收缓冲区 */
UINT8 MyBuf[RECE_BUF_LEN];                                                      /* 定义一个临时缓冲区 */


extern const UINT16 *memp_num;

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_IRQHandler( void )							                           /* 以太网中断 */
{
	CH57xNET_ETHIsr();							                               /* 以太网中断中断服务函数 */
}

void TMR0_IRQHandler( void ) 						                           /* 定时器中断 */
{
	CH57xNET_TimeIsr(CH57xNETTIMEPERIOD);                                      /* 定时器中断服务函数 */
	R8_TMR0_INT_FLAG |= 0xff;						                           /* 清除定时器中断标志 */
}



/*******************************************************************************
* Function Name  : mStopIfError
* Description    : 调试使用，显示错误代码
* Input          : iError 错误代码
* Output         : None
* Return         : None
*******************************************************************************/
void mStopIfError(UINT8 iError)
{
    if (iError == CH57xNET_ERR_SUCCESS) return;                                 /* 操作成功 */
    PRINT("mStopIfError: %02X\r\n", (UINT16)iError);                            /* 显示错误 */    
}


/*******************************************************************************
* Function Name  : net_initkeeplive
* Description    : keeplive初始化
* Input          : None      
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef  KEEPLIVE_ENABLE
void net_initkeeplive(void)
{
    struct _KEEP_CFG  klcfg;

    klcfg.KLIdle = 20000;                                                       /* 空闲 */
    klcfg.KLIntvl = 10000;                                                      /* 间隔 */
    klcfg.KLCount = 5;                                                          /* 次数 */
    CH57xNET_ConfigKeepLive(&klcfg);
}
#endif

/*******************************************************************************
* Function Name  : CH57xNET_LibInit
* Description    : 库初始化操作
* Input          : ip      ip地址指针
*                ：gwip    网关ip地址指针
*                : mask    掩码指针
*                : macaddr MAC地址指针 
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
#ifdef  KEEPLIVE_ENABLE
    net_initkeeplive( );
#endif
    return (i);                      /* 库初始化 */
}

/*******************************************************************************
* Function Name  : CH57xNET_HandleSockInt
* Description    : Socket中断处理函数
* Input          : sockeid  socket索引
*                ：initstat 中断状态
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_HandleSockInt(UINT8 sockeid,UINT8 initstat)
{
    UINT32 len;
    UINT32 totallen;
    UINT8 *p = MyBuf;

    if(initstat & SINT_STAT_RECV)                                               /* 接收中断 */
    {
        len = CH57xNET_SocketRecvLen(sockeid,NULL);                             /* 查询长度 */
		PRINT("Receive Len = %d\r\n",len);                           
        totallen = len;
        CH57xNET_SocketRecv(sockeid,MyBuf,&len);                                /* 将接收缓冲区的数据读到MyBuf中*/
        while(1)
        {
           len = totallen;
           CH57xNET_SocketSend(sockeid,p,&len);                                 /* 将MyBuf中的数据发送 */
           totallen -= len;                                                     /* 将总长度减去以及发送完毕的长度 */
           p += len;                                                            /* 将缓冲区指针偏移*/
           if(totallen)continue;                                                /* 如果数据未发送完毕，则继续发送*/
           break;                                                               /* 发送完毕，退出 */
        }
    }
    if(initstat & SINT_STAT_CONNECT)                                            /* TCP连接中断 */
    {                                                                           /* 产生此中断表示TCP已经连接，可以进行收发数据 */
        PRINT("TCP Connect Success\n");                           
    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP断开中断 */
    {                                                                           /* 产生此中断，CH579库内部会将此socket清除，置为关闭*/
        PRINT("TCP Disconnect\n");                                              /* 应用曾需可以重新创建连接 */
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP超时中断 */
    {                                                                           /* 产生此中断，CH579库内部会将此socket清除，置为关闭*/
        PRINT("TCP Timout\n");                                                  /* 应用曾需可以重新创建连接 */
    }
}


/*******************************************************************************
* Function Name  : CH57xNET_HandleGloableInt
* Description    : 全局中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_HandleGlobalInt(void)
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    initstat = CH57xNET_GetGlobalInt();                                         /* 读全局中断状态并清除 */
    if(initstat & GINT_STAT_UNREACH)                                            /* 不可达中断 */
    {
        PRINT("UnreachCode ：%d\n",CH57xInf.UnreachCode);                       /* 查看不可达代码 */
        PRINT("UnreachProto ：%d\n",CH57xInf.UnreachProto);                     /* 查看不可达协议类型 */
        PRINT("UnreachPort ：%d\n",CH57xInf.UnreachPort);                       /* 查询不可达端口 */      
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP冲突中断 */
   {
   
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
           if(socketinit)CH57xNET_HandleSockInt(i,socketinit);                  /* 如果有中断则清零 */
       }    
   }
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPCtlServer
* Description    : 创建TCP Server socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPCtlServer(void)
{
   UINT8 i;                                                             
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   TmpSocketInf.SourPort = 21;                                                  /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
   i = CH57xNET_SocketCreat(&SocketId,&TmpSocketInf);                           /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
   ftp.SocketCtlMonitor = SocketId; 
   i = CH57xNET_SocketListen(SocketId);                                         /* TCP监听 */
   PRINT("SocketId ctl:%d\n",(UINT16)SocketId);
   mStopIfError(i);                                                             /* 检查错误 */
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPServerDat
* Description    : 创建TCP Server socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPServerDat( UINT16 *port )
{
   UINT8 i;
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */
 
   if(SerPort < 100||SerPort > 200) SerPort = 100;
   SerPort++;
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   TmpSocketInf.SourPort = 256*5 + SerPort;                                     /* 设置源端口 */
   *port = TmpSocketInf.SourPort; 
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
   i = CH57xNET_SocketCreat(&SocketId,&TmpSocketInf);                           /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
   ftp.SocketDatMonitor = SocketId; 
   i = CH57xNET_SocketListen(SocketId);                                         /* TCP监听 */
   PRINT("SocketId dat server:%d\n",(UINT16)SocketId);
   mStopIfError(i);                                                             /* 检查错误 */
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPClientDat
* Description    : 创建TCP Server socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPClientDat( UINT16 port,UINT8 *pAddr )
{
   UINT8 i;                                                             
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,pAddr,4);
   TmpSocketInf.SourPort = 20;                                                  /* 设置源端口 */
   TmpSocketInf.DesPort = port;
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
   i = CH57xNET_SocketCreat(&SocketId,&TmpSocketInf);                           /* 创建socket，将返回的socket索引保存在SocketId中 */
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[SocketId];               /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   ftp.SocketDatConnect = SocketId;
   PRINT("SocketId dat client:%d\n",(UINT16)SocketId);
   mStopIfError(i);                                                             /* 检查错误 */
   i = CH57xNET_SocketConnect(SocketId);                                        /* TCP创建连接 */
   mStopIfError(i);                                                             /* 检查错误 */
   memset((void *)SocketRecvBuf[SocketId],'\0',sizeof(SocketRecvBuf[SocketId]));              
}

/*******************************************************************************
* Function Name  : SendData
* Description    : 发送数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPSendData( char *PSend, UINT32 Len,UINT8 index )
{
    UINT32 length;
    UINT8 i,*p,count;

    p = (UINT8 *)PSend;
    PRINT("send buf:\n%s\n",PSend);
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
        p += Len;                                                               /* 将缓冲区指针偏移 */
        if(length)continue;                                                     /* 如果数据未发送完毕，则继续发送 */
        break;                                                                  /* 发送完毕，退出 */
    }
    PRINT("Send Data  Success\n");
    ftp.TimeCount = 0;
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPGetSockeID
* Description    : 获取数据连接socketid
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPGetSockeID( UINT8 socketid )
{
    UINT8 i;

    if(SocketInf[socketid].SourPort == 21){
        if(ftp.TcpCtlSta <= FTP_MACH_DISCONT){
            ftp.SocketCtlConnect = socketid;
            ftp.TcpCtlSta = FTP_MACH_CONNECT;
        }
        else{
            CH57xNET_SocketClose( socketid,TCP_CLOSE_NORMAL );
        }
    }
    if(SocketInf[socketid].SourPort == 20||SocketInf[socketid].SourPort == (256*5 + SerPort)){
        if(ftp.TcpDatSta <= FTP_MACH_DISCONT){
            ftp.TcpDatSta = FTP_MACH_CONNECT;
            ftp.SocketDatConnect = socketid;
        }
        else{
            i = CH57xNET_SocketClose( socketid,TCP_CLOSE_NORMAL );              /* 只接受一个客户端连接进行数据收发 */
            mStopIfError( i ); 
            PRINT("ERROR: only support a socket connected\n");
        }
    }
}
/*******************************************************************************
* Function Name  : CH57xNET_FTPInitVari
* Description    : 变量初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPInitVari( )
{
    FTP   *pFTP;
    UINT8 i,j;

    pFTP = &ftp;
    memset((void *)pFTP,'\0',sizeof(ftp));
    ftp.SocketDatMonitor = 255;
    ftp.SocketDatConnect = 255;
    ftp.SocketCtlMonitor = 255;
    ftp.SocketCtlConnect = 255; 
    memset((void *)SourIP,'\0',sizeof(SourIP));                   
    j = 0;
    for(i=0;i<4;i++){                                                           /* 将十进制的IP地址转换所需要的字符格式 */
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
         SourIP[j++] = '.';
    }
    mInitFtpList( );
}

/*******************************************************************************
* Function Name  :  Timer0Init
* Description    : 定时器1初始化
* Input          : time 定时时间
* Output         : None
* Return         : None
*******************************************************************************/
void Timer0Init(UINT32 time)
{
	R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;		                               /* 清除所有计数值 */
	R8_TMR0_CTRL_MOD = 0;						                               /* 设置定时器模式 */
	R32_TMR0_CNT_END = FREQ_SYS/1000000*time;	                               /* 设置定时时间 */
	R8_TMR0_INT_FLAG = R8_TMR0_INT_FLAG;		                               /* 清除标志 */
	R8_TMR0_INTER_EN = RB_TMR_IE_CYC_END;	                                   /* 定时中断 */
	R8_TMR0_CTRL_MOD |= RB_TMR_COUNT_EN;
	NVIC_EnableIRQ(TMR0_IRQn);	
}

/*******************************************************************************
* Function Name  : CH57xNET_FTPConnect
* Description    : ch579初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH57xNET_FTPConnect( void ) 
{
    UINT8 i = 0;
    CH57xNET_FTPInitVari( );
    i = CH57xNET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* 库初始化 */
    mStopIfError(i);                                                            /* 检查错误 */
    PRINT("CH57xNET_LibInit Success\n");
	Timer0Init( 10000 );	                                                 	/* 初始化定时器:10ms */
	NVIC_EnableIRQ(ETH_IRQn);
	                                                                            /* 初始化中断 */
	while ( CH57xInf.PHYStat < 2 ) 
		DelsyMs(50);
    CH57xNET_FTPCtlServer( );
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
int main(void) 
{
    SystemClock_UART1_init();
	CH57xNET_FTPConnect( );                                                     /* 进行TCP FTP控制连接 */	
    while(1)
    {
        CH57xNET_MainTask();                                                    /* CH57xNET库主任务函数，需要在主循环中不断调用 */
        if(CH57xNET_QueryGlobalInt())CH57xNET_HandleGlobalInt();                /* 查询中断，如果有中断，则调用全局中断处理函数 */
        CH57xNET_FTPServerCmd( );             
	}
}
/*********************************** endfile **********************************/
