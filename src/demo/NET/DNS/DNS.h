/********************************** (C) COPYRIGHT ******************************
* File Name          : DNS.H
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/01
* Description        : CH579 NET库DNS应用演示
*                  
*******************************************************************************/



/******************************************************************************/
#ifndef    _DNS_H_
#define    _DNS_H_

#define    MAX_DNS_BUF_SIZE    512        /*DNS缓冲区最大长度 */
#define CH563NET_DBG                           1                                /* printf调试使能 */
/*
********************************************************************************
Define Part
********************************************************************************
*/

#define    TYPE_A         1    /* Host address */
#define    TYPE_NS        2    /* Name server */
#define    TYPE_MD        3    /* Mail destination (obsolete) */
#define    TYPE_MF        4    /* Mail forwarder (obsolete) */
#define    TYPE_CNAME     5    /* Canonical name */
#define    TYPE_SOA       6    /* Start of Authority */
#define    TYPE_MB        7    /* Mailbox name (experimental) */
#define    TYPE_MG        8    /* Mail group member (experimental) */
#define    TYPE_MR        9    /* Mail rename name (experimental) */
#define    TYPE_NULL      10   /* Null (experimental) */
#define    TYPE_WKS       11   /* Well-known sockets */
#define    TYPE_PTR       12   /* Pointer record */
#define    TYPE_HINFO     13   /* Host information */
#define    TYPE_MINFO     14   /* Mailbox information (experimental)*/
#define    TYPE_MX        15   /* Mail exchanger */
#define    TYPE_TXT       16   /* Text strings */
#define    TYPE_ANY       255  /* Matches any type */

#define    CLASS_IN    1    /* The ARPA Internet */


struct dhdr         
{             
    UINT16  id;         /* 标识 */
    UINT8   qr;         /* 查询或应答标志*/
    UINT8   opcode;
    UINT8   aa;         /* 授权回答 */
    UINT8   tc;         /* 可截断的 */
    UINT8   rd;         /* 期望递归*/
    UINT8   ra;         /* 可以递归 */
    UINT8   rcode;      /* 应答码 */
    UINT16  qdcount;    /* 问题数 */
    UINT16  ancount;    /* 应答数 */
    UINT16  nscount;    /* 授权数 */
    UINT16  arcount;    /* 额外记录数 */
};

extern UINT8 status;

UINT16 get16(UINT8 * s);

int ParseName(UINT8 * msg, UINT8 * compressed, char * buf);

UINT8 * DnsQuestion(UINT8 * msg, UINT8 * cp);

UINT8 * DnsAnswer(UINT8 * msg, UINT8 * cp, UINT8 * pSip);

UINT8 parseMSG(struct dhdr * pdhdr, UINT8 * pbuf, UINT8 * pSip);

UINT8 * put16(UINT8 * s, UINT16 i);

UINT16 MakeDnsQueryMsg(UINT16 op, char * name, UINT8 * buf, UINT16 len);

UINT8 DnsQuery(UINT8 s, UINT8 * name, UINT8 * pSip);

UINT8 DnsQuery(UINT8 s, UINT8 * name, UINT8 * pSip);

void UDPSocketParamInit(UINT8 sockindex,UINT8 *addr,UINT16 SourPort,UINT16 DesPort);

#endif
