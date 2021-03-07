/* 2014.09.09
*****************************************
**   Copyright  (C)  W.ch  1999-2019   **
**   Web:      http://wch.cn           **
*****************************************
**  USB-flash File Interface for CH579 **
**  KEIL423, ARM Compiler 4.1          **
*****************************************
*/
/* CH579 U盘主机文件系统接口, 支持: FAT12/FAT16/FAT32 */

//#define DISK_BASE_BUF_LEN		512	/* 默认的磁盘数据缓冲区大小为512字节(可以选择为2048甚至4096以支持某些大扇区的U盘),为0则禁止在本文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
/* 如果需要复用磁盘数据缓冲区以节约RAM,那么可将DISK_BASE_BUF_LEN定义为0以禁止在本文件中定义缓冲区,而由应用程序在调用CH579LibInit之前将与其它程序合用的缓冲区起始地址置入pDISK_BASE_BUF变量 */

//#define NO_DEFAULT_ACCESS_SECTOR	1		/* 禁止默认的磁盘扇区读写子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_DISK_CONNECT		1		/* 禁止默认的检查磁盘连接子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_FILE_ENUMER		1		/* 禁止默认的文件名枚举回调程序,下面用自行编写的程序代替它 */

#include "CH57x_common.h"
#include "CH579UFI.H"

CMD_PARAM_I	mCmdParam;						/* 命令参数 */
#if		DISK_BASE_BUF_LEN > 0
//UINT8	DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((at(BA_RAM+SZ_RAM/2)));	/* 外部RAM的磁盘数据缓冲区,缓冲区长度为一个扇区的长度 */
UINT8	DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));	/* 外部RAM的磁盘数据缓冲区,缓冲区长度为一个扇区的长度 */
//UINT8	DISK_FAT_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));	/* 外部RAM的磁盘FAT数据缓冲区,缓冲区长度为一个扇区的长度 */
#endif

/* 以下程序可以根据需要修改 */

#ifndef	NO_DEFAULT_ACCESS_SECTOR		/* 在应用程序中定义NO_DEFAULT_ACCESS_SECTOR可以禁止默认的磁盘扇区读写子程序,然后用自行编写的程序代替它 */
//if ( use_external_interface ) {  // 替换U盘扇区底层读写子程序
//    CH579vSectorSize=512;  // 设置实际的扇区大小,必须是512的倍数,该值是磁盘的扇区大小
//    CH579vSectorSizeB=9;   // 设置实际的扇区大小的位移数,512则对应9,1024对应10,2048对应11
//    CH579DiskStatus=DISK_MOUNTED;  // 强制块设备连接成功(只差分析文件系统)
//}

UINT8	CH579ReadSector( UINT8 SectCount, PUINT8 DataBuf )  /* 从磁盘读取多个扇区的数据到缓冲区中 */
{
	UINT8	retry;
//	if ( use_external_interface ) return( extReadSector( CH579vLbaCurrent, SectCount, DataBuf ) );  /* 外部接口 */
	for( retry = 0; retry < 3; retry ++ ) {  /* 错误重试 */
		pCBW -> mCBW_DataLen = (UINT32)SectCount << CH579vSectorSizeB;  /* 数据传输长度 */
		pCBW -> mCBW_Flag = 0x80;
		pCBW -> mCBW_LUN = CH579vCurrentLun;
		pCBW -> mCBW_CB_Len = 10;
		pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_READ10;
		pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 2 ] = (UINT8)( CH579vLbaCurrent >> 24 );
		pCBW -> mCBW_CB_Buf[ 3 ] = (UINT8)( CH579vLbaCurrent >> 16 );
		pCBW -> mCBW_CB_Buf[ 4 ] = (UINT8)( CH579vLbaCurrent >> 8 );
		pCBW -> mCBW_CB_Buf[ 5 ] = (UINT8)( CH579vLbaCurrent );
		pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
		pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
		CH579BulkOnlyCmd( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( CH579IntStatus == ERR_SUCCESS ) {
			return( ERR_SUCCESS );
		}
		CH579IntStatus = CH579AnalyzeError( retry );
		if ( CH579IntStatus != ERR_SUCCESS ) {
			return( CH579IntStatus );
		}
	}
	return( CH579IntStatus = ERR_USB_DISK_ERR );  /* 磁盘操作错误 */
}

#ifdef	EN_DISK_WRITE
UINT8	CH579WriteSector( UINT8 SectCount, PUINT8 DataBuf )  /* 将缓冲区中的多个扇区的数据块写入磁盘 */
{
	UINT8	retry;
//	if ( use_external_interface ) return( extWriteSector( CH579vLbaCurrent, SectCount, DataBuf ) );  /* 外部接口 */
	for( retry = 0; retry < 3; retry ++ ) {  /* 错误重试 */
		pCBW -> mCBW_DataLen = (UINT32)SectCount << CH579vSectorSizeB;  /* 数据传输长度 */
		pCBW -> mCBW_Flag = 0x00;
		pCBW -> mCBW_LUN = CH579vCurrentLun;
		pCBW -> mCBW_CB_Len = 10;
		pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_WRITE10;
		pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 2 ] = (UINT8)( CH579vLbaCurrent >> 24 );
		pCBW -> mCBW_CB_Buf[ 3 ] = (UINT8)( CH579vLbaCurrent >> 16 );
		pCBW -> mCBW_CB_Buf[ 4 ] = (UINT8)( CH579vLbaCurrent >> 8 );
		pCBW -> mCBW_CB_Buf[ 5 ] = (UINT8)( CH579vLbaCurrent );
		pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
		pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
		CH579BulkOnlyCmd( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( CH579IntStatus == ERR_SUCCESS ) {
			mDelayuS( 200 );  /* 写操作后延时 */
			return( ERR_SUCCESS );
		}
		CH579IntStatus = CH579AnalyzeError( retry );
		if ( CH579IntStatus != ERR_SUCCESS ) {
			return( CH579IntStatus );
		}
	}
	return( CH579IntStatus = ERR_USB_DISK_ERR );  /* 磁盘操作错误 */
}
#endif
#endif  // NO_DEFAULT_ACCESS_SECTOR

#ifndef	NO_DEFAULT_DISK_CONNECT			/* 在应用程序中定义NO_DEFAULT_DISK_CONNECT可以禁止默认的检查磁盘连接子程序,然后用自行编写的程序代替它 */
// only one host port
#define	UHUB0_CTRL		R8_UHOST_CTRL
#define	UHUB1_CTRL		R8_UHOST_CTRL
#define	bUHS_H0_ATTACH	RB_UMS_DEV_ATTACH
#define	bUHS_H1_ATTACH	RB_UMS_DEV_ATTACH
/* 检查磁盘是否连接 */
UINT8	CH579DiskConnect( void )
{
	R8_USB_DEV_AD &= 0x7F;
	if ( R8_USB_DEV_AD == USB_DEVICE_ADDR || R8_USB_DEV_AD == USB_DEVICE_ADDR + 1 ) {  /* 内置Root-HUB下的USB设备 */
		if ( ( R8_USB_DEV_AD == USB_DEVICE_ADDR ? UHUB0_CTRL : UHUB1_CTRL ) & RB_UH_PORT_EN ) {  /* 内置Root-HUB下的USB设备存在且未插拔 */
			return( ERR_SUCCESS );  /* USB设备已经连接且未插拔 */
		}
		else if ( R8_USB_MIS_ST & (UINT8)( R8_USB_DEV_AD == USB_DEVICE_ADDR ? bUHS_H0_ATTACH : bUHS_H1_ATTACH ) ) {  /* 内置Root-HUB下的USB设备存在 */
mDiskConnect:
			CH579DiskStatus = DISK_CONNECT;  /* 曾经断开过 */
			return( ERR_SUCCESS );  /* 外部HUB或USB设备已经连接或者断开后重新连接 */
		}
		else {  /* USB设备断开 */
mDiskDisconn:
			CH579DiskStatus = DISK_DISCONNECT;
			return( ERR_USB_DISCON );
		}
	}
#ifndef	FOR_ROOT_UDISK_ONLY
	else if ( R8_USB_DEV_AD > 0x10 && R8_USB_DEV_AD <= 0x14 || R8_USB_DEV_AD > 0x20 && R8_USB_DEV_AD <= 0x24 ) {  /* 外部HUB的端口下的USB设备 */
		if ( ( R8_USB_DEV_AD & 0x20 ? UHUB1_CTRL : UHUB0_CTRL ) & RB_UH_PORT_EN ) {  /* 内置Root-HUB下的外部HUB存在且未插拔 */
			TxBuffer[ MAX_PACKET_SIZE - 1 ] = R8_USB_DEV_AD;  /* 备份 */
			R8_USB_DEV_AD = USB_DEVICE_ADDR - 1 + ( R8_USB_DEV_AD >> 4 );  /* 设置USB主机端的USB地址指向HUB */
			CH579IntStatus = HubGetPortStatus( TxBuffer[ MAX_PACKET_SIZE - 1 ] & 0x0F );  /* 查询HUB端口状态,返回在TxBuffer中 */
			if ( CH579IntStatus == ERR_SUCCESS ) {
				if ( TxBuffer[2] & (1<<(HUB_C_PORT_CONNECTION-0x10)) ) {  /* 检测到HUB端口上的插拔事件 */
					CH579DiskStatus = DISK_DISCONNECT;  /* 假定为HUB端口上的USB设备断开 */
					HubClearPortFeature( TxBuffer[ MAX_PACKET_SIZE - 1 ] & 0x0F, HUB_C_PORT_CONNECTION );  /* 清除HUB端口连接事件状态 */
				}
				R8_USB_DEV_AD = TxBuffer[ MAX_PACKET_SIZE - 1 ];  /* 设置USB主机端的USB地址指向USB设备 */
				if ( TxBuffer[0] & (1<<HUB_PORT_CONNECTION) ) {  /* 连接状态 */
					if ( CH579DiskStatus < DISK_CONNECT ) {
						CH579DiskStatus = DISK_CONNECT;  /* 曾经断开过 */
					}
					return( ERR_SUCCESS );  /* USB设备已经连接或者断开后重新连接 */
				}
				else {
//					CH579DiskStatus = DISK_DISCONNECT;
//					return( ERR_USB_DISCON );
					CH579DiskStatus = DISK_CONNECT;
					return( ERR_HUB_PORT_FREE );  /* HUB已经连接但是HUB端口尚未连接磁盘 */
				}
			}
			else {
				R8_USB_DEV_AD = TxBuffer[ MAX_PACKET_SIZE - 1 ];  /* 设置USB主机端的USB地址指向USB设备 */
				if ( CH579IntStatus == ERR_USB_DISCON ) {
//					CH579DiskStatus = DISK_DISCONNECT;
//					return( ERR_USB_DISCON );
					goto mDiskDisconn;
				}
				else {
					CH579DiskStatus = DISK_CONNECT;  /* HUB操作失败 */
					return( CH579IntStatus );
				}
			}
		}
		else if ( R8_USB_MIS_ST & (UINT8)( R8_USB_DEV_AD & 0x20 ? bUHS_H1_ATTACH : bUHS_H0_ATTACH ) ) {  /* 内置Root-HUB下的USB设备存在,外部HUB或USB设备已经连接或者断开后重新连接 */
//			CH579DiskStatus = DISK_CONNECT;  /* 曾经断开过 */
//			return( ERR_SUCCESS );  /* 外部HUB或USB设备已经连接或者断开后重新连接 */
			goto mDiskConnect;
		}
		else {  /* 外部HUB断开 */
			CH579DiskStatus = DISK_DISCONNECT;
		}
	}
#endif
	else {
//		CH579DiskStatus = DISK_DISCONNECT;
//		return( ERR_USB_DISCON );
		goto mDiskDisconn;
	}
}
#endif  // NO_DEFAULT_DISK_CONNECT

#ifndef	NO_DEFAULT_FILE_ENUMER			/* 在应用程序中定义NO_DEFAULT_FILE_ENUMER可以禁止默认的文件名枚举回调程序,然后用自行编写的程序代替它 */
void xFileNameEnumer( void )			/* 文件名枚举回调子程序 */
{
/* 如果指定枚举序号CH579vFileSize为0xFFFFFFFF后调用FileOpen，那么每搜索到一个文件FileOpen都会调用本回调程序，
   回调程序xFileNameEnumer返回后，FileOpen递减CH579vFileSize并继续枚举直到搜索不到文件或者目录。建议做法是，
   在调用FileOpen之前定义一个全局变量为0，当FileOpen回调本程序后，本程序由CH579vFdtOffset得到结构FAT_DIR_INFO，
   分析结构中的DIR_Attr以及DIR_Name判断是否为所需文件名或者目录名，记录相关信息，并将全局变量计数增量，
   当FileOpen返回后，判断返回值如果是ERR_MISS_FILE或ERR_FOUND_NAME都视为操作成功，全局变量为搜索到的有效文件数。
   如果在本回调程序xFileNameEnumer中将CH579vFileSize置为1，那么可以通知FileOpen提前结束搜索。以下是回调程序例子 */
#if		0
	UINT8			i;
	UINT16			FileCount;
	PX_FAT_DIR_INFO	pFileDir;
	PUINT8			NameBuf;
	pFileDir = (PX_FAT_DIR_INFO)( pDISK_BASE_BUF + CH579vFdtOffset );  /* 当前FDT的起始地址 */
	FileCount = (UINT16)( 0xFFFFFFFF - CH579vFileSize );  /* 当前文件名的枚举序号,CH579vFileSize初值是0xFFFFFFFF,找到文件名后递减 */
	if ( FileCount < sizeof( FILE_DATA_BUF ) / 12 ) {  /* 检查缓冲区是否足够存放,假定每个文件名需占用12个字节存放 */
		NameBuf = & FILE_DATA_BUF[ FileCount * 12 ];  /* 计算保存当前文件名的缓冲区地址 */
		for ( i = 0; i < 11; i ++ ) NameBuf[ i ] = pFileDir -> DIR_Name[ i ];  /* 复制文件名,长度为11个字符,未处理空格 */
//		if ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) NameBuf[ i ] = 1;  /* 判断是目录名 */
		NameBuf[ i ] = 0;  /* 文件名结束符 */
	}
#endif
}
#endif  // NO_DEFAULT_FILE_ENUMER

UINT8	CH579LibInit( void )  /* 初始化CH579程序库,操作成功返回0 */
{
	if ( CH579GetVer( ) < CH579_LIB_VER ) return( 0xFF );  /* 获取当前子程序库的版本号,版本太低则返回错误 */
#if		DISK_BASE_BUF_LEN > 0
	pDISK_BASE_BUF = & DISK_BASE_BUF[0];  /* 指向外部RAM的磁盘数据缓冲区 */
	pDISK_FAT_BUF = & DISK_BASE_BUF[0];  /* 指向外部RAM的磁盘FAT数据缓冲区,可以与pDISK_BASE_BUF合用以节约RAM */
//	pDISK_FAT_BUF = & DISK_FAT_BUF[0];  /* 指向外部RAM的磁盘FAT数据缓冲区,独立于pDISK_BASE_BUF以提高速度 */
/* 如果希望提高文件存取速度,那么可以在主程序中调用CH579LibInit之后,将pDISK_FAT_BUF重新指向另一个独立分配的与pDISK_BASE_BUF同样大小的缓冲区 */
#endif
	CH579DiskStatus = DISK_UNKNOWN;  /* 未知状态 */
	CH579vSectorSizeB = 9;  /* 默认的物理磁盘的扇区是512B */
	CH579vSectorSize = 512;  // 默认的物理磁盘的扇区是512B,该值是磁盘的扇区大小
	CH579vStartLba = 0;  /* 默认为自动分析FDD和HDD */
	return( ERR_SUCCESS );
}
