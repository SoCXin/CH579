#ifndef __ALS_COMMON_TRACE_H
#define __ALS_COMMON_TRACE_H

#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_buf.h"
#include "mesh_kernel.h"

#define PROV_PIN		GPIO_Pin_15
#define MSG_PIN			GPIO_Pin_14

bool read_led_state(u32_t led_pin);

void set_led_state(u32_t led_pin, bool on);

void toggle_led_state(u32_t led_pin);

const char *hex_dump(const void *buf, size_t len);

#endif
