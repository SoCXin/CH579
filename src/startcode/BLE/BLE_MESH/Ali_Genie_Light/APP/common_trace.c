/********************************** (C) COPYRIGHT *******************************
* File Name          : common_trace.c
* Author             : WCH
* Version            : V1.0
* Date               : 2019/12/15
* Description 
*******************************************************************************/

#include "mesh_error.h"
#include "CH57x_common.h"

#define BT_DBG_ENABLED 1
#define CONFIG_BLE_MESH_STACK_TRACE_LEVEL LOG_LEVEL_DEBUG

#include "cfg_srv.h"
#include "health_srv.h"
#include "mesh_trace.h"
#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_main.h"
#include "mesh_cfg_pub.h"
#include "tm_genie.h"
#include "mesh_slist.h"
#include "HAL.h"
#include "common_trace.h"

bool read_led_state(u32_t led_pin)
{
	return !GPIOB_ReadPortPin(led_pin);
}

void set_led_state(u32_t led_pin, bool on)
{
	on ? GPIOB_ResetBits(led_pin) : GPIOB_SetBits(led_pin);
}

void toggle_led_state(u32_t led_pin)
{
	GPIOB_InverseBits(led_pin);
}

const char *hex_dump(const void *buf, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    static char hexbufs[150];
    const u8_t *b = buf;
    char *str;
    int i;

    str = hexbufs;

    len = MIN(len, (sizeof(hexbufs) - 1) / 2);

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
}
