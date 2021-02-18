
#ifndef _BLE_MESH_TYPES_H_
#define _BLE_MESH_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef CH57xBLE_ROM
#include "CH57xBLE_ROM.H"
#else
#include "CH57xBLE_LIB.H"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char         s8_t;
typedef signed short        s16_t;
typedef int          		s32_t;
typedef signed long long    s64_t;

typedef unsigned char       u8_t;
typedef unsigned short      u16_t;
typedef unsigned int        u32_t;
typedef unsigned long long  u64_t;

typedef unsigned int        bt_mesh_atomic_t;
typedef int 				off_t;

typedef signed int 			ssize_t;

#ifndef bool
#define bool        int8_t
#endif

#ifndef false
#define false       0
#endif

#ifndef true
#define true        1
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_TYPES_H_ */
