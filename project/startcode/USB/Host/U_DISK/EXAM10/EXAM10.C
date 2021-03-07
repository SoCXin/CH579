/********************************** (C) COPYRIGHT *******************************
* File Name          : EXAM1.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/15
* Description        :
 C语言的U盘文件创建，修改文件属性，删除文件等操作
 支持: FAT12/FAT16/FAT32
 注意包含 CH579UFI.LIB/USBHOST.C/DEBUG.C
*******************************************************************************/

/** 不使用U盘文件系统库或者U盘挂载USBhub下面，需要关闭定义 #define	FOR_ROOT_UDISK_ONLY  */
/** 使用U盘文件系统库，需要开启下面定义, 不使用请关闭 #define DISK_BASE_BUF_LEN		512	      */

#include "CH57x_common.h"
#include "CH579UFI.H"

__align(4) UINT8  RxBuffer[ MAX_PACKET_SIZE ];  // IN, must even address
__align(4) UINT8  TxBuffer[ MAX_PACKET_SIZE ];  // OUT, must even address

UINT8  buf[100];                                  //长度可以根据应用自己指定


/* 检查操作状态,如果错误则显示错误代码并停机 */
void mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* 操作成功 */
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* 显示错误 */
    /* 遇到错误后,应该分析错误码以及CH554DiskStatus状态,例如调用CH579DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
       建议出错后的处理步骤:
       1、调用一次CH579DiskReady,成功则继续操作,例如Open,Read/Write等
       2、如果CH579DiskReady不成功,那么强行将从头开始操作(等待U盘连接，CH554DiskReady等) */
    while ( 1 )
    {  }
}

int main( )
{
    UINT8   s, c, i;
    
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PWR_UnitModCfg( ENABLE, UNIT_SYS_PLL );		// 打开PLL
    DelayMs(5);
    
	GPIOA_SetBits(GPIO_Pin_9);
	GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
	GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
	UART1_DefInit();
	PRINT( "Start @ChipID=%02X %s \n", R8_CHIP_ID, __TIME__);

    pHOST_RX_RAM_Addr = RxBuffer;
    pHOST_TX_RAM_Addr = TxBuffer;
	USB_HostInit();    
    CH579LibInit( );                           //初始化U盘程序库以支持U盘文件
    
    FoundNewDev = 0;
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( R8_USB_INT_FG & RB_UIF_DETECT )   // 如果有USB主机检测中断则处理
        {
            R8_USB_INT_FG = RB_UIF_DETECT ;                // 清连接中断标志
            s = AnalyzeRootHub( );                                          // 分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT ) 		FoundNewDev = 1;
        }
        
        if ( FoundNewDev || s == ERR_USB_CONNECT )                         // 有新的USB设备插入
        {
            FoundNewDev = 0;
            mDelaymS( 200 );                                               // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
            s = InitRootDevice();  // 初始化USB设备
            if ( s == ERR_SUCCESS )
            {
                // U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH579子程序库继续完成后续工作
                CH579DiskStatus = DISK_USB_ADDR;
                for ( i = 0; i != 10; i ++ )
                {
                    printf( "Wait DiskReady\n" );
                    s = CH579DiskReady( );                                 //等待U盘准备好
                    if ( s == ERR_SUCCESS )
                    {
                        break;
                    }
                    else
                    {
                        printf("%02x\n",(UINT16)s);
                    }
                    mDelaymS( 50 );
                }
                
                if ( CH579DiskStatus >= DISK_MOUNTED )
                {                    
                    //创建文件演示
                    printf( "Create\n" );
                    strcpy( (PCHAR)mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* 新文件名,在根目录下,中文文件名 */
                    s = CH579FileCreate( );                                        /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
                    mStopIfError( s );
                    printf( "ByteWrite\n" );
                    //实际应该判断写数据长度和定义缓冲区长度是否相符，如果大于缓冲区长度则需要多次写入
                    i = sprintf( (PCHAR)buf,"Note: \xd\xa这个程序是以字节为单位进行U盘文件读写,579简单演示功能。\xd\xa");  /*演示 */
                    for(c=0; c<10; c++)
                    {
                        mCmdParam.ByteWrite.mByteCount = i;                          /* 指定本次写入的字节数 */
                        mCmdParam.ByteWrite.mByteBuffer = buf;                       /* 指向缓冲区 */
                        s = CH579ByteWrite( );                                       /* 以字节为单位向文件写入数据 */
                        mStopIfError( s );
                        printf("成功写入 %02X次\n",(UINT16)c);
                    }
                    //演示修改文件属性
										printf( "Modify\n" );
										mCmdParam.Modify.mFileAttr = 0xff;   //输入参数: 新的文件属性,为0FFH则不修改
										mCmdParam.Modify.mFileTime = 0xffff;   //输入参数: 新的文件时间,为0FFFFH则不修改,使用新建文件产生的默认时间
										mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //输入参数: 新的文件日期: 2015.05.18
										mCmdParam.Modify.mFileSize = 0xffffffff;  // 输入参数: 新的文件长度,以字节为单位写文件应该由程序库关闭文件时自动更新长度,所以此处不修改
										i = CH579FileModify( );   //修改当前文件的信息,修改日期
										mStopIfError( i );
                    printf( "Close\n" );
                    mCmdParam.Close.mUpdateLen = 1;                                  /* 自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度 */
                    i = CH579FileClose( );
                    mStopIfError( i );
                    
//                  strcpy( (PCHAR)mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* 新文件名,在根目录下,中文文件名 */
//                  s = CH579FileOpen( );                                        /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
//                  mStopIfError( s );
                   
                    
                  /* 删除某文件 */
                  printf( "Erase\n" );
                  strcpy( (PCHAR)mCmdParam.Create.mPathName, "/OLD" );  //将被删除的文件名,在根目录下
                  i = CH579FileErase( );  //删除文件并关闭
                  if ( i != ERR_SUCCESS ) printf( "Error: %02X\n", (UINT16)i );  //显示错误
                }
            }
        }
        mDelaymS( 100 );  // 模拟单片机做其它事
        SetUsbSpeed( 1 );  // 默认为全速
    }
}


