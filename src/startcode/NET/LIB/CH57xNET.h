/********************************** (C) COPYRIGHT *********************************
* File Name          : CH57xNET.H
* Author             : tech9
* Version            : V1.0
* Date               : 2015/01/15
* Description        : C Define for CH57xNET.LIB
*                      Website: http://wch.cn
*                      Email:   tech@wch.cn         
**********************************************************************************/
/* 更新日志 */



#ifndef __CH57xNET_H__
#define __CH57xNET_H__

#define CH57xNET_ETHDBG 0

/* 库版本信息 */
#define CH57xNET_LIB_VER                      0X01                   /* 库版本信息 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef     TRUE
#define     TRUE    1
#define     FALSE   0
#endif
#ifndef     NULL
#define     NULL    0
#endif


	
/* Socket的个数 */
#ifndef CH57xNET_MAX_SOCKET_NUM
#define CH57xNET_MAX_SOCKET_NUM               4                                 /* Socket的个数，用户可以配置，默认为4个Socket,最大为32 */
#endif

#ifndef RX_QUEUE_ENTRIES
#define RX_QUEUE_ENTRIES                      4                                 /* CH57x MAC接收描述符队列，此值不得小于2,可配置默认为7，最大为32 */
#endif                                                                          /* #ifndef RX_QUEUE_ENTRIES */
                                                                                
#ifndef RX_BUF_SIZE
#define RX_BUF_SIZE                           600                             /* CH57x MAC接收每个缓冲区长度，为4的整数倍 */
#endif

#ifndef CH57xNET_PING_ENABLE                                                    
#define CH57xNET_PING_ENABLE                  TRUE                              /* 默认PING开启 */
#endif                                                                          /* PING使能 */   
                                                                                
#ifndef TCP_RETRY_COUNT                                                         
#define TCP_RETRY_COUNT                       20                                /* TCP重试次数，位宽为5位*/
#endif                                                                          
                                                                                
#ifndef TCP_RETRY_PERIOD                                                        
#define TCP_RETRY_PERIOD                      10                                /* TCP重试周期，单位为50MS，位宽为5位 */
#endif                                                                          
                                                                                
#ifndef CH57xNETTIMEPERIOD                                                      
#define CH57xNETTIMEPERIOD                    10                                /* 定时器周期，单位Ms,不得大于500 */
#endif                                                                          
                                                                                
#define MAC_INT_TYPE_IRQ                      0                                 /* 以太网使用irq中断 */
#define MAC_INT_TYPE_FIQ                      1                                 /* 以太网使用frq中断 */
                                                                                
#ifndef MAC_INT_TYPE                                                            
#define MAC_INT_TYPE                          MAC_INT_TYPE_IRQ                  /* 默认以太网使用IRQ中断 */
#endif                                                                          
                                                                                
#ifndef SOCKET_SEND_RETRY                                                       
#define SOCKET_SEND_RETRY                     1                                 /* 默认发送重试 */
#endif                    

#define TX_QUEUE_ENTRIES                      2                                 /* CH57x MAC发送描述符队列为2 */
#define LIB_CFG_VALUE                         ((SOCKET_SEND_RETRY << 25) |\
                                              (MAC_INT_TYPE << 24) |\
                                              (TCP_RETRY_PERIOD << 19) |\
                                              (TCP_RETRY_COUNT << 14) |\
                                              (CH57xNET_PING_ENABLE << 13) |\
                                              (TX_QUEUE_ENTRIES << 9) |\
                                              (RX_QUEUE_ENTRIES << 5) |\
                                              (CH57xNET_MAX_SOCKET_NUM))
#ifndef MISCE_CFG0_TCP_SEND_COPY 
#define MISCE_CFG0_TCP_SEND_COPY              1                                 /* TCP发送缓冲区复制 */
#endif                                                                          

#ifndef MISCE_CFG0_TCP_RECV_COPY                                                
#define MISCE_CFG0_TCP_RECV_COPY              1                                 /* TCP接收复制优化，内部调试使用 */
#endif                                                                          

#ifndef MISCE_CFG0_TCP_OLD_DELETE                                               
#define MISCE_CFG0_TCP_OLD_DELETE             0                                 /* 删除最早的TCP连接 */
#endif

/*关于内存分配 */
#ifndef CH57xNET_NUM_IPRAW
#define CH57xNET_NUM_IPRAW                    4                                 /* IPRAW连接的个数 */ 
#endif

#ifndef CH57xNET_NUM_UDP
#define CH57xNET_NUM_UDP                      4                                 /* UDP连接的个数 */                                   
#endif

#ifndef CH57xNET_NUM_TCP
#define CH57xNET_NUM_TCP                      4                                 /* TCP连接的个数 */ 
#endif

#ifndef CH57xNET_NUM_TCP_LISTEN
#define CH57xNET_NUM_TCP_LISTEN               4                                 /* TCP监听的个数 */
#endif

#ifndef CH57xNET_NUM_PBUF
#define CH57xNET_NUM_PBUF                     5                                 /* PBUF结构的个数 */
#endif

#ifndef CH57xNET_NUM_POOL_BUF
#define CH57xNET_NUM_POOL_BUF                 6                                /* POOL BUF的个数 */
#endif

#ifndef CH57xNET_NUM_TCP_SEG
#define CH57xNET_NUM_TCP_SEG                  10                                /* tcp段的个数*/
#endif

#ifndef CH57xNET_NUM_IP_REASSDATA
#define CH57xNET_NUM_IP_REASSDATA             5                                 /* IP分段的长度 */
#endif

#ifndef CH57xNET_TCP_MSS
#define CH57xNET_TCP_MSS                      536                               /* tcp MSS的大小*/
#endif

#ifndef CH57x_MEM_HEAP_SIZE
#define CH57x_MEM_HEAP_SIZE                   4600                              /* 内存堆大小 */
#endif

#ifndef CH57xNET_NUM_ARP_TABLE
#define CH57xNET_NUM_ARP_TABLE                10                                /* ARP列表个数 */
#endif

#ifndef CH57xNET_MEM_ALIGNMENT
#define CH57xNET_MEM_ALIGNMENT                4                                 /* 4字节对齐 */
#endif

#ifndef CH57xNET_IP_REASS_PBUFS             
#if (CH57xNET_NUM_POOL_BUF < 32)
#define CH57xNET_IP_REASS_PBUFS               (CH57xNET_NUM_POOL_BUF - 1)       /* IP分片的PBUF个数，最大为31 */
#else
#define CH57xNET_IP_REASS_PBUFS               31       
#endif
#endif

#define CH57xNET_MISC_CONFIG0                 ((MISCE_CFG0_TCP_SEND_COPY << 0) |\
                                               (MISCE_CFG0_TCP_RECV_COPY << 1) |\
                                               (MISCE_CFG0_TCP_OLD_DELETE << 2)|\
                                               (CH57xNET_IP_REASS_PBUFS)<<3)
/* PHY 状态 */
#define PHY_DISCONN                           (1<<0)                            /* PHY断开 */
#define PHY_10M_FLL                           (1<<1)                            /* 10M全双工 */
#define PHY_10M_HALF                          (1<<2)                            /* 10M半双工 */
#define PHY_100M_FLL                          (1<<3)                            /* 100M全双工 */
#define PHY_100M_HALF                         (1<<4)                            /* 100M半双工 */
#define PHY_AUTO                              (1<<5)                            /* PHY自动模式，CMD10H_SET_PHY */
                                                                                
/* Socket 工作模式定义,协议类型 */                                                       
#define PROTO_TYPE_IP_RAW                     0                                 /* IP层原始数据 */
#define PROTO_TYPE_UDP                        2                                 /* UDP协议类型 */
#define PROTO_TYPE_TCP                        3                                 /* TCP协议类型 */
                                                                                
/* 中断状态 */                                                                  
/* 以下为GLOB_INT会产生的状态 */                                                
#define GINT_STAT_UNREACH                     (1<<0)                            /* 不可达中断*/
#define GINT_STAT_IP_CONFLI                   (1<<1)                            /* IP冲突*/
#define GINT_STAT_PHY_CHANGE                  (1<<2)                            /* PHY状态改变 */
#define GINT_STAT_SOCKET                      (1<<4)                            /* scoket 产生中断 */
                                                                                
/*以下为Sn_INT会产生的状态*/                                                    
#define SINT_STAT_RECV                        (1<<2)                            /* socket端口接收到数据或者接收缓冲区不为空 */
#define SINT_STAT_CONNECT                     (1<<3)                            /* 连接成功,TCP模式下产生此中断 */
#define SINT_STAT_DISCONNECT                  (1<<4)                            /* 连接断开,TCP模式下产生此中断 */
#define SINT_STAT_TIM_OUT                     (1<<6)                            /* ARP和TCP模式下会发生此中断 */

/* 错误码 */
#define CH57xNET_ERR_SUCCESS                  0x00                              /* 命令操作成功 */
#define CH57xNET_RET_ABORT                    0x5F                              /* 命令操作失败 */
#define CH57xNET_ERR_BUSY                     0x10                              /* 忙状态，表示当前正在执行命令 */
#define CH57xNET_ERR_MEM                      0x11                              /* 内存错误 */
#define CH57xNET_ERR_BUF                      0x12                              /* 缓冲区错误 */
#define CH57xNET_ERR_TIMEOUT                  0x13                              /* 超时 */
#define CH57xNET_ERR_RTE                      0x14                              /* 路由错误*/
#define CH57xNET_ERR_ABRT                     0x15                              /* 连接停止*/
#define CH57xNET_ERR_RST                      0x16                              /* 连接复位 */
#define CH57xNET_ERR_CLSD                     0x17                              /* 连接关闭/socket 在关闭状态*/
#define CH57xNET_ERR_CONN                     0x18                              /* 无连接 */
#define CH57xNET_ERR_VAL                      0x19                              /* 错误的值 */
#define CH57xNET_ERR_ARG                      0x1a                              /* 错误的参数 */
#define CH57xNET_ERR_USE                      0x1b                              /* 已经被使用 */
#define CH57xNET_ERR_IF                       0x1c                              /* MAC错误  */
#define CH57xNET_ERR_ISCONN                   0x1d                              /* 已连接 */
#define CH57xNET_ERR_SOCKET_MEM               0X20                              /* Socket信息列表已满或者错误 */
#define CH57xNET_ERR_UNSUPPORT_PROTO          0X21                              /* 不支持的协议类型 */
#define CH57xNET_ERR_UNKNOW                   0xFA                              /* 未知错误 */

/* 不可达代码 */
#define UNREACH_CODE_HOST                     0                                 /* 主机不可达 */
#define UNREACH_CODE_NET                      1                                 /* 网络不可达 */
#define UNREACH_CODE_PROTOCOL                 2                                 /* 协议不可达 */
#define UNREACH_CODE_PROT                     3                                 /* 端口不可达 */
/*其他值请参考RFC792文档*/                                                      

/* TCP关闭参数 */
#define TCP_CLOSE_NORMAL                      0                                 /* 正常关闭，进行4此握手 */
#define TCP_CLOSE_RST                         1                                 /* 复位连接，并关闭  */
#define TCP_CLOSE_ABANDON                     2                                 /* CH57xNET内部丢弃连接，不会发送任何终止报文 */

/* socket状态 */
#define  SOCK_STAT_CLOSED                     0X00                              /* socket关闭 */
#define  SOCK_STAT_OPEN                       0X05                              /* socket打开 */

/* TCP状态 */
#define TCP_CLOSED                            0                                 /* TCP连接 */
#define TCP_LISTEN                            1                                 /* TCP关闭 */
#define TCP_SYN_SENT                          2                                 /* SYN发送，连接请求 */
#define TCP_SYN_RCVD                          3                                 /* SYN接收，接收到连接请求 */
#define TCP_ESTABLISHED                       4                                 /* TCP连接建立 */
#define TCP_FIN_WAIT_1                        5                                 /* WAIT_1状态 */
#define TCP_FIN_WAIT_2                        6                                 /* WAIT_2状态 */
#define TCP_CLOSE_WAIT                        7                                 /* 等待关闭 */
#define TCP_CLOSING                           8                                 /* 正在关闭 */
#define TCP_LAST_ACK                          9                                 /* LAST_ACK*/
#define TCP_TIME_WAIT                         10                                /* 2MSL等待 */

/* sokcet信息表 */
#ifndef ST_SCOK_INF
#define ST_SCOK_INF
typedef struct _SCOK_INF
{
    UINT32 IntStatus;                                                           /* 中断状态 */
    UINT32 SockIndex;                                                           /* Socket索引值 */
    UINT32 RecvStartPoint;                                                      /* 接收缓冲区的开始指针 */
    UINT32 RecvBufLen;                                                          /* 接收缓冲区长度 */
    UINT32 RecvCurPoint;                                                        /* 接收缓冲区的当前指针 */
    UINT32 RecvReadPoint;                                                       /* 接收缓冲区的读指针 */
    UINT32 RecvRemLen;                                                          /* 接收缓冲区的剩余长度 */
    UINT32 ProtoType;                                                           /* 协议类型 */
    UINT32 ScokStatus;                                                          /* 低字节Socket状态，次低字节为TCP状态，仅TCP模式下有意义 */
    UINT32 DesPort;                                                             /* 目的端口 */
    UINT32 SourPort;                                                            /* 源端口在IPRAW模式下为协议类型 */
    UINT8  IPAddr[4];                                                           /* Socket目标IP地址 32bit*/
    void *Resv1;                                                                /* 保留，内部使用，用于保存各个PCB */
    void *Resv2;                                                                /* 保留，内部使用，TCP Server使用 */
 // void (*RecvCallBack)(struct _SCOK_INF *socinf,UINT32 ipaddr,UINT16 port,UINT8 *buf,UINT32 len); /* 接收回调函数*/
    void (*AppCallBack)(struct _SCOK_INF *,UINT32 ,UINT16 ,UINT8 *,UINT32 ); /* 接收回调函数*/

}SOCK_INF;
#endif


/* CH57x全局信息 */
#ifndef ST_CH57x_SYS
#define ST_CH57x_SYS
struct _CH57x_SYS
{
    UINT8  IPAddr[4];                                                           /* CH57xIP地址 32bit */
    UINT8  GWIPAddr[4];                                                         /* CH57x网关地址 32bit */
    UINT8  MASKAddr[4];                                                         /* CH57x子网掩码 32bit */
    UINT8  MacAddr[8];                                                          /* CH57xMAC地址 48bit */
    UINT8  UnreachIPAddr[4];                                                    /* 不可到达IP */
    UINT32 RetranCount;                                                         /* 重试次数 默认为10次 */
    UINT32 RetranPeriod;                                                        /* 重试周期,单位MS,默认200MS */
    UINT32 PHYStat;                                                             /* CH57xPHY状态码 8bit */
    UINT32 CH57xStat;                                                           /* CH57x的状态 ，包含是否打开等 */ 
    UINT32 MackFilt;                                                            /* CH57x MAC过滤，默认为接收广播，接收本机MAC 8bit */
    UINT32 GlobIntStatus;                                                       /* 全局中断 */
    UINT32 UnreachCode;                                                         /* 不可达 */
    UINT32 UnreachProto;                                                        /* 不可达协议 */
    UINT32 UnreachPort;                                                         /* 不可到达端口 */
    UINT32 SendFlag;
};
#endif

/* 内存以及杂项配置 */
#ifndef ST_CH57x_CFG
#define ST_CH57x_CFG
struct _CH57x_CFG
{
    UINT32 RxBufSize;                                                            /* MAC接收缓冲区大小 */
    UINT32 TCPMss;                                                               /* TCP MSS大小 */
    UINT32 HeapSize;                                                             /* 堆分配内存大小 */
    UINT32 ARPTableNum;                                                          /* ARP列表个数 */
    UINT32 MiscConfig0;                                                          /* 其他杂项配置 */
};
#endif

/* KEEP LIVE配置结构体 */
#ifndef ST_KEEP_CFG
#define ST_KEEP_CFG
struct _KEEP_CFG
{
   UINT32 KLIdle;                                                               /* KEEPLIVE空闲时间 */
   UINT32 KLIntvl;                                                              /* KEEPLIVE周期 */
   UINT32 KLCount;                                                              /* KEEPLIVE次数 */
};
#endif

/* 库内部变量声明 */
extern UINT8  CH57xIPIntStatus;                                                 /* 中断状态 */
extern struct _CH57x_SYS   CH57xInf;                                            /* 系统信息列表 */
extern UINT32 CH57xNETConfig;                                                   /* 库配置，下行说明 */
/* 位0-4 Socket的个数,最大值为31 */
/* 位5-8 MAC 接收描述符的个数，最大值为15 */
/* 位13 PING使能，1为开启PING，0为关闭PING，默认为开启 */
/* 位14-18 TCP重试次数*/
/* 位19-23 TCP重试周期，单位为50毫秒*/
/* 位24 以太网中断类型 */
/* 位25 发送重试配置 */

/* 以下值为固定值不可以更改 */
#define CH57xNET_MEM_ALIGN_SIZE(size)         (((size) + CH57xNET_MEM_ALIGNMENT - 1) & ~(CH57xNET_MEM_ALIGNMENT-1))
#define CH57xNET_SIZE_IPRAW_PCB               0x1C                              /* IPRAW PCB大小 */
#define CH57xNET_SIZE_UDP_PCB                 0x20                              /* UDP PCB大小 */
#define CH57xNET_SIZE_TCP_PCB                 0xAC                              /* TCP PCB大小 */
#define CH57xNET_SIZE_TCP_PCB_LISTEN          0x20                              /* TCP LISTEN PCB大小 */
#define CH57xNET_SIZE_IP_REASSDATA            0x20                              /* IP分片管理  */
#define CH57xNET_SIZE_PBUF                    0x10                              /* Packet Buf */
#define CH57xNET_SIZE_TCP_SEG                 0x14                              /* TCP SEG结构 */
#define CH57xNET_SIZE_MEM                     0x06                              /* sizeof(struct mem) */
#define CH57xNET_SIZE_ARP_TABLE               0x10                              /* sizeof arp table */

#define CH57xNET_SIZE_POOL_BUF                CH57xNET_MEM_ALIGN_SIZE(CH57xNET_TCP_MSS + 40 +14) /* pbuf池大小 */
#define CH57xNET_MEMP_SIZE                    ((CH57xNET_MEM_ALIGNMENT - 1) + \
                                              (CH57xNET_NUM_IPRAW * CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_IPRAW_PCB)) + \
                                              (CH57xNET_NUM_UDP * CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_UDP_PCB)) + \
                                              (CH57xNET_NUM_TCP * CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_PCB)) + \
                                              (CH57xNET_NUM_TCP_LISTEN * CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_PCB_LISTEN)) + \
                                              (CH57xNET_NUM_TCP_SEG * CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_TCP_SEG)) + \
                                              (CH57xNET_NUM_IP_REASSDATA * CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_IP_REASSDATA)) + \
                                              (CH57xNET_NUM_PBUF * (CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_PBUF) + CH57xNET_MEM_ALIGN_SIZE(0))) + \
                                              (CH57xNET_NUM_POOL_BUF * (CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_PBUF) + CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_POOL_BUF))))                    


#define  HEAP_MEM_ALIGN_SIZE                  (CH57xNET_MEM_ALIGN_SIZE(CH57xNET_SIZE_MEM))
#define  CH57xNET_RAM_HEAP_SIZE               (CH57x_MEM_HEAP_SIZE + (2 * HEAP_MEM_ALIGN_SIZE) + CH57xNET_MEM_ALIGNMENT)
#define  CH57xNET_RAM_ARP_TABLE_SIZE          (CH57xNET_SIZE_ARP_TABLE * CH57xNET_NUM_ARP_TABLE)
/* DNS结构体回调 */
typedef void (*dns_callback)(const char *name, UINT8 *ipaddr, void *callback_arg);

/* 检查配置 */
/* 检查CH57xNET_NUM_IPRAW是否不小于1 */
#if(CH57xNET_NUM_IPRAW < 1)
#error "CH57xNET_NUM_IPRAW Error,Please Config CH57xNET_NUM_IPRAW >= 1"
#endif
/* 检查CH57xNET_SIZE_UDP_PCB是否不小于1 */
#if(CH57xNET_SIZE_UDP_PCB < 1)
#error "CH57xNET_SIZE_UDP_PCB Error,Please Config CH57xNET_SIZE_UDP_PCB >= 1"
#endif
/* 检查CH57xNET_NUM_TCP是否不小于1 */
#if(CH57xNET_NUM_TCP < 1)
#error "CH57xNET_NUM_TCP Error,Please Config CH57xNET_NUM_TCP >= 1"
#endif
/* 检查CH57xNET_NUM_TCP_LISTEN是否不小于1 */
#if(CH57xNET_NUM_TCP_LISTEN < 1)
#error "CH57xNET_NUM_TCP_LISTEN Error,Please Config CH57xNET_NUM_TCP_LISTEN >= 1"
#endif
/* 检查字节对齐必须为4的整数倍 */
#if((CH57xNET_MEM_ALIGNMENT % 4) || (CH57xNET_MEM_ALIGNMENT == 0))
#error "CH57xNET_MEM_ALIGNMENT Error,Please Config CH57xNET_MEM_ALIGNMENT = 4 * N, N >=1"
#endif
/* TCP最大报文段长度 */
#if((CH57xNET_TCP_MSS > 1460) || (CH57xNET_TCP_MSS < 60))
#error "CH57xNET_TCP_MSS Error,Please Config CH57xNET_TCP_MSS >= 60 && CH57xNET_TCP_MSS <= 1460"
#endif
/* ARP缓存表个数 */
#if((CH57xNET_NUM_ARP_TABLE > 0X7F) || (CH57xNET_NUM_ARP_TABLE < 1))
#error "CH57xNET_NUM_ARP_TABLE Error,Please Config CH57xNET_NUM_ARP_TABLE >= 1 && CH57xNET_NUM_ARP_TABLE <= 0X7F"
#endif
/* 检查POOL BUF配置 */
#if(CH57xNET_NUM_POOL_BUF < 1)
#error "CH57xNET_NUM_POOL_BUF Error,Please Config CH57xNET_NUM_POOL_BUF >= 1"
#endif
/* 检查PBUF结构配置 */
#if(CH57xNET_NUM_PBUF < 1)
#error "CH57xNET_NUM_PBUF Error,Please Config CH57xNET_NUM_PBUF >= 1"
#endif
/* 检查IP分配配置 */
#if((CH57xNET_NUM_IP_REASSDATA > 10)||(CH57xNET_NUM_IP_REASSDATA < 1))
#error "CH57xNET_NUM_IP_REASSDATA Error,Please Config CH57xNET_NUM_IP_REASSDATA < 10 && CH57xNET_NUM_IP_REASSDATA >= 1 "
#endif
/* 检查IP分片大小 */
#if(CH57xNET_IP_REASS_PBUFS > CH57xNET_NUM_POOL_BUF)
#error "CH57xNET_IP_REASS_PBUFS Error,Please Config CH57xNET_IP_REASS_PBUFS < CH57xNET_NUM_POOL_BUF"
#endif



/* 库内部函数声明 */

UINT8 CH57xNET_Init(const UINT8* ip,const UINT8* gwip,const UINT8* mask,const UINT8* macaddr); /* 库初始化 */

UINT8 CH57xNET_GetVer(void);                                                    /* 查询库的版本号 */
UINT8 CH57xNET_ConfigLIB(struct _CH57x_CFG *cfg);                               /* 配置库*/
                                                                                
void  CH57xNET_MainTask(void);                                                  /* 库主任务函数，需要一直不断调用 */
                                                                                
void CH57xNET_TimeIsr(UINT16 timperiod);                                        /* 时钟中断服务函数，调用前请配置时钟周期 */
                                                                                
void CH57xNET_ETHIsr(void);                                                     /* CH57xETH中断服务函数 */ 
                                                                                
UINT8 CH57xNET_GetPHYStatus(void);                                              /* 获取PHY状态 */
                                                                                
UINT8 CH57xNET_QueryGlobalInt(void);                                             /* 查询全局中断 */
                                                                                
UINT8 CH57xNET_GetGlobalInt (void);                                             /* 读全局中断并将全局中断清零 */
                                                                                
void CH57xNET_OpenMac(void);                                                    /* 打开MAC */
                                                                                
void CH57xNET_CloseMac(void);                                                   /* 关闭MAC */
                                                                                
UINT8 CH57xNET_SocketCreat(UINT8 *socketid,SOCK_INF *socinf);                   /* 创建socket */
                                                                                
UINT8 CH57xNET_SocketSend(UINT8 socketid,UINT8 *buf,UINT32 *len);               /* Socket发送数据 */
                                                                                
UINT8 CH57xNET_SocketRecv(UINT8 socketid,UINT8 *buf,UINT32 *len);               /* Socket接收数据 */
                                                                                
UINT8 CH57xNET_GetSocketInt(UINT8 sockedid);                                    /* 获取socket中断并清零 */
                                                                                
UINT32 CH57xNET_SocketRecvLen(UINT8 socketid,UINT32 *bufaddr);                  /* 获取socket接收长度 */
                                                                                
UINT8 CH57xNET_SocketConnect(UINT8 socketid);                                   /* TCP连接*/
                                                                                
UINT8 CH57xNET_SocketListen(UINT8 socindex);                                    /* TCP监听 */
                                                                                
UINT8 CH57xNET_SocketClose(UINT8 socindex,UINT8 flag);                          /* 关闭连接 */

void CH57xNET_ModifyRecvBuf(UINT8 sockeid,UINT32 bufaddr,UINT32 bufsize);       /* 修改接收缓冲区 */

UINT8 CH57xNET_SocketUdpSendTo(UINT8 socketid, UINT8 *buf, UINT32 *slen,UINT8 *sip,UINT16 port);/* 向指定的目的IP，端口发送UDP包 */

UINT8 CH57xNET_Aton(const UINT8 *cp, UINT8 *addr);                              /* ASCII码地址转网络地址 */
                                                                                
UINT8 *CH57xNET_Ntoa(UINT8 *ipaddr);                                            /* 网络地址转ASCII地址 */
                                                                                
UINT8 CH57xNET_SetSocketTTL(UINT8 socketid, UINT8 ttl);                         /* 设置socket的TTL */
                                                                                
void CH57xNET_RetrySendUnack(UINT8 socketid);                                   /* TCP重传 */

UINT8 CH57xNET_QueryUnack(SOCK_INF  *sockinf,UINT32 *addrlist,UINT16 lislen)    /* 查询未发送成功的数据包 */;

UINT8 CH57xNET_DHCPStart(UINT8(* usercall)(UINT8 status,void *));               /* DHCP启动 */
                                                                                
UINT8 CH57xNET_DHCPStop(void);                                                  /* DHCP停止 */

void CH57xNET_InitDNS(UINT8 *dnsip,UINT16 port);                                /* DNS初始化 */

UINT8 CH57xNET_GetHostName(const char *hostname,UINT8 *addr,dns_callback found,void *arg);/* DNS获取主机名 */

void CH57xNET_ConfigKeepLive(struct _KEEP_CFG *cfg);                            /* 配置库KEEP LIVE参数 */

UINT8 CH57xNET_SocketSetKeepLive(UINT8 socindex,UINT8 cfg);                     /* 配置socket KEEP LIVE*/

void CH57xNET_SetHostname(char *name);

#ifdef __cplusplus
}
#endif
#endif


