#ifndef __ALS_GENERIC_MOD_H
#define __ALS_GENERIC_MOD_H

#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_buf.h"
#include "mesh_kernel.h"
#include "als_vendor_model.h"

extern const struct bt_mesh_model_op gen_onoff_op[];

void send_led_indicate(struct indicate_param *param);

#endif
