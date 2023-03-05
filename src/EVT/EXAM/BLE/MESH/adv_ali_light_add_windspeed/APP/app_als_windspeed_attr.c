/********************************** (C) COPYRIGHT *******************************
 * File Name          : app_als_windspeed_attr.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/03/24
 * Description        :
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
#include "CONFIG.h"
#include "MESH_LIB.h"
#include "app_mesh_config.h"
#include "app_als_windspeed_attr.h"
#include "app_vendor_model.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */

#define ALI_DEF_TTL    (10)

// ģ��windspeedֵ
uint8_t device_windspeed = 1;

/*******************************************************************************
 * Function Name  : read_windspeed
 * Description    : ��ȡ��ǰwindspeed
 * Input          : None
 * Return         : None
 *******************************************************************************/
uint8_t read_windspeed(void)
{
    APP_DBG("device_windspeed: %d ", device_windspeed);
    return device_windspeed;
}

/*******************************************************************************
 * Function Name  : set_windspeeds
 * Description    : ���õ�ǰwindspeed
 * Input          : None
 * Return         : None
 *******************************************************************************/
void set_windspeed(uint8_t windspeed)
{
    device_windspeed = windspeed;
}

/*******************************************************************************
 * Function Name  : gen_windspeed_status
 * Description    : �ظ���è����windspeed
 * Input          : model: ģ�Ͳ���
 *										ctx�����ݲ���
 * Return         : None
 *******************************************************************************/
static void gen_windspeed_status(struct bt_mesh_model   *model,
                                 struct bt_mesh_msg_ctx *ctx)
{
    NET_BUF_SIMPLE_DEFINE(msg, 32);
    int err;

    ////////////////////////////////////////////////////////////////////////
    //		0xD3	0xA8	0x01	|	0x##	|	0x0a	0x01			|	0x##	0x##				//
    //		Opcode						|	TID		|	Attribute Type	|	Attribute Value		//
    ////////////////////////////////////////////////////////////////////////
    bt_mesh_model_msg_init(&msg, OP_VENDOR_MESSAGE_ATTR_STATUS);
    net_buf_simple_add_u8(&msg, als_avail_tid_get());
    net_buf_simple_add_le16(&msg, ALI_GEN_ATTR_TYPE_WINDSPEED);
    net_buf_simple_add_le16(&msg, read_windspeed());

    APP_DBG("ttl: 0x%02x dst: 0x%04x", ctx->recv_ttl, ctx->recv_dst);

    if(ctx->recv_ttl != ALI_DEF_TTL)
    {
        ctx->send_ttl = BLE_MESH_TTL_DEFAULT;
    }
    else
    {
        ctx->send_ttl = 0;
    }

    err = bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
    if(err)
    {
        APP_DBG("send status failed: %d", err);
    }
}

/*******************************************************************************
 * Function Name  : gen_windspeed_get
 * Description    : ��è�����·��Ļ�ȡwindspeed����
 * Input          : model: ģ�Ͳ���
 *										ctx�����ݲ���
 *										buf: ��������
 * Return         : None
 *******************************************************************************/
void gen_windspeed_get(struct bt_mesh_model   *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple  *buf)
{
    APP_DBG(" ");
    gen_windspeed_status(model, ctx);
}

/*******************************************************************************
* Function Name  : gen_windspeed_set
* Description    : ��è�����·�������windspeed����
                                        ����뵱ǰwindspeed��ͬ,����Ҫ����ind����è
* Input          : model: ģ�Ͳ���
*										ctx�����ݲ���
*										buf: ��������
* Return         : None
*******************************************************************************/
void gen_windspeed_set(struct bt_mesh_model   *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple  *buf)
{
    struct indicate_param param = {
        .trans_cnt = 0x09,
        .period = K_MSEC(300),
        .rand = (tmos_rand() % 50),
        .tid = als_avail_tid_get(),
    };

    APP_DBG("ttl: 0x%02x dst: 0x%04x rssi: %d ",
            ctx->recv_ttl, ctx->recv_dst, ctx->recv_rssi);

    if((buf->data[1] | (buf->data[2] << 8)) < ALI_GEN_ATTR_TYPE_WORK_STATUS)
    {
        // ����Ϊ�趨ֵ
        set_windspeed(buf->data[3]);
    }
    else
    {
        switch(buf->data[1] | (buf->data[2] << 8))
        {
            // ����Ϊ�趨�仯��
            case ALI_GEN_ATTR_TYPE_DELTA_VALUE:
            {
                char delta = (char)buf->data[5];
                set_windspeed(read_windspeed() + delta);
            }
        }
    }

    if(ctx->recv_ttl != ALI_DEF_TTL)
    {
        param.send_ttl = BLE_MESH_TTL_DEFAULT;
    }

    /* Overwrite default configuration */
    if(BLE_MESH_ADDR_IS_UNICAST(ctx->recv_dst))
    {
        param.rand = 0;
        param.send_ttl = BLE_MESH_TTL_DEFAULT;
        param.period = K_MSEC(100);
    }

    send_windspeed_indicate(&param);

    gen_windspeed_status(model, ctx);
}

/*******************************************************************************
 * Function Name  : gen_windspeed_set_unack
 * Description    : ��è�����·�������windspeed����(��Ӧ��)
 * Input          : model: ģ�Ͳ���
 *										ctx�����ݲ���
 *										buf: ��������
 * Return         : None
 *******************************************************************************/
void gen_windspeed_set_unack(struct bt_mesh_model   *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple  *buf)
{
    APP_DBG(" ");

    if((buf->data[1] | (buf->data[2] << 8)) < ALI_GEN_ATTR_TYPE_WORK_STATUS)
    {
        // ����Ϊ�趨ֵ
        set_windspeed(buf->data[3]);
    }
    else
    {
        switch(buf->data[1] | (buf->data[2] << 8))
        {
            // ����Ϊ�趨�仯��
            case ALI_GEN_ATTR_TYPE_DELTA_VALUE:
            {
                char delta = (char)buf->data[5];
                set_windspeed(read_windspeed() + delta);
            }
        }
    }
}

/******************************** endfile @ main ******************************/
