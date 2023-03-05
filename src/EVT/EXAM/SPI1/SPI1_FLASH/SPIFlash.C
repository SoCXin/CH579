/********************************** (C) COPYRIGHT *******************************
 * File Name          : SPIFlash.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2019/4/29
 * Description        : SPI Flash操作接口函数(W25QXX)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"
#include "SPIFlash.H"

/********************************* 引脚定义 ************************************
*    PA3  <===========>  SCS
*    PA0  <===========>  SCK
*    PA1  <===========>  DI/MOSI
*    PA2  <===========>  DO/MISO
*******************************************************************************/

#define  SPI1_CS_LOW()        GPIOA_ResetBits( GPIO_Pin_3 )
#define  SPI1_CS_HIGH()       GPIOA_SetBits( GPIO_Pin_3 )

/*******************************************************************************
* Function Name  : ReadExternalFlashStatusReg_SPI
* Description    : 用来读取状态寄存器,并返回状态寄存器的值
* Input          : None
* Output         : None
* Return         : ExFlashRegStatus
*******************************************************************************/
UINT8 ReadExternalFlashStatusReg_SPI( void )
{
    UINT8 ExFlashRegStatus;
    
    
    SPI1_CS_LOW();
    SPI1_MasterSendByte( CMD_STATUS1 );                                          //发送读状态寄存器的命令
    ExFlashRegStatus = SPI1_MasterRecvByte();                                    //读取状态寄存器
    SPI1_CS_HIGH();
    
    return ExFlashRegStatus;
}

/*******************************************************************************
* Function Name  : WaitExternalFlashIfBusy
* Description    : 等待芯片空闲(在执行Byte-Program, Sector-Erase, Block-Erase, Chip-Erase操作后)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WaitExternalFlashIfBusy( void )
{
    while ((ReadExternalFlashStatusReg_SPI())&0x01 == 0x01 )
    {
        ;    //等待直到Flash空闲
    }
}
/*******************************************************************************
* Function Name  : WriteExternalFlashEnable_SPI
* Description    : 写使能,同样可以用于使能写状态寄存器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WriteExternalFlashEnable_SPI( void )
{
    SPI1_CS_LOW();
    SPI1_MasterSendByte( CMD_WR_ENABLE );                                        //发送写使能命令
    SPI1_CS_HIGH();
}
/*******************************************************************************
* Function Name  : EraseExternal4KFlash_SPI
* Description    : 擦除4K Flash  擦除一个扇区
* Input          : Dst_Addr 0-1 ffff ffff ,清除任意地址所在的扇区。
* Output         : None
* Return         : None
*******************************************************************************/
void EraseExternal4KFlash_SPI( UINT32 Dst_Addr )
{
    WriteExternalFlashEnable_SPI();
    WaitExternalFlashIfBusy();
    
    SPI1_CS_LOW();
    SPI1_MasterSendByte(CMD_ERASE_4KBYTE);                                    //扇区擦除命令
    SPI1_MasterSendByte(((Dst_Addr & 0xFFFFFF) >> 16));                       //发送3字节地址
    SPI1_MasterSendByte(((Dst_Addr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(Dst_Addr & 0xFF);
    SPI1_CS_HIGH();
    
    WaitExternalFlashIfBusy();
}
/*******************************************************************************
* Function Name  : EraseExternalFlash_SPI
* Description    : 擦除32K Flash  擦除一个扇区
* Input          : Dst_Addr 0-1 ffff ffff ,清除任意地址所在的扇区。
* Output         : None
* Return         : None
*******************************************************************************/
void EraseExternal32KFlash_SPI( UINT32 Dst_Addr )
{
    WriteExternalFlashEnable_SPI();
    WaitExternalFlashIfBusy();
    
    SPI1_CS_LOW();
    SPI1_MasterSendByte(CMD_ERASE_32KBYTE);                                    //32K擦除命令
    SPI1_MasterSendByte(((Dst_Addr & 0xFFFFFF) >> 16));                        //发送3字节地址
    SPI1_MasterSendByte(((Dst_Addr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(Dst_Addr & 0xFF);
    SPI1_CS_HIGH();
    
    WaitExternalFlashIfBusy();
}
/*******************************************************************************
* Function Name  : PageWriteExternalFlash_SPI
* Description    : 页写；SPI在一页(0~65535)内写入少于256个字节的数据
* Input          : RcvBuffer:数据存储区
*                  StarAddr:开始写入的地址
*                  Len:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
* Output         : None
* Return         : None
*******************************************************************************/
void PageWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer)
{
    UINT16 i;
    
    WriteExternalFlashEnable_SPI();                                            //SET WEL
    
    SPI1_CS_LOW();                                                            
    SPI1_MasterSendByte(CMD_PAGE_PROG);                                        //发送写页命令
    SPI1_MasterSendByte(((StarAddr & 0xFFFFFF) >> 16));                        //发送24bit地址
    SPI1_MasterSendByte(((StarAddr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(StarAddr & 0xFF);
    for(i=0; i!=Len; i++)
    {
        SPI1_MasterSendByte(RcvBuffer[i]);    //循环写数
    }
    SPI1_CS_HIGH();
    
    WaitExternalFlashIfBusy();                                                 //等待写入结束
}
/*******************************************************************************
* Function Name  : BlukWriteExternalFlash_SPI
* Description    : 无检验写SPI FLASH
*                  必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
*                  具有自动换页功能
*                  在指定地址开始写入指定长度的数据,但是要确保地址不越界!
* Input          : SendBuffer:数据存储区
*                  StarAddr:开始写入的地址
*                  Len:要写入的字节数(最大65535)
* Output         : None
* Return         : None
*******************************************************************************/
void BlukWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 SendBuffer)
{
    UINT16  pageremain;
    
    pageremain = 256-StarAddr%256;                                               //单页剩余的字节数
    if(Len<=pageremain)
    {
        pageremain=Len;    //不大于256个字节
    }
    while(1)
    {
        PageWriteExternalFlash_SPI(StarAddr,pageremain,SendBuffer);
        if(Len==pageremain)
        {
            break;    //写入结束了
        }
        else
        {
            SendBuffer+=pageremain;
            StarAddr+=pageremain;
            Len-=pageremain;                                                   //减去已经写入了的字节数
            if(Len>256)
            {
                pageremain=256;    //一次可以写入256个字节
            }
            else
            {
                pageremain=Len;    //不够256个字节了
            }
        }
    }
}
/*******************************************************************************
* Function Name  : ReadExternalFlash_SPI
* Description    : 读取地址的数据.
*******************************************************************************/
void ReadExternalFlash_SPI( UINT32 StarAddr, UINT16 Len, PUINT8 RcvBuffer )
{    
    SPI1_CS_LOW();                                                      
    SPI1_MasterSendByte(CMD_READ_DATA);                                 //读命令
    SPI1_MasterSendByte(((StarAddr & 0xFFFFFF) >> 16));                 //发送3字节地址
    SPI1_MasterSendByte(((StarAddr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(StarAddr & 0xFF);
    SPI1_MasterRecv( RcvBuffer, Len );
    SPI1_CS_HIGH();
}
/*******************************************************************************
* Function Name  : BlukReadExternalFlash_SPI
* Description    : 读取起始地址(StarAddr)内多个字节(Len)的数据.存入缓冲区RcvBuffer中
* Input          : StarAddr -Destination Address 000000H - 1FFFFFH
                   Len 读取数据长度
                   RcvBuffer 接收缓冲区起始地址
* Output         : None
* Return         : None
*******************************************************************************/
void BlukReadExternalFlash_SPI( UINT32 StarAddr, UINT16 Len, PUINT8 RcvBuffer )
{    
    SPI1_CS_LOW();
    SPI1_MasterSendByte(CMD_FAST_READ);                                 //高速度
    SPI1_MasterSendByte(((StarAddr & 0xFFFFFF) >> 16));                 //发送3字节地址
    SPI1_MasterSendByte(((StarAddr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(StarAddr & 0xFF);
    SPI1_MasterSendByte(0x00);    
    SPI1_MasterRecv( RcvBuffer, Len );
    SPI1_CS_HIGH();
}
/*******************************************************************************
* Function Name  : SPIFlash_ReadID
* Description    : SPI Flash读取芯片ID
* Input          : None
* Output         : None
* Return         : 0XEF13,表示芯片型号为W25Q80
*                  0XEF14,表示芯片型号为W25Q16
*                  0XEF15,表示芯片型号为W25Q32
*                  0XEF16,表示芯片型号为W25Q64
*                  0XEF17,表示芯片型号为W25Q128
*******************************************************************************/
UINT16 SPIFlash_ReadID(void)
{
    UINT16  temp = 0;
    
    SPI1_CS_LOW();
    SPI1_MasterSendByte(CMD_DEVICE_ID);                    //读取ID命令
    SPI1_MasterSendByte(0x00);
    SPI1_MasterSendByte(0x00);
    SPI1_MasterSendByte(0x00);
    temp = SPI1_MasterRecvByte();
    temp = temp<<8;
    temp |= SPI1_MasterRecvByte();
    SPI1_CS_HIGH();
    
    return temp;
}
/*******************************************************************************
* Function Name  : SPI1Flash_Init
* Description    : SPI Flash初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPIFlash_Init(void)
{
	GPIOA_SetBits( GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3 );
	GPIOA_ModeCfg( GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3, GPIO_ModeOut_PP_5mA );	// MOSI/SCK/CS
	GPIOA_SetBits( GPIO_Pin_2 );
	GPIOA_ModeCfg( GPIO_Pin_2, GPIO_ModeIN_PU );	// MISO	
	
    SPI1_MasterDefInit();
    SPI1_DataMode( Mode3_HighBitINFront );
    SPI1_CLKCfg( 4 );
    printf("id:0x%04x\n", SPIFlash_ReadID() );
}


