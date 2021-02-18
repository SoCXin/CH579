#ifndef __ALS_VENDOR_MOD_H
#define __ALS_VENDOR_MOD_H

#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_buf.h"
#include "mesh_kernel.h"
#include "mesh_slist.h"
#include "als_main.h"

/** Default number of Indication */
#define CONFIG_INDICATE_NUM (3)

struct bt_adv_ind_send_cb
{
    void (*start)(int err, void *cb_data);
    void (*end)(int err, void *cb_data);
};

struct indicate_param
{
	u8_t tid;
	u8_t trans_cnt;
	s32_t period;
	s32_t rand;
	u8_t send_ttl;

	void *cb_data;
    const struct bt_adv_ind_send_cb *cb;
};

struct bt_mesh_indicate
{
	struct indicate_param param;

	sys_snode_t node;
	struct net_buf *buf;
	struct k_delayed_work timer;
};

extern struct bt_mesh_model vnd_models[1];
extern u8_t static_key[16];
extern u8_t tm_uuid[16];

struct net_buf *bt_mesh_ind_create(u16_t len, s32_t timeout);
void bt_mesh_indicate_send(struct net_buf *buf, struct indicate_param *param);

u8_t als_avail_tid_get(void);
void als_vendor_init(void);

#define BLE_IND(buf) (*(struct bt_mesh_indicate **)net_buf_user_data(buf))

#endif
