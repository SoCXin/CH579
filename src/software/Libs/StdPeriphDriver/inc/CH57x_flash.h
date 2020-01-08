


#ifndef __CH57x_FLASH_H__
#define __CH57x_FLASH_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "CH579SFR.h"
#include "core_cm0.h"
     
#define ROM_UUID_ADDR        0x40608         // chip UID address ( ID-48bit + CKS-16bit )
#define ROM_MAC_ADDR         0x40608         // NET MAC address 48bit

// 获取唯一ID
void GetUniqueID(PUINT8 buf);                                           /* 获取芯片唯一ID，小端模式，6B-ID， 2B-CKS */    
     
// 获取网络MAC
void GetMACAddress(PUINT8 buf);                                         /* 获取网络MAC，小端模式，6B-MAC */    
	 

void EnableCodeFlash(void);                                             /* 解锁 CodeFlash 区域擦写权限 */
void EnableDataFlash(void);                                             /* 解锁 DataFlash 区域擦写权限 */
void LockFlashALL(void);                                                /* 锁定 CodeFlash/DataFlash 全部区域 */
UINT8 Flash_BlockErase(UINT32 addr);                                    /* 块擦除，一次擦除512B。使用时，需要和 EnableFlashCode()/EnableDataFlash() + LockFlashALL() 配合使用 */
UINT8 Flash_WriteDW(UINT32 addr, UINT32 dat);	                        /* 双字写，地址需4字节对齐。使用时，需要和 EnableFlashCode()/EnableDataFlash() + LockFlashALL() 配合使用 */
	 
	 
#ifdef __cplusplus
}
#endif

#endif  // __CH57x_FLASH_H__	

