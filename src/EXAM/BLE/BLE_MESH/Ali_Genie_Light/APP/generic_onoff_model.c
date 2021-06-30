/********************************** (C) COPYRIGHT *******************************
* File Name          : generic_onoff_model.c
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
#include "als_vendor_model.h"
#include "common_trace.h"
#include "HAL.h"

void send_led_indicate(struct indicate_param *param)
{
	struct net_buf *buf;
	
	buf = bt_mesh_ind_create(16, K_NO_WAIT);
	if (!buf)
	{
		BT_ERR("Unable allocate buffers");
		return;
	}
	
	/* Init indication opcode */
	bt_mesh_model_msg_init(&buf->b, OP_VENDOR_MESSAGE_ATTR_INDICATION);
    
	/* Add tid field */
	net_buf_simple_add_u8(&buf->b, param->tid);
	
	/* Add generic onoff attrbute op */
	net_buf_simple_add_le16(&buf->b, 0x0100);
	
	/* Add current generic onoff status */
	net_buf_simple_add_u8(&buf->b, read_led_state(MSG_PIN));
	
	bt_mesh_indicate_send(buf, param);
	
	net_buf_unref(buf);
}

#define ALI_DEF_TTL	(10)

static void gen_onoff_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx)
{
    NET_BUF_SIMPLE_DEFINE(msg, 32);
    int err;

    BT_DBG("#mesh-onoff STATUS");

    bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_2(0x82, 0x04));
    net_buf_simple_add_u8(&msg, read_led_state(MSG_PIN));
	
	BT_DBG("ttl: 0x%02x dst: 0x%04x", ctx->recv_ttl, ctx->recv_dst);

	if (ctx->recv_ttl != ALI_DEF_TTL)
	{
		ctx->send_ttl = BLE_MESH_TTL_DEFAULT;
	}
	else
	{
		ctx->send_ttl = 0;
	}

    err = bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
    if (err)
    {
        BT_ERR("#mesh-onoff STATUS: send status failed: %d", err);
    }
}

//static void gen_onoff_unack(struct bt_mesh_model *model,
//								struct bt_mesh_msg_ctx *ctx)
//{
//    NET_BUF_SIMPLE_DEFINE(msg, 32);
//    int err;

//    BT_DBG("");

//    bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_2(0x82, 0x03));
//	
//	net_buf_simple_add_u8(&msg, 0x00);
//    net_buf_simple_add_u8(&msg, read_led_state(MSG_PIN));

//	/** Force generic onoff status not relayed by other node */
//	ctx->send_ttl = 0;

//    err = bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
//    if (err)
//    {
//        BT_ERR("#mesh-onoff UNACK: send status failed: %d", err);
//    }
//}

static void gen_onoff_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    BT_DBG("#mesh-onoff GET");

    gen_onoff_status(model, ctx);
}

static void gen_onoff_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
	u8_t status;

	BT_DBG("#mesh-onoff SET-UNACK");
	
	status = read_led_state(MSG_PIN);
	if (status != buf->data[1])
	{
		set_led_state(MSG_PIN, buf->data[1]);
	}
}

static void gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
	u8_t status;
	struct indicate_param param = {
		.trans_cnt = 0x09,
		.period = K_MSEC(300),
		.rand = (tmos_rand() % 50),
		.tid = als_avail_tid_get(),
	};
	
    BT_DBG("ttl: 0x%02x dst: 0x%04x rssi: %d", 
			ctx->recv_ttl, ctx->recv_dst, ctx->recv_rssi);

	status = read_led_state(MSG_PIN);
	if (status != buf->data[0])
	{
		set_led_state(MSG_PIN, buf->data[0]);

		if (ctx->recv_ttl != ALI_DEF_TTL)
		{
			param.send_ttl = BLE_MESH_TTL_DEFAULT;
		}

		/* Overwrite default configuration */
		if (BLE_MESH_ADDR_IS_UNICAST(ctx->recv_dst))
		{
			param.rand = 0;
			param.send_ttl = BLE_MESH_TTL_DEFAULT;
			param.period = K_MSEC(100);
		}

		send_led_indicate(&param);
	}

	gen_onoff_status(model, ctx);
}

const struct bt_mesh_model_op gen_onoff_op[] = {
    {BLE_MESH_MODEL_OP_2(0x82, 0x01), 0, gen_onoff_get},
    {BLE_MESH_MODEL_OP_2(0x82, 0x02), 2, gen_onoff_set},
    {BLE_MESH_MODEL_OP_2(0x82, 0x03), 2, gen_onoff_set_unack},
    BLE_MESH_MODEL_OP_END,
};
