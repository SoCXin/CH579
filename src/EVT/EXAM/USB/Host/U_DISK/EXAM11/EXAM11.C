/********************************** (C) COPYRIGHT *******************************
 * File Name          : EXAM11.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/08/15
 * Description        : CH579 C语言的U盘目录文件枚举程序
 * 支持: FAT12/FAT16/FAT32
 * 注意包含 CH579UFI.LIB/USBHOST.C/DEBUG.C
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/** 不使用U盘文件系统库或者U盘挂载USBhub下面，需要关闭定义 #define	FOR_ROOT_UDISK_ONLY  */
/** 使用U盘文件系统库，需要开启下面定义, 不使用请关闭 #define DISK_BASE_BUF_LEN		512	      */


#include "CH57x_common.h"
#include "CH579UFI.H"

__align(4) UINT8  RxBuffer[ MAX_PACKET_SIZE ];  // IN, must even address
__align(4) UINT8  TxBuffer[ MAX_PACKET_SIZE ];  // OUT, must even address


/* 检查操作状态,如果错误则显示错误代码并停机 */
void    mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* 操作成功 */
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* 显示错误 */
    /* 遇到错误后,应该分析错误码以及CH579DiskStatus状态,例如调用CH579DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
       建议出错后的处理步骤:
       1、调用一次CH579DiskReady,成功则继续操作,例如Open,Read/Write等
       2、如果CH579DiskReady不成功,那么强行将从头开始操作(等待U盘连接，CH579DiskReady等) */
    while ( 1 )
    {   }
}


int main( )
{
    UINT8   s, i;
    PUINT8  pCodeStr;
    UINT16  j;

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
    printf( "Wait Device In\n" );
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( R8_USB_INT_FG & RB_UIF_DETECT )   // 如果有USB主机检测中断则处理
        {
            R8_USB_INT_FG = RB_UIF_DETECT ;                // 清连接中断标志
            s = AnalyzeRootHub( );                                          // 分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT ) 		FoundNewDev = 1;
        }
        
        if ( FoundNewDev || s == ERR_USB_CONNECT )
        {
            // 有新的USB设备插入
            FoundNewDev = 0;
            mDelaymS( 200 );                                                    // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
            s = InitRootDevice( );                                              // 初始化USB设备
            if ( s == ERR_SUCCESS )
            {
                printf( "Start UDISK_demo @CH579UFI library\n" );
                // U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH579子程序库继续完成后续工作
                CH579DiskStatus = DISK_USB_ADDR;
                for ( i = 0; i != 10; i ++ )
                {
                    printf( "Wait DiskReady\n" );
                    s = CH579DiskReady( );
                    if ( s == ERR_SUCCESS )
                    {
                        break;
                    }
                    mDelaymS( 50 );
                }
                if ( CH579DiskStatus >= DISK_MOUNTED )                           //U盘准备好
                {                    
                    /* 读文件 */
                    printf( "Open\n" );
                    strcpy( (PCHAR)mCmdParam.Open.mPathName, "/C51/CH579HFT.C" );         //设置要操作的文件名和路径
                    s = CH579FileOpen( );                                          //打开文件
                    if ( s == ERR_MISS_DIR )
                    {
                        printf("不存在该文件夹则列出根目录所有文件\n");
                        pCodeStr = (PUINT8)"/*";
                    }
                    else
                    {
                        pCodeStr = (PUINT8)"/C51/*";    //列出\C51子目录下的的文件
                    }
                    
                    printf( "List file %s\n", pCodeStr );
                    for ( j = 0; j < 10000; j ++ )                                 //限定10000个文件,实际上没有限制
                    {
                        strcpy( (PCHAR)mCmdParam.Open.mPathName, (PCCHAR)pCodeStr );              //搜索文件名,*为通配符,适用于所有文件或者子目录
                        i = strlen( (PCHAR)mCmdParam.Open.mPathName );
                        mCmdParam.Open.mPathName[ i ] = 0xFF;                      //根据字符串长度将结束符替换为搜索的序号,从0到254,如果是0xFF即255则说明搜索序号在CH579vFileSize变量中
                        CH579vFileSize = j;                                        //指定搜索/枚举的序号
                        i = CH579FileOpen( );                                      //打开文件,如果文件名中含有通配符*,则为搜索文件而不打开
                        /* CH579FileEnum 与 CH579FileOpen 的唯一区别是当后者返回ERR_FOUND_NAME时那么对应于前者返回ERR_SUCCESS */
                        if ( i == ERR_MISS_FILE )
                        {
                            break;    //再也搜索不到匹配的文件,已经没有匹配的文件名
                        }
                        if ( i == ERR_FOUND_NAME )                                 //搜索到与通配符相匹配的文件名,文件名及其完整路径在命令缓冲区中
                        {
                            printf( "  match file %04d#: %s\n", (unsigned int)j, mCmdParam.Open.mPathName );  /* 显示序号和搜索到的匹配文件名或者子目录名 */
                            continue;                                                /* 继续搜索下一个匹配的文件名,下次搜索时序号会加1 */
                        }
                        else                                                       //出错
                        {
                            mStopIfError( i );
                            break;
                        }
                    }
                    i = CH579FileClose( );                                          //关闭文件
                    printf( "U盘演示完成\n" );                     
                }
                else
                {
                    printf( "U盘没有准备好 ERR =%02X\n", (UINT16)s );
                }
            }
            else
            {
                printf("初始化U盘失败，请拔下U盘重试\n");
            }
        }
        mDelaymS( 100 );  // 模拟单片机做其它事
        SetUsbSpeed( 1 );  // 默认为全速
    }
}



