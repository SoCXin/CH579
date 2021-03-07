




#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__


#ifdef __cplusplus
 extern "C" {
#endif

     
     
#define  CMD_STATUS1         0x05
#define  CMD_WR_ENABLE       0x06
#define  CMD_ERASE_4KBYTE    0x20
#define  CMD_ERASE_32KBYTE   0x52
#define  CMD_READ_DATA       0x03
#define  CMD_PAGE_PROG       0x02
#define  CMD_FAST_READ       0x0B
#define  CMD_DEVICE_ID       0x90
     
     

/* 擦除函数 */
extern void EraseExternal4KFlash_SPI( UINT32 Dst_Addr );                                       //擦除4K
extern void EraseExternal32KFlash_SPI( UINT32 Dst_Addr );                                      //擦除32K
/* 写函数 */
extern void BlukWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 SendBuffer);          //写SPI Flash函数,无限制
/* 读函数 */
extern void ReadExternalFlash_SPI( UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer );              //读SPI Flash函数,无限制
extern void BlukReadExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer);            //读取长度必须为16整数倍，好处是读取速度更快
/* 初始化相关 */
extern UINT16 SPIFlash_ReadID(void);
extern void   SPIFlash_Init(void);



#ifdef __cplusplus
}
#endif


#endif


