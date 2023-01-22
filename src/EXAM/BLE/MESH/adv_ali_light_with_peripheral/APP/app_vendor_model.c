/********************************** (C) COPYRIGHT *******************************
 * File Name          : app_vendor_model.c
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
#include "app_mesh_config.h"
#include "MESH_LIB.h"
#include "CONFIG.h"
#include "app_vendor_model.h"
#include "app_generic_onoff_model.h"
#include "app_generic_lightness_model.h"
#include "app_generic_color_model.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
        ��Ԫ���ַ��Ϣ                     */
/* Mac Address Light3   */
#define PID    10135680
#define MAC_ADDR                       \
  {                                    \
    0x50, 0x3d, 0xeb, 0x90, 0x35, 0x17 \
  }
#define ALI_SECRET                                                                                 \
  {                                                                                                \
    0xcb, 0xd1, 0xc1, 0x33, 0xa8, 0x28, 0xc7, 0xdc, 0x7e, 0xbd, 0xef, 0x6f, 0xbc, 0x41, 0x55, 0x0e \
  }

/*********************************************************************
 * GLOBAL TYPEDEFS
 */

uint8_t       tm_uuid[16];
uint8_t       static_key[16];
const uint8_t MacAddr[6] = MAC_ADDR;

static const struct bt_als_cfg cfg = {
    .mac = MAC_ADDR,
    .secret = ALI_SECRET,
    .pid = PID,
    .cid = 0x01a8,
    .version = 0x00060000,
};

static uint8_t als_vendor_model_TaskID = 0; // Task ID for internal task/event processing
static uint8_t als_tid = 0;

static struct net_buf          ind_buf[CONFIG_INDICATE_NUM] = {0};
static struct bt_mesh_indicate indicate[CONFIG_INDICATE_NUM] = {0};

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
static void     ind_reset(struct bt_mesh_indicate *ind, int err);
static uint16_t als_vendor_model_ProcessEvent(uint8_t task_id, uint16_t events);

/*********************************************************************
 * @fn      tm_attr_get
 *
 * @brief   ��è��ȡ��������
 *
 * @param   model   - ģ�Ͳ���.
 * @param   ctx     - ���ݲ���.
 * @param   buf     - ��������.
 *
 * @return  none
 */
static void tm_attr_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    APP_DBG(" ");
}

/*********************************************************************
 * @fn      tm_attr_set
 *
 * @brief   ��è������������
 *
 * @param   model   - ģ�Ͳ���.
 * @param   ctx     - ���ݲ���.
 * @param   buf     - ��������.
 *
 * @return  none
 */
static void tm_attr_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    APP_DBG(" ");
}

/*********************************************************************
 * @fn      tm_attr_set_unack
 *
 * @brief   ��è�������������Ӧ��
 *
 * @param   model   - ģ�Ͳ���.
 * @param   ctx     - ���ݲ���.
 * @param   buf     - ��������.
 *
 * @return  none
 */
static void tm_attr_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    APP_DBG(" ");
}

/*********************************************************************
 * @fn      tm_attr_status
 *
 * @brief   ��è״̬Ӧ��
 *
 * @param   model   - ģ�Ͳ���.
 * @param   ctx     - ���ݲ���.
 * @param   buf     - ��������.
 *
 * @return  none
 */
static void tm_attr_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    APP_DBG(" ");
}

/*********************************************************************
 * @fn      tm_attr_confirm
 *
 * @brief   �յ���è���鷢����confirm - ����Ϣ����Vendor Model Client�ظ���Vendor Model Server��
                                 ���ڱ�ʾ���յ�Vendor Model Server������Indication
 *
 * @param   model   - ģ�Ͳ���.
 * @param   ctx     - ���ݲ���.
 * @param   buf     - ��������.
 *
 * @return  none
 */
static void tm_attr_confirm(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    uint8_t recv_tid;

    recv_tid = net_buf_simple_pull_u8(buf);

    APP_DBG("src: 0x%04x dst: 0x%04x tid 0x%02x rssi: %d", ctx->addr, ctx->recv_dst, recv_tid, ctx->recv_rssi);

    for(int i = 0; i < CONFIG_INDICATE_NUM; i++)
    {
        if(indicate[i].param.tid == recv_tid)
        {
            ind_reset(&indicate[i], 0);
            tmos_stop_task(als_vendor_model_TaskID, indicate[i].event);
            continue;
        }
    }
}

/*********************************************************************
 * @fn      tm_attr_trans
 *
 * @brief   ����Ϣ����Mesh�豸����è����֮��͸������
 *
 * @param   model   - ģ�Ͳ���.
 * @param   ctx     - ���ݲ���.
 * @param   buf     - ��������.
 *
 * @return  none
 */
static void tm_attr_trans(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    APP_DBG(" ");
}

// opcode ��Ӧ�Ĵ�����
static const struct bt_mesh_model_op vnd_model_op[] = {
    {OP_VENDOR_MESSAGE_ATTR_GET, 0, tm_attr_get},
    {OP_VENDOR_MESSAGE_ATTR_SET, 0, tm_attr_set},
    {OP_VENDOR_MESSAGE_ATTR_SET_UNACK, 0, tm_attr_set_unack},
    {OP_VENDOR_MESSAGE_ATTR_STATUS, 0, tm_attr_status},
    {OP_VENDOR_MESSAGE_ATTR_CONFIRMATION, 1, tm_attr_confirm},
    {OP_VENDOR_MESSAGE_ATTR_TRANSPARENT_MSG, 0, tm_attr_trans},
    BLE_MESH_MODEL_OP_END,
};

uint16_t vnd_model_srv_keys[CONFIG_MESH_MOD_KEY_COUNT_DEF] = {BLE_MESH_KEY_UNUSED};
uint16_t vnd_model_srv_groups[CONFIG_MESH_MOD_GROUP_COUNT_DEF] = {BLE_MESH_ADDR_UNASSIGNED};

struct bt_mesh_model vnd_models[] = {
    BLE_MESH_MODEL_VND_CB(CID_ALI_GENIE, 0x0000, vnd_model_op, NULL, vnd_model_srv_keys, vnd_model_srv_groups, NULL,
        NULL),
};

/*********************************************************************
 * @fn      als_avail_tid_get
 *
 * @brief   TID selection method
 *
 * @param   none
 *
 * @return  next TID
 */
uint8_t als_avail_tid_get(void)
{
    return als_tid++;
}

/*********************************************************************
 * @fn      uuid_generate
 *
 * @brief   ���ɰ���淶��UUID
 *
 * @param   cfg - ������Ϣ
 *
 * @return  none
 */
static void uuid_generate(struct bt_als_cfg const *cfg)
{
    /* Company ID */
    tm_uuid[0] = cfg->cid;
    tm_uuid[1] = cfg->cid >> 8;

    /*	bit3��0 �������㲥���汾�ţ�Ŀǰ��0x01
     bit4Ϊ1��һ��һ��
     bit5Ϊ1��֧��OTA
     bit7��6������Э��汾
     00��BLE4.0
     01��BLE4.2
     10��BLE5.0
     11��BLE5.0���� */
    /* Advertising Verison */
    tm_uuid[2] = BIT(0) | /* adv version */
                 BIT(4) | /* secret */
                 BIT(5) | /* ota */
                 BIT(7);  /* ble verison */

    /* Product Id */
    tm_uuid[2 + 1] = cfg->pid;
    tm_uuid[2 + 2] = cfg->pid >> 8;
    tm_uuid[2 + 3] = cfg->pid >> 16;
    tm_uuid[2 + 4] = cfg->pid >> 24;

    /* Device Mac Address */
    for(int i = 0; i < 6; i++)
    {
        tm_uuid[2 + 1 + 4 + i] = cfg->mac[5 - i];
    }

    /* UUID Verison */
    tm_uuid[2 + 1 + 4 + 6] = BIT(1);

    /* RFU */
    tm_uuid[2 + 1 + 4 + 6 + 1] = 0x00;
    tm_uuid[2 + 1 + 4 + 6 + 2] = 0x00;
}

/*********************************************************************
 * @fn      num_to_str
 *
 * @brief   ����ת�ַ�
 *
 * @param   out     - ���
 * @param   in      - ����
 * @param   in_len  - ����
 *
 * @return  none
 */
static void num_to_str(uint8_t *out, const uint8_t *in, uint16_t in_len)
{
    uint16_t          i;
    static const char hex[] = "0123456789abcdef";

    for(i = 0; i < in_len; i++)
    {
        out[i * 2] = hex[in[i] >> 4];
        out[i * 2 + 1] = hex[in[i] & 0xf];
    }
}

/*********************************************************************
 * @fn      oob_key_generate
 *
 * @brief   ����OOB key
 *
 * @param   cfg - ������Ϣ
 *
 * @return  none
 */
static void oob_key_generate(struct bt_als_cfg const *cfg)
{
    int                           err;
    uint32_t                      pid;
    uint8_t                       out[8 + 1 + 12 + 1 + 32], dig[32];
    struct tc_sha256_state_struct s;

    tc_sha256_init(&s);

    /** pid value */
    pid = ((uint32_t)((((cfg->pid) >> 24) & 0xff) | (((cfg->pid) >> 8) & 0xff00) | (((cfg->pid) & 0xff00) << 8) | (((cfg->pid) & 0xff) << 24)));
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
    if(err != TC_CRYPTO_SUCCESS)
    {
        APP_DBG("Unable Update Sha256");
        return;
    }

    err = tc_sha256_final(dig, &s);
    if(err != TC_CRYPTO_SUCCESS)
    {
        APP_DBG("Unable Generate sha256 value");
        return;
    }

    memcpy(static_key, dig, 16);
}

/*********************************************************************
 * @fn      ind_reset
 *
 * @brief   �Ƴ��б����÷�����ɻص����ͷŻ���
 *
 * @param   ind     - ��Ҫ���õ�֪ͨ
 * @param   err     - ������
 *
 * @return  none
 */
static void ind_reset(struct bt_mesh_indicate *ind, int err)
{
    if(ind->param.cb && ind->param.cb->end)
    {
        ind->param.cb->end(err, ind->param.cb_data);
    }

    tmos_msg_deallocate(ind->buf->__buf);
    ind->buf->__buf = NULL;
}

/*********************************************************************
 * @fn      bt_mesh_indicate_reset
 *
 * @brief   �ͷ�����δ���͵�֪ͨ
 *
 * @param   none
 *
 * @return  none
 */
void bt_mesh_indicate_reset(void)
{
    uint8_t i;
    for(i = 0; i < CONFIG_INDICATE_NUM; i++)
    {
        if(indicate[i].buf->__buf != NULL)
        {
            ind_reset(&indicate[i], -ECANCELED);
        }
    }
}

/*********************************************************************
 * @fn      ind_start
 *
 * @brief   ���� indicate ��ʼ�ص�
 *
 * @param   duration    - ���η��ͽ�Ҫ������ʱ��
 * @param   err         - ������
 * @param   cb_data     - �ص�����
 *
 * @return  none
 */
static void ind_start(uint16_t duration, int err, void *cb_data)
{
    struct bt_mesh_indicate *ind = cb_data;

    if(ind->buf->__buf == NULL)
    {
        return;
    }

    if(err)
    {
        APP_DBG("Unable send indicate (err:%d)", err);
        tmos_start_task(als_vendor_model_TaskID, ind->event, K_MSEC(100));
        return;
    }
}

/*********************************************************************
 * @fn      ind_end
 *
 * @brief   ���� indicate �����ص�
 *
 * @param   err         - ������
 * @param   cb_data     - �ص�����
 *
 * @return  none
 */
static void ind_end(int err, void *cb_data)
{
    struct bt_mesh_indicate *ind = cb_data;

    if(ind->buf->__buf == NULL)
    {
        return;
    }
    tmos_start_task(als_vendor_model_TaskID, ind->event, ind->param.period);
}

// ���� indicate �ص��ṹ��
const struct bt_mesh_send_cb ind_cb =
    {
        .start = ind_start,
        .end = ind_end,
};

/*********************************************************************
 * @fn      adv_ind_send
 *
 * @brief   ���� indicate
 *
 * @param   ind - ��Ҫ���͵�֪ͨ
 *
 * @return  none
 */
static void adv_ind_send(struct bt_mesh_indicate *ind)
{
    int err;
    NET_BUF_SIMPLE_DEFINE(msg, 32);

    struct bt_mesh_msg_ctx ctx = {
        .app_idx = vnd_models[0].keys[0],
        .addr = ALI_TM_SUB_ADDRESS,
    };

    if(ind->buf->__buf == NULL)
    {
        APP_DBG("NULL buf");
        return;
    }

    if(ind->param.trans_cnt == 0)
    {
        ind_reset(ind, -ETIMEDOUT);
        return;
    }

    ind->param.trans_cnt--;

    ctx.send_ttl = ind->param.send_ttl;

    net_buf_simple_add_mem(&msg, ind->buf->data, ind->buf->len);

    err = bt_mesh_model_send(vnd_models, &ctx, &msg, &ind_cb, ind);
    if(err)
    {
        APP_DBG("Unable send model message (err:%d)", err);
        ind_reset(ind, -EIO);
        return;
    }
}

/*********************************************************************
 * @fn      bt_mesh_ind_alloc
 *
 * @brief   ��һ���յ�indicate���������ڴ�
 *
 * @param   len - ��Ҫ��������ݳ���
 *
 * @return  indicate�ṹ��ָ��
 */
struct bt_mesh_indicate *bt_mesh_ind_alloc(uint16_t len)
{
    uint8_t i;
    for(i = 0; i < CONFIG_INDICATE_NUM; i++)
    {
        if(indicate[i].buf->__buf == NULL)
            break;
    }
    if(i == CONFIG_INDICATE_NUM)
    {
        return NULL;
    }

    indicate[i].buf->__buf = tmos_msg_allocate(len);
    indicate[i].buf->size = len;

    if(indicate[i].buf->__buf == NULL)
    {
        return NULL;
    }

    return &indicate[i];
}

/*********************************************************************
 * @fn      bt_mesh_indicate_send
 *
 * @brief   ��������֪ͨ���¼�
 *
 * @param   ind - indicate�ṹ��ָ��
 *
 * @return  none
 */
void bt_mesh_indicate_send(struct bt_mesh_indicate *ind)
{
    tmos_start_task(als_vendor_model_TaskID, ind->event, ind->param.rand);
}

/*********************************************************************
 * @fn      send_led_indicate
 *
 * @brief   ���͵�ǰLED����״̬������LED����״̬����ʱ����Ҫ���ô˺���
 *
 * @param   param -  ����֪ͨ�ķ��Ͳ���
 *
 * @return  none
 */
void send_led_indicate(struct indicate_param *param)
{
    struct bt_mesh_indicate *ind;

    ind = bt_mesh_ind_alloc(16);
    if(!ind)
    {
        APP_DBG("Unable allocate buffers");
        return;
    }
    memcpy(&(ind->param), param, sizeof(struct indicate_param));

    /* Init indication opcode */
    bt_mesh_model_msg_init(&(ind->buf->b), OP_VENDOR_MESSAGE_ATTR_INDICATION);

    /* Add tid field */
    net_buf_simple_add_u8(&(ind->buf->b), param->tid);

    /* Add generic onoff attrbute op */
    net_buf_simple_add_le16(&(ind->buf->b), ALI_GEN_ATTR_TYPE_POWER_STATE);

    /* Add current generic onoff status */
    net_buf_simple_add_u8(&(ind->buf->b), read_led_state(MSG_PIN));

    bt_mesh_indicate_send(ind);
}

/*********************************************************************
 * @fn      send_lightness_indicate
 *
 * @brief   ���͵�ǰLED���ȣ�����LED���ȸ���ʱ����Ҫ���ô˺���
 *
 * @param   param   - ����֪ͨ�ķ��Ͳ���
 *
 * @return  none*/
void send_lightness_indicate(struct indicate_param *param)          //��Ӵ�
{
    struct bt_mesh_indicate *ind;

    ind = bt_mesh_ind_alloc(16);
    if(!ind)
    {
        APP_DBG("Unable allocate buffers");
        return;
    }
    memcpy(&(ind->param), param, sizeof(struct indicate_param));

    /* Init indication opcode */
    bt_mesh_model_msg_init(&(ind->buf->b), OP_VENDOR_MESSAGE_ATTR_INDICATION);

    /* Add tid field */
    net_buf_simple_add_u8(&(ind->buf->b), param->tid);

    /* Add brightness attrbute opcode */
    net_buf_simple_add_le16(&(ind->buf->b), ALI_GEN_ATTR_TYPE_BRIGHTNESS);

    /* Add brightness status (655~65535��Ӧ��è����1~100) */
    net_buf_simple_add_le16(&(ind->buf->b), read_led_lightness(MSG_PIN));           //��Ӵ����޸ģ�

    bt_mesh_indicate_send(ind);
}

/*********************************************************************
 * @fn      send_lightness_indicate
 *
 * @brief   ���͵�ǰLEDɫ�£�����LEDɫ�¸���ʱ����Ҫ���ô˺���
 *
 * @param   param   - ����֪ͨ�ķ��Ͳ���
 *
 * @return  none*/
void send_color_indicate(struct indicate_param *param)          //��Ӵ�
{
    struct bt_mesh_indicate *ind;

    ind = bt_mesh_ind_alloc(16);
    if(!ind)
    {
        APP_DBG("Unable allocate buffers");
        return;
    }
    memcpy(&(ind->param), param, sizeof(struct indicate_param));

    /* Init indication opcode */
    bt_mesh_model_msg_init(&(ind->buf->b), OP_VENDOR_MESSAGE_ATTR_INDICATION);

    /* Add tid field */
    net_buf_simple_add_u8(&(ind->buf->b), param->tid);

    /* Add brightness attrbute opcode */
    net_buf_simple_add_le16(&(ind->buf->b), ALI_GEN_ATTR_TYPE_COLOR);

    /* Add brightness status (992~20000��Ӧ��è����3000~6400) */
    net_buf_simple_add_le16(&(ind->buf->b), read_led_color(MSG_PIN));           //��Ӵ����޸ģ�

    bt_mesh_indicate_send(ind);
}

/*********************************************************************
 * @fn      als_vendor_init
 *
 * @brief   ���� ����ģ�� ��ʼ��
 *
 * @param   model -  �ص�ģ�Ͳ���
 *
 * @return  always success
 */
int als_vendor_init(struct bt_mesh_model *model)
{
    uint32_t ran;

    uuid_generate(&cfg);
    oob_key_generate(&cfg);

    /** Random Local TID Value
     *  @Caution Don't use single octer only.
     */
    ran = tmos_rand();
    als_tid += ((uint8_t *)&ran)[0];
    als_tid += ((uint8_t *)&ran)[1];
    als_tid += ((uint8_t *)&ran)[2];
    als_tid += ((uint8_t *)&ran)[3];

    for(int i = 0; i < CONFIG_INDICATE_NUM; i++)
    {
        indicate[i].buf = &ind_buf[i];
        indicate[i].event = (1 << i);
    }

    als_vendor_model_TaskID = TMOS_ProcessEventRegister(als_vendor_model_ProcessEvent);
    return 0;
}

/*********************************************************************
 * @fn      als_vendor_model_ProcessEvent
 *
 * @brief   ���ﳧ��ģ���¼�����
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
static uint16_t als_vendor_model_ProcessEvent(uint8_t task_id, uint16_t events)
{
    for(int i = 0; i < CONFIG_INDICATE_NUM; i++)
    {
        if(events & indicate[i].event)
        {
            adv_ind_send(&indicate[i]);
            return (events ^ indicate[i].event);
        }
    }

    // Discard unknown events
    return 0;
}


/******************************** endfile @ main ******************************/
