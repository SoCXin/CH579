/********************************** (C) COPYRIGHT ******************************
* File Name          : DNS.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/05
* Description        : CH579 NET库DNS应用演示                  
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CH57x_common.h"
#include "core_cm0.h"
#include "CH57xNET.H"
#include "DNS.H"


#define CH57xNET_DBG                          1
/* 变量定义 */
UINT8 DNS_SERVER_IP[4]={114,114,114,114};                                       /* DNS服务器 */
#define    IPPORT_DOMAIN    53                                                  /* DNS默认端口 */
UINT16 MSG_ID = 0x1100;                                                         /* 标识 */
UINT32 count = 0;
UINT8 dns_buf[MAX_DNS_BUF_SIZE];

/*******************************************************************************
* Function Name  : get16
* Description    : 将缓冲区UINT8数据转为UINT16格式数据
* Input          : s -UINT8类型数据
* Output         : None
* Return         : 转化后的UINT16类型数据
*******************************************************************************/
UINT16 get16(UINT8 * s)
{
    UINT16 i;

    i = *s++ << 8;
    i = i + *s;
    return i;
}

/*******************************************************************************
* Function Name  : ParseName
* Description    : 分析完整的域名
* Input          : msg        -指向报文的指针
                   compressed -指向报文中主域名的指针
                   buf        -缓冲区指针，用于存放转化后域名
* Output         : None
* Return         : 压缩报文的长度
*******************************************************************************/
int ParseName(UINT8 * msg, UINT8 * compressed, char * buf)
{
    UINT16 slen;                                                                /* 当前片段长度*/
    UINT8 * cp;
    int clen = 0;                                                               /* 压缩域名长度 */
    int indirect = 0;
    int nseg = 0;                                                               /* 域名被分割的片段总数 */

    cp = compressed;
    for (;;){
        slen = *cp++;                                                           /* 首字节的计数值*/
        if (!indirect) clen++;
        if ((slen & 0xc0) == 0xc0){                                             /* 计数字节高两比特为1，用于压缩格式 */
            if (!indirect) clen++;
            indirect = 1;
            cp = &msg[((slen & 0x3f)<<8) + *cp];                                /* 按计数字节数值指针偏移到指定位置 */
            slen = *cp++;
        }
        if (slen == 0) break;                                                   /* 计数为0，结束 */
        if (!indirect) clen += slen;
        while (slen-- != 0) *buf++ = (char)*cp++;
        *buf++ = '.';
        nseg++;
    }
    if (nseg == 0){
        /* 根域名; */
        *buf++ = '.';
    }
    *buf++ = '\0';
    return clen;                                                                /* 压缩报文长度 */
}

/*******************************************************************************
* Function Name  : DnsQuestion
* Description    : 分析响应报文中的问题记录部分
* Input          : msg  -指向响应报文的指针
                   cp   -指向问题记录的指针
* Output         : None
* Return         : 指向下一记录的指针
*******************************************************************************/
UINT8 * DnsQuestion(UINT8 * msg, UINT8 * cp)
{
    int len;
    char name[MAX_DNS_BUF_SIZE];

    len = ParseName(msg, cp, name);
    cp += len;
    cp += 2;                                                                    /* 类型 */
    cp += 2;                                                                    /* 类 */
    return cp;
}

/*******************************************************************************
* Function Name  : DnsAnswer
* Description    : 分析响应报文中的回答记录部分
* Input          : msg  -指向响应报文的指针
                   cp   -指向回答记录的指针
                   psip           
* Output         : None
* Return         :指向下一记录的指针
*******************************************************************************/
UINT8 * DnsAnswer(UINT8 * msg, UINT8 * cp, UINT8 * pSip)
{
    int len, type;
    char name[MAX_DNS_BUF_SIZE];

    len = ParseName(msg, cp, name);
    cp += len;
    type = get16(cp);
    cp += 2;        /* 类型 */
    cp += 2;        /* 类 */
    cp += 4;        /* 生存时间 */
    cp += 2;        /* 资源数据长度 */
    switch ( type ){
        case TYPE_A:
            pSip[0] = *cp++;
            pSip[1] = *cp++;
            pSip[2] = *cp++;
            pSip[3] = *cp++;
            break;
        case TYPE_CNAME:
        case TYPE_MB:
        case TYPE_MG:
        case TYPE_MR:
        case TYPE_NS:
        case TYPE_PTR:
            len = ParseName(msg, cp, name);
            cp += len;
            break;
        case TYPE_HINFO:
        case TYPE_MX:
        case TYPE_SOA:
        case TYPE_TXT:
            break;
        default:
            break;
    }
    return cp;
}

/*******************************************************************************
* Function Name  : parseMSG
* Description    : 分析响应报文中的资源记录部分
* Input          : msg  -指向DNS报文头部的指针
                   cp   -指向响应报文的指针
* Output         : None
* Return         :成功返回1，否则返回0
*******************************************************************************/

UINT8 parseMSG(struct dhdr * pdhdr, UINT8 * pbuf, UINT8 * pSip)
{
    UINT16 tmp;
    UINT16 i;
    UINT8 * msg;
    UINT8 * cp;

    msg = pbuf;
    memset(pdhdr, 0, sizeof(pdhdr));
    pdhdr->id = get16(&msg[0]);
    tmp = get16(&msg[2]);
    if (tmp & 0x8000) pdhdr->qr = 1;
    pdhdr->opcode = (tmp >> 11) & 0xf;
    if (tmp & 0x0400) pdhdr->aa = 1;
    if (tmp & 0x0200) pdhdr->tc = 1;
    if (tmp & 0x0100) pdhdr->rd = 1;
    if (tmp & 0x0080) pdhdr->ra = 1;
    pdhdr->rcode = tmp & 0xf;
    pdhdr->qdcount = get16(&msg[4]);
    pdhdr->ancount = get16(&msg[6]);
    pdhdr->nscount = get16(&msg[8]);
    pdhdr->arcount = get16(&msg[10]);
    /* 分析可变数据长度部分*/
    cp = &msg[12];
    /* 查询问题 */
    for (i = 0; i < pdhdr->qdcount; i++)
    {
        cp = DnsQuestion(msg, cp);
    }
    /* 回答 */
    for (i = 0; i < pdhdr->ancount; i++)
    {
        cp = DnsAnswer(msg, cp, pSip);
    }
    /*授权 */
    for (i = 0; i < pdhdr->nscount; i++)
    {
      /*待解析*/    ;
    }
    /* 附加信息 */
    for (i = 0; i < pdhdr->arcount; i++)
    {
      /*待解析*/    ;
    }
    if(pdhdr->rcode == 0) return 1;                                             /* rcode = 0:成功 */
    else return 0;
}

/*******************************************************************************
* Function Name  : put16
* Description    :UINT16 格式数据按UINT8格式存到缓冲区
* Input          : s -缓冲区首地址
                   i -UINT16数据
* Output         : None
* Return         : 偏移指针
*******************************************************************************/
UINT8 * put16(UINT8 * s, UINT16 i)
{
    *s++ = i >> 8;
    *s++ = i;
    return s;
}

/*******************************************************************************
* Function Name  : MakeDnsQuery
* Description    : 制作DNS查询报文
  input          : op   - 递归
*                  name - 指向待查域名指针
*                  buf  - DNS缓冲区.
*                  len  - 缓冲区最大长度.
* Output         : None
* Return         : 指向DNS报文指针
*******************************************************************************/
UINT16 MakeDnsQueryMsg(UINT16 op, char * name, UINT8 * buf, UINT16 len)
{
    UINT8 *cp;
    char *cp1;
    char tmpname[MAX_DNS_BUF_SIZE];
    char *dname;
    UINT16 p;
    UINT16 dlen;

	//printf("Domain name:%s \n",name);
    cp = buf;
    MSG_ID++;
    cp = put16(cp, MSG_ID);                                                     /* 标识 */
    p = (op << 11) | 0x0100;            
    cp = put16(cp, p);                                                          /* 0x0100：Recursion desired */
    cp = put16(cp, 1);                                                          /* 问题数：1 */
    cp = put16(cp, 0);                                                          /* 资源记录数：0 */
    cp = put16(cp, 0);                                                          /* 资源记录数：0 */
    cp = put16(cp, 0);                                                          /* 额外资源记录数：0 */

    strcpy(tmpname, name);
    dname = tmpname;
    dlen = strlen(dname);
    for (;;){                                                                   /* 按照DNS请求报文域名格式，把URI写入到buf里面去 */
        cp1 = strchr(dname, '.');
        if (cp1 != NULL) len = cp1 - dname;    
        else len = dlen;        
        *cp++ = len;            
        if (len == 0) break;
        strncpy((char *)cp, dname, len);
        cp += len;
        if (cp1 == NULL)
        {
            *cp++ = 0;        
            break;
        }
        dname += len+1;                                                         /* dname首地址后移 */
        dlen -= len+1;                                                          /* dname长度减小 */
    }
    cp = put16(cp, 0x0001);                                                     /* type ：1------ip地址 */
    cp = put16(cp, 0x0001);                                                     /* class：1-------互联网地址 */
    return ((UINT16)(cp - buf));
}

/*******************************************************************************
* Function Name  : DnsQuery
* Description    : 进行DNS查询
  input          : s    -socket索引
*                  name - 指向待查域名指针
                   pSip -查询结果
* Output         : None
* Return         : 查询结果。成功返回1，失败返回-1
*******************************************************************************/
UINT8 DnsQuery(UINT8 s, UINT8 * name, UINT8 * pSip)
{
    struct dhdr dhp;

    UINT8 ret;
    UINT32 len;
    if(status >1 ){
        count++;
        DelsyMs(10);
        if( count>20000 ){
#if CH57xNET_DBG
            printf("DNS Fail!!!!!\n");
#endif
            count=0;
            status = 0;
            return 2;
        } 
    }
    if(status == 1)
    {
        UDPSocketParamInit(s,DNS_SERVER_IP,4000,IPPORT_DOMAIN);
        status = 2;
#if CH57xNET_DBG
        printf(" 2 status = %d!\n",status);
#endif
    }
    if(status ==2)
    {
        len = MakeDnsQueryMsg(0,(char *)name, dns_buf, MAX_DNS_BUF_SIZE);
		ret = CH57xNET_SocketSend(s,dns_buf,&len);
        if ( ret ) return(0);
        else{
            status = 3;
#if CH57xNET_DBG
            printf("status = 3!\n");
#endif
        }
    }
    if(status ==4)
    {
        return(parseMSG(&dhp, dns_buf, pSip));                                  /*解析响应报文并返回结果*/
		
    }
    return 0;
}

/*********************************** endfile **********************************/
