/********************************** (C) COPYRIGHT *******************************
* File Name          : als_vendor_model.c
* Author             : WCH
* Version            : V1.0
* Date               : 2019/12/15
* Description 
*******************************************************************************/

#include "mesh_error.h"
#include "CH57x_common.h"

#define BT_DBG_ENABLED 1

#include "cfg_srv.h"
#include "health_srv.h"
#include "mesh_trace.h"
#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_main.h"
#include "mesh_cfg_pub.h"
#include "tm_genie.h"
#include "mesh_slist.h"
#include "common_trace.h"
#include "HAL.h"
#include "als_vendor_model.h"
#include "als_main.h"
#include "sha256.h"
#include "constants.h"

static sys_slist_t list;

//pid: 0x25e5
#if defined(LIGHT_1)
/* Mac Address Light1 */
#define PID				9701
#define MAC_ADDR 		{0xf8,0xa7,0x63,0x6a,0xec,0x3e}
#define ALI_SECRET		{0x60,0x36,0xd9,0x18,0x88,0x94,0xdd,0x0e,0x6c,0x09,0x59,0xd3,0x45,0x3e,0x4f,0xab}

#elif defined(LIGHT_2)
/* Mac Address Light2 */
#define PID				9701
#define MAC_ADDR  		{0xf8,0xa7,0x63,0x6a,0xec,0x3f}
#define ALI_SECRET		{0x6b,0xfa,0x68,0x6f,0x9d,0x1b,0x37,0x00,0x01,0xd1,0xfd,0xb8,0x27,0x7d,0xc0,0x81}

#elif defined(LIGHT_3)
/* Mac Address Light3 */
#define PID				9701
#define MAC_ADDR 		{0xf8,0xa7,0x63,0x6a,0xec,0x40}
#define ALI_SECRET		{0xba,0x27,0xb4,0xf8,0xab,0xfb,0xe9,0x0b,0x7b,0x1e,0x6e,0xa2,0x7e,0xce,0xde,0xcd}

#elif defined(LIGHT_4)
/* Mac Address Light4 */
#define PID				9701
#define MAC_ADDR  		{0xf8,0xa7,0x63,0x6a,0xec,0x41}
#define ALI_SECRET		{0x4e,0xb1,0x5f,0xc2,0x74,0xc4,0x0a,0x46,0xdc,0x20,0x3c,0xdb,0x0a,0xb4,0x8f,0xa4}

#elif defined(LIGHT_5)
/* Mac Address Light5 */
#define PID				9701
#define MAC_ADDR  		{0xf8,0xa7,0x63,0x6a,0xec,0x42}
#define ALI_SECRET		{0xbc,0xbe,0x8d,0x4c,0x36,0x04,0x5f,0x5c,0x20,0xec,0xce,0x73,0x60,0x5e,0x43,0xa9}

#elif defined(LIGHT_6)
/* Mac Address Light6 */
#define PID				9701
#define MAC_ADDR 		{0xf8,0xa7,0x63,0x6a,0xec,0x43}
#define ALI_SECRET		{0x9b,0x71,0xf3,0xd0,0x9e,0x70,0xcd,0x05,0x4a,0xf1,0x16,0x4f,0x1c,0xa6,0x8b,0xe0}

#elif defined(LPN_1)	/* LPN Only */
#define PID				Undef
#define MAC_ADDR 		{0xf8,0xa7,0x63,0x85,0xb9,0x0b}
#define ALI_SECRET		{0x2b,0x33,0x5e,0x73,0x14,0x3f,0xf5,0x4b,0xb4,0x2c,0xc5,0xc1,0xd2,0xff,0x84,0xe7}

#elif defined(PLAN_B_0)
#define PID				11418
#define MAC_ADDR 		{0x28,0xfa,0x7a,0x03,0x3c,0x05}
#define ALI_SECRET		{0xa6,0xce,0x5d,0xa4,0xe3,0x22,0x68,0x50,0xb2,0xb9,0xc2,0xb0,0xa3,0xf3,0x8d,0xf7}

#elif defined(PLAN_B_1)
#define PID				11418
#define MAC_ADDR 		{0x28,0xfa,0x7a,0x03,0x3c,0x06}
#define ALI_SECRET		{0x8c,0x16,0x3f,0x87,0x55,0xf4,0x9a,0xb4,0x84,0x98,0xd3,0xfd,0x40,0x55,0x25,0xce}
#endif

u8_t tm_uuid[16];
u8_t static_key[16];
u8C MacAddr[6] = MAC_ADDR;

/* TODO Fixed Address in flash ? */
static const struct bt_als_cfg cfg = 
{
	.mac = MAC_ADDR,
	.secret = ALI_SECRET,
	.pid = PID,
	.cid = 0x01a8,
	.version = 0x00060000,
};

typedef struct bt_mesh_indicate *(*bt_ind_alloc_t)(int id);

static void ind_destory(struct net_buf *buf)
{
	BT_DBG("");
}

NET_BUF_POOL_HEAP_DEFINE_UN(ind_pool, CONFIG_INDICATE_NUM, ind_destory);

static struct bt_mesh_indicate ind_cb_pool[CONFIG_INDICATE_NUM];

static struct bt_mesh_indicate *ind_alloc(int id)
{
    return &ind_cb_pool[id];
}

static inline void ind_send_start(int err, const struct bt_adv_ind_send_cb *cb,
                                  void *cb_data)
{
    if (cb && cb->start)
    {
        cb->start(err, cb_data);
    }
}

static inline void ind_send_end(int err, const struct bt_adv_ind_send_cb *cb,
                                void *cb_data)
{
    if (cb && cb->end)
    {
        cb->end(err, cb_data);
    }
}

static struct net_buf *bt_mesh_ind_create_from_pool(struct net_buf_pool *pool, u16_t len,
                                        bt_ind_alloc_t get_id, s32_t timeout)
{
    struct bt_mesh_indicate *ind;
    struct net_buf *buf;

    buf = net_buf_alloc_len(pool, len, timeout);
    if (!buf)
    {
        return NULL;
    }

    ind = get_id(net_buf_id(buf));
    BLE_IND(buf) = ind;

    (void)memset(ind, 0, offsetof(struct bt_mesh_indicate, timer));

    return buf;
}

struct net_buf *bt_mesh_ind_create(u16_t len, s32_t timeout)
{
    return bt_mesh_ind_create_from_pool(&ind_pool, len, ind_alloc, timeout);
}

void bt_mesh_indicate_send(struct net_buf *buf, struct indicate_param *param)
{
	struct bt_mesh_indicate *ind;

	ind = BLE_IND(buf);
	ind->buf = net_buf_ref(buf);

	memcpy(&ind->param, param, sizeof(struct indicate_param));

	sys_slist_append(&list, &ind->node);

	k_delayed_work_submit(&ind->timer, ind->param.rand);
}

static void tm_attr_get(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    BT_DBG("%s", hex_dump(buf->data, buf->len));
}

static void tm_attr_set(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    BT_DBG("%s", hex_dump(buf->data, buf->len));
}

static void tm_attr_set_unack(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    BT_DBG("%s", hex_dump(buf->data, buf->len));
}

static void tm_attr_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    BT_DBG("%s", hex_dump(buf->data, buf->len));
}

static void ind_reset(struct bt_mesh_indicate *ind, int err)
{
	sys_slist_find_and_remove(&list, &ind->node);

	ind_send_end(err, ind->param.cb, ind->param.cb_data);

	net_buf_unref(ind->buf);
	ind->buf = NULL;
}

static void tm_attr_confirm(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
	u8_t recv_tid;
	sys_snode_t *node, *snode;
	struct bt_mesh_indicate *ind;
	
	recv_tid = net_buf_simple_pull_u8(buf);
	
	BT_DBG("src: 0x%04x dst: 0x%04x tid 0x%02x rssi: %d", 
					ctx->addr, ctx->recv_dst, recv_tid, ctx->recv_rssi);

	SYS_SLIST_FOR_EACH_NODE_SAFE(&list, node, snode)
	{
		ind = CONTAINER_OF(node, struct bt_mesh_indicate, node);
		if (ind->param.tid == recv_tid)
		{
			ind_reset(ind, 0);
			k_delayed_work_cancel(&ind->timer);

			continue;
		}
	}
}

static void tm_attr_trans(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    BT_DBG("%s", hex_dump(buf->data, buf->len));
}

static const struct bt_mesh_model_op vnd_model_op[] = {
    {OP_VENDOR_MESSAGE_ATTR_GET, 				0, tm_attr_get},
	{OP_VENDOR_MESSAGE_ATTR_SET, 				0, tm_attr_set},
	{OP_VENDOR_MESSAGE_ATTR_SET_UNACK, 			0, tm_attr_set_unack},
	{OP_VENDOR_MESSAGE_ATTR_STATUS, 			0, tm_attr_status},
	{OP_VENDOR_MESSAGE_ATTR_CONFIRMATION, 		1, tm_attr_confirm},
	{OP_VENDOR_MESSAGE_ATTR_TRANSPARENT_MSG, 	0, tm_attr_trans},
    BLE_MESH_MODEL_OP_END,
};

struct bt_mesh_model vnd_models[] = {
    BLE_MESH_MODEL_VND(CID_ALI_GENIE, 0x0000, vnd_model_op, NULL, NULL),
};

static void ind_start(u16_t duration, int err, void *cb_data)
{
	struct bt_mesh_indicate *ind = cb_data;

	if (ind->buf == NULL)
	{
		return;
	}

	if (err)
	{
		BT_ERR("Unable send indicate (err:%d)", err);
		k_delayed_work_submit(&ind->timer, K_MSEC(100));
		return;
	}
}

static void ind_end(int err, void *cb_data)
{
	struct bt_mesh_indicate *ind = cb_data;

	if (ind->buf == NULL)
	{
		return;
	}

	k_delayed_work_submit(&ind->timer, ind->param.period);
}

const struct bt_mesh_send_cb ind_cb = 
{
	.start = ind_start,
	.end = ind_end,
};

static void adv_ind_send(struct k_work *work)
{
	BT_DBG("");

	int err;
	struct bt_mesh_indicate *ind;

	NET_BUF_SIMPLE_DEFINE(msg, 32);

	struct bt_mesh_msg_ctx ctx = {
		.app_idx = vnd_models[0].keys[0],
		.addr = 0xf000,
	};

	ind = CONTAINER_OF(work, struct bt_mesh_indicate, timer);
	
	if (ind->buf == NULL)
	{
		return;
	}

	if (ind->param.trans_cnt == 0)
	{
		ind_reset(ind, -ETIMEDOUT);
		return;
	}

	ind->param.trans_cnt --;

	ctx.send_ttl = ind->param.send_ttl;

	/** TODO */
	net_buf_simple_add_mem(&msg, ind->buf->data, ind->buf->len);

	err = bt_mesh_model_send(vnd_models, &ctx, &msg, &ind_cb, ind);
	if (err)
	{
		BT_ERR("Unable send model message (err:%d)", err);

		ind_reset(ind, -EIO);
		return;
	}
}

static u8_t tid;

/** TODO TID selection method */
u8_t als_avail_tid_get(void)
{
	return tid++;
}

static void uuid_generate(struct bt_als_cfg const *cfg)
{
	/* Company ID */
	sys_put_le16(cfg->cid, tm_uuid);
	
	/* Advertising Verison */
	tm_uuid[2] = BIT(0) |	/* adv version */
				 BIT(4) |	/* secret */
				 BIT(5) |	/* ota */
				 BIT(7); 	/* ble verison */
	
	/* Product Id */
	sys_put_le32(cfg->pid, &tm_uuid[2 + 1]);
	
	/* Device Mac Address */
	for(int i=0;i<6;i++) tm_uuid[2 + 1 + 4 + i] = cfg->mac[5-i];
	
	/* UUID Verison */
	tm_uuid[2 + 1 + 4 + 6] = BIT(1);
	
	/* RFU */
	sys_put_le16(0x00, &tm_uuid[2 + 1 + 4 + 6 + 1]);
	
	BT_INFO("uuid %s", hex_dump(tm_uuid, 16));
}

static void num_to_str(u8_t *out, const u8_t *in, u16_t in_len)
{
	u16_t i;
	static const char hex[] = "0123456789abcdef";

	for (i = 0; i < in_len; i++)
	{
        out[i * 2]     = hex[in[i] >> 4];
        out[i * 2 + 1] = hex[in[i] & 0xf];
	}
}

static void oob_key_generate(struct bt_als_cfg const *cfg)
{
	int err;
	u32_t pid;
	u8_t out[8 + 1 + 12 + 1 + 32], dig[32];
	struct tc_sha256_state_struct s;

    tc_sha256_init(&s);
			
	/** pid value */
	pid = sys_cpu_to_be32(cfg->pid);
	num_to_str(out, (void *)&pid, 4);

	/** Separator */
    strcpy((void *)(out + 8), ",");
	/** mac value */
	num_to_str(out + 8 + 1, (void *)cfg->mac, 6);

	/** Separator */
    strcpy((void *)(out + 8 + 1 + 12), ",");
	/** secret value */
	num_to_str(out + 8 + 1 + 12 + 1, (void *)cfg->secret, 16);

    err = tc_sha256_update(&s, out, sizeof(out));
    if (err != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("Unable Update Sha256");
        return;
    }

    err = tc_sha256_final(dig, &s);
    if (err != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("Unable Generate sha256 value");
        return;
    }

    BT_INFO("key %s", hex_dump(dig, 16));
	memcpy(static_key, dig, 16);
}

void als_vendor_init(void)
{
	u32_t ran;
	
	uuid_generate(&cfg);
	oob_key_generate(&cfg);

	/** Random Local TID Value
     *  @Caution Don't use single octer only.
	*/
	ran = tmos_rand();
	tid += ((u8_t *)&ran)[0];
	tid += ((u8_t *)&ran)[1];
	tid += ((u8_t *)&ran)[2];
	tid += ((u8_t *)&ran)[3];
	
	sys_slist_init(&list);
	
	for(int i = 0; i < CONFIG_INDICATE_NUM; i++)
	{
		k_delayed_work_init(&ind_cb_pool[i].timer, adv_ind_send);
	}
}
