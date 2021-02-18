/********************************** (C) COPYRIGHT *******************************
* File Name          : app.c
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
#include "cfg_cli.h"

#include "health_srv.h"
#include "health_cli.h"

#include "mesh_util.h"
#include "mesh_trace.h"
#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_main.h"
#include "mesh_cfg_pub.h"
#include "HAL.h"
#include "dbg.h"

typedef enum
{
	NODE_INIT				=	0,
	NODE_APPKEY_ADD			=	1,
	NODE_MOD_BIND_SET		=	2,
	NODE_MOD_SUB_SET		=	3,
	
	NODE_CONFIGURATIONED	=	4,
} node_stage_t;

typedef enum
{
	LOCAL_INIT				=	0,
	LOCAL_APPKEY_ADD		=	1,
	LOCAL_MOD_BIND_SET		=	2,
	LOCAL_MOD_PUB_SET		=	3,
	
	LOCAL_CONFIGURATIONED	=	4,
} local_stage_t;

typedef union
{
	node_stage_t node;
	local_stage_t local;
}stage_t;

typedef void (*cfg_rsp_handler_t)(void *node, const void *rsp);
typedef bool (*stage_handler_t)(void *node);

typedef struct
{
	cfg_rsp_handler_t rsp;
	stage_handler_t stage;
}cfg_cb_t;

typedef struct
{
	u16_t	node_addr;
	u16_t 	elem_count;
	u16_t 	net_idx;
	u16_t 	retry_cnt:12,
			fixed:1,
			blocked:1;

	stage_t stage;
	const cfg_cb_t *cb;
}node_t;

#define LED_PIN	GPIO_Pin_14

static node_t nodes[2 + 1];
static struct k_delayed_work node_work;

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BLE_MESH_RELAY_ENABLED,
    .beacon = BLE_MESH_BEACON_DISABLED,
    .frnd = BLE_MESH_FRIEND_NOT_SUPPORTED,
    .gatt_proxy = BLE_MESH_GATT_PROXY_NOT_SUPPORTED,

    .default_ttl = 7,

    /* 5 transmissions with 20ms interval */
    .net_transmit = BLE_MESH_TRANSMIT(4, 20),
    .relay_retransmit = BLE_MESH_TRANSMIT(4, 20),
};

static struct bt_mesh_health_srv health_srv;

BLE_MESH_HEALTH_PUB_DEFINE(health_pub, 8);

static const u8_t net_key[16] 	= {
	0x00, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x00, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

static const u8_t dev_key[16] 	= {
	0x00, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x00, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

static const u8_t app_key[16] 	= {
	0x00, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x00, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

static const u16_t net_idx 		=	0x0000;
static const u16_t app_idx 		=	0x0001;
static const u32_t iv_index		=	0x00000000;
static u16_t addr 				= 	0x0006;
static u8_t flags				=	0x00;

static inline BOOL read_led_state(u32_t led_pin)
{
	return !GPIOB_ReadPortPin(led_pin);
}

static inline void set_led_state(u32_t led_pin, BOOL on)
{
	on ? GPIOB_ResetBits(led_pin) : GPIOB_SetBits(led_pin);
}

static node_t * free_node_get(void)
{	
	for (int i = 0; i < ARRAY_SIZE(nodes); i++)
	{
		if (nodes[i].node_addr == BLE_MESH_ADDR_UNASSIGNED)
		{
			return &nodes[i];
		}
	}

	return NULL;
}

static node_t * node_get(u16_t node_addr)
{	
	for (int i = 0; i < ARRAY_SIZE(nodes); i++)
	{
		if (nodes[i].node_addr == node_addr)
		{
			return &nodes[i];
		}
	}

	return NULL;
}

static bool node_should_blocked(u16_t node_addr)
{
	for (int i = 0; i < ARRAY_SIZE(nodes); i++)
	{
		if (nodes[i].node_addr != BLE_MESH_ADDR_UNASSIGNED &&
			nodes[i].node_addr != node_addr &&
			nodes[i].fixed != true &&
			nodes[i].blocked == false)
		{
			return true;
		}
	}

	return false;
}

static node_t * node_unblock_get(void)
{
	for (int i = 0; i < ARRAY_SIZE(nodes); i++)
	{
		if (nodes[i].node_addr != BLE_MESH_ADDR_UNASSIGNED &&
			nodes[i].fixed != true &&
			nodes[i].blocked == false)
		{
			return &nodes[i];
		}
	}

	return NULL;
}

static node_t * node_block_get(void)
{
	for (int i = 0; i < ARRAY_SIZE(nodes); i++)
	{
		if (nodes[i].node_addr != BLE_MESH_ADDR_UNASSIGNED &&
			nodes[i].fixed != true &&
			nodes[i].blocked == true)
		{
			return &nodes[i];
		}
	}

	return NULL;
}

static const char * node_debug(node_stage_t stage)
{
	switch (stage)
	{
		case NODE_INIT:
			return "init";
		case NODE_APPKEY_ADD:
			return "application key added";
		case NODE_MOD_BIND_SET:
			return "model binded";
		case NODE_MOD_SUB_SET:
			return "model subscription set";
		case NODE_CONFIGURATIONED:
			return "configuration completed";
		default:
			return "Unknown";
	}
}

static inline void node_stage_set(node_t *node, node_stage_t new_stage)
{
	node->retry_cnt = 3;

	BT_DBG("%s-->%s", node_debug(node->stage.node), node_debug(new_stage));

	node->stage.node = new_stage;
}

static const char * local_debug(local_stage_t stage)
{
	switch (stage)
	{
		case LOCAL_INIT:
			return "init";
		case LOCAL_APPKEY_ADD:
			return "application key added";
		case LOCAL_MOD_BIND_SET:
			return "model binded";
		case LOCAL_MOD_PUB_SET:
			return "model publish set";
		case LOCAL_CONFIGURATIONED:
			return "configuration completed";
		default:
			return "Unknown";
	}
}

static inline void local_stage_set(node_t *node, local_stage_t new_stage)
{
	node->retry_cnt = 1;

	BT_DBG("%s-->%s", local_debug(node->stage.local), local_debug(new_stage));

	node->stage.local = new_stage;
}

static void node_rsp(void *p1, const void *p2)
{
	node_t *node = p1;
	const cfg_cli_status_t *val = p2;

	switch (val->cfgHdr.opcode)
	{
		case OP_APP_KEY_ADD:
			BT_DBG("Application Key Added");
			node_stage_set(node, NODE_MOD_BIND_SET);
			break;
		case OP_MOD_APP_BIND:
			BT_DBG("OnOff Model Binded");
			node_stage_set(node, NODE_MOD_SUB_SET);
			break;
		case OP_MOD_SUB_ADD:
			BT_DBG("OnOff Model Subscription Set");
			node_stage_set(node, NODE_CONFIGURATIONED);
			break;
		default:
			BT_WARN("Unknown Opcode (0x%04x)", val->cfgHdr.opcode);
			return;
	}
}

static void local_rsp(void *p1, const void *p2)
{
	node_t *node = p1;
	const cfg_cli_status_t *val = p2;

	switch (val->cfgHdr.opcode)
	{
		case OP_APP_KEY_ADD:
			BT_DBG("Application Key Added");
			local_stage_set(node, LOCAL_MOD_BIND_SET);
			break;
		case OP_MOD_APP_BIND:
			BT_DBG("OnOff Model Binded");
			local_stage_set(node, LOCAL_MOD_PUB_SET);
			break;
		case OP_MOD_PUB_SET:
			BT_DBG("OnOff Model Publish Seted");
			local_stage_set(node, LOCAL_CONFIGURATIONED);
			break;
		default:
			BT_WARN("Unknown Opcode (0x%04x)", val->cfgHdr.opcode);
			return;
	}
}

static void cfg_cli_rsp_handler(const cfg_cli_status_t *val)
{
	node_t *node;

	node = node_unblock_get();
	if (!node)
	{
		BT_ERR("Unable find Unblocked Node");
		return;
	}

	if (val->cfgHdr.status == 0xFF)
	{
		BT_ERR("Opcode 0x%04x, timeout", val->cfgHdr.opcode);
		goto end;
	}

	node->cb->rsp(node, val);

end:
	k_delayed_work_submit(&node_work, K_SECONDS(1));
}

static void gen_onoff_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx)
{
    NET_BUF_SIMPLE_DEFINE(msg, 32);
    int err;

    BT_DBG("#mesh-onoff STATUS");

    bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_2(0x82, 0x04));
    net_buf_simple_add_u8(&msg, read_led_state(LED_PIN));

    err = bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
    if (err)
    {
        BT_ERR("#mesh-onoff STATUS: send status failed: %d", err);
    }
}

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
	BT_DBG("#mesh-onoff SET-UNACK");
	set_led_state(LED_PIN, buf->data[0]);
}

static void gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    BT_DBG("#mesh-onoff SET");

    gen_onoff_set_unack(model, ctx, buf);

    gen_onoff_status(model, ctx);
}

static const struct bt_mesh_model_op gen_onoff_op[] = {
    {BLE_MESH_MODEL_OP_2(0x82, 0x01), 0, gen_onoff_get},
    {BLE_MESH_MODEL_OP_2(0x82, 0x02), 2, gen_onoff_set},
    {BLE_MESH_MODEL_OP_2(0x82, 0x03), 2, gen_onoff_set_unack},
    BLE_MESH_MODEL_OP_END,
};

static int generic_onoff_update(struct bt_mesh_model *mod)
{
	BT_DBG("");
	
	set_led_state(LED_PIN, !read_led_state(LED_PIN));
	
	/* generic onoff set unack message */
	bt_mesh_model_msg_init(mod->pub->msg, BLE_MESH_MODEL_OP_2(0x82, 0x03));
	
	net_buf_simple_add_u8(mod->pub->msg, 0x00);
	net_buf_simple_add_u8(mod->pub->msg, read_led_state(LED_PIN));
	net_buf_simple_add(mod->pub->msg, 1);
	
	return 0;
}

BLE_MESH_MODEL_PUB_DEFINE(generic_pub, generic_onoff_update, 20);

struct bt_mesh_cfg_cli cfg_cli = {
	.handler = cfg_cli_rsp_handler,
};

static struct bt_mesh_model root_models[] = {
    BLE_MESH_MODEL_CFG_SRV(&cfg_srv),
	BLE_MESH_MODEL_CFG_CLI(&cfg_cli),
    BLE_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
	BLE_MESH_MODEL(BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_op, &generic_pub, NULL),
};

static struct bt_mesh_elem elements[] = {
    BLE_MESH_ELEM(0, root_models, BLE_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
    .cid = 0x0739,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

static const uint8_t dev_uuid[16] = {0x01};

static void unprov_recv(bt_mesh_prov_bearer_t bearer,
                        const u8_t uuid[16], bt_mesh_prov_oob_info_t oob_info,
					    const unprivison_info_t *info)
{
	BT_DBG("");

	int err;
	
	if (bearer & BLE_MESH_PROV_ADV)
	{
		err = bt_mesh_provision_adv(uuid, net_idx, BLE_MESH_ADDR_UNASSIGNED, 5);
		if (err)
		{
			BT_ERR("Unable Open PB-ADV Session (err:%d)", err);
		}
	}
}

static void link_open(bt_mesh_prov_bearer_t bearer)
{
	BT_DBG("");
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
	BT_DBG("");
	bt_mesh_provisioner_enable(BLE_MESH_PROV_ADV);
}

static bool node_stage(void *p1)
{
	int err;
	bool ret = false;
	node_t *node = p1;

	switch (node->stage.node)
	{
		case NODE_APPKEY_ADD:
			err = bt_mesh_cfg_app_key_add(node->net_idx, node->node_addr, net_idx, app_idx, app_key);
			if (err)
			{
				BT_ERR("Unable to adding Application key (err %d)", err);
				ret = 1;
			}
			break;

		case NODE_MOD_BIND_SET:
			err = bt_mesh_cfg_mod_app_bind(node->net_idx, node->node_addr, node->node_addr, app_idx, BLE_MESH_MODEL_ID_GEN_ONOFF_SRV);
			if (err)
			{
				BT_ERR("Unable to Binding OnOff Model (err %d)", err);
				ret = 1;
			}
			break;

		case NODE_MOD_SUB_SET:
			err = bt_mesh_cfg_mod_sub_add(node->net_idx, node->node_addr, node->node_addr, 0xC000, BLE_MESH_MODEL_ID_GEN_ONOFF_SRV);
			if (err)
			{
				BT_ERR("Unable to Set OnOff Model Subscription (err %d)", err);
				ret = 1;
			}
			break;

		default:
			ret = 1;
			break;
	}
	
	return ret;
}

static bool local_stage(void *p1)
{
	int err;
	bool ret = false;
	node_t *node = p1;

	switch (node->stage.local)
	{
		case LOCAL_APPKEY_ADD:
			err = bt_mesh_cfg_app_key_add(node->net_idx, node->node_addr, net_idx, app_idx, app_key);
			if (err)
			{
				BT_ERR("Unable to adding Application key (err %d)", err);
				ret = 1;
			}
			break;

		case LOCAL_MOD_BIND_SET:
			err = bt_mesh_cfg_mod_app_bind(node->net_idx, node->node_addr, node->node_addr, app_idx, BLE_MESH_MODEL_ID_GEN_ONOFF_SRV);
			if (err)
			{
				BT_ERR("Unable to Binding OnOff Model (err %d)", err);
				ret = 1;
			}
			break;

		case LOCAL_MOD_PUB_SET:
		{
			struct bt_mesh_cfg_mod_pub pub;

			pub.addr = 0xC000;
			pub.app_idx = app_idx;
			pub.cred_flag = false;
			pub.period = BT_MESH_PUB_PERIOD_SEC(2);
			pub.transmit = 0;
			pub.ttl = BLE_MESH_TTL_DEFAULT;

			err = bt_mesh_cfg_mod_pub_set(node->net_idx, node->node_addr, node->node_addr, BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, &pub);
			if (err)
			{
				BT_ERR("Unable to Set OnOff Model Publish (err %d)", err);
				ret = 1;
			}
			break;
		}

		default:
			ret = 1;
			break;
	}
	
	return ret;
}

static const cfg_cb_t node_cfg_cb = {
	node_rsp,
	node_stage,
};

static const cfg_cb_t local_cfg_cb = {
	local_rsp,
	local_stage,
};

static void k_work_handler(struct k_work *work)
{
	node_t *node;
	
	node = node_unblock_get();
	if (!node)
	{
		BT_ERR("Unable find Unblocked Node");
		return;
	}

	if (node->retry_cnt -- == 0)
	{
		BT_ERR("Ran Out of Retransmit");
		goto unblock;
	}
	
	if (!node->cb->stage(node))
	{
		return;
	}

unblock:
	
	node->fixed = true;

	node = node_block_get();
	if (node)
	{
		node->blocked = false;
		k_delayed_work_submit(&node_work, K_NO_WAIT);
	}
}

static node_t * node_cfg_process(node_t *node, u16_t net_idx, u16_t addr, u8_t num_elem)
{
	if (!node)
	{
		node = free_node_get();
		if (!node)
		{
			BT_ERR("No Free Node Object Available");
			return NULL;
		}
		node->net_idx = net_idx;
		node->node_addr = addr;
		node->elem_count = num_elem;
	}

	node->blocked = node_should_blocked(addr);
	
	BT_DBG("blocked %d", node->blocked);
	
	if (!node->blocked)
	{
		k_delayed_work_submit(&node_work, K_NO_WAIT);
	}

	return node;
}

static void prov_complete(u16_t net_idx, u16_t addr, u8_t flags, u32_t iv_index)
{
	int err;
	node_t *node;

	BT_DBG("");

	err = bt_mesh_provisioner_enable(BLE_MESH_PROV_ADV);
	if (err)
	{
		BT_ERR("Unabled Enable Provisoner (err:%d)", err);
	}

	node = node_get(addr);
	if (!node || !node->fixed)
	{
		node = node_cfg_process(node, net_idx, addr, ARRAY_SIZE(elements));
		if (!node)
		{
			BT_ERR("Unable allocate node object");
			return;
		}
		
		node->cb = &local_cfg_cb;
		local_stage_set(node, LOCAL_APPKEY_ADD);
	}
}

static void node_added(u16_t net_idx, u16_t addr, u8_t num_elem)
{
	BT_DBG("");
	node_t *node;

	node = node_get(addr);
	if (!node || !node->fixed)
	{
		node = node_cfg_process(node, net_idx, addr, num_elem);
		if (!node)
		{
			BT_ERR("Unable allocate node object");
			return;
		}

		node->cb = &node_cfg_cb;
		node_stage_set(node, NODE_APPKEY_ADD);
	}
}

static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
    .output_size = 0,
    .output_actions = BLE_MESH_NO_OUTPUT,
	.link_open = link_open,
	.link_close = link_close,
	.complete = prov_complete,
	.unprovisioned_beacon = unprov_recv,
	.node_added = node_added,
};

u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];
static u8_t MESH_MEM[3048];

static void node_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(nodes); i++)
	{
		nodes[i].stage.node = NODE_INIT;
		nodes[i].node_addr = BLE_MESH_ADDR_UNASSIGNED;
	}

	k_delayed_work_init(&node_work, k_work_handler);
}

extern const ble_mesh_cfg_t mesh_cfg;
extern const struct device dev;

static void blemesh_on_sync(void)
{
    int err;
	mem_info_t info;
	
	info.base_addr = MESH_MEM;
	info.mem_len = ARRAY_SIZE(MESH_MEM);
	
	err = bt_mesh_cfg_set(&mesh_cfg, &dev, NULL, &info);
	if (err)
	{
		BT_ERR("Unable set configuration (err:%d)", err);
		return;
	}

    err = bt_mesh_init(&prov, &comp, NULL);
    if (err)
    {
        BT_ERR("Initializing mesh failed (err %d)", err);
        return;
    }
	
	node_init();
	
	BT_DBG("Bluetooth initialized");

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS))
    {
        settings_load();
    }

    if (bt_mesh_is_provisioned())
    {
        BT_INFO("Mesh network restored from flash");
    }
    else
    {
        err = bt_mesh_provision(net_key, net_idx, flags, iv_index, addr, dev_key);
		if (err)
		{
			BT_ERR("Self Privisioning (err %d)", err);
			return;
		}
    }

    BT_DBG("Mesh initialized");
}

int main()
{
#if defined(DEBUG) || defined(NOLIB_DEBUG)
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    UART1_DefInit();
    UART1_BaudRateCfg(115200);
    R8_UART1_FCR = (2 << 6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART1_LCR = RB_LCR_WORD_SZ;
    R8_UART1_IER = RB_IER_TXD_EN;
    R8_UART1_DIV = 1;
    UART1_ByteTrigCfg(UART_1BYTE_TRIG);
#endif
	
	GPIOB_ModeCfg(LED_PIN, GPIO_ModeOut_PP_5mA);

#if 0
	FlashBlockErase(CONFIG_MESH_NVS_ADDR_DEF + 512 * 0);
	FlashBlockErase(CONFIG_MESH_NVS_ADDR_DEF + 512 * 1);
	FlashBlockErase(CONFIG_MESH_NVS_ADDR_DEF + 512 * 2);
	FlashBlockErase(CONFIG_MESH_NVS_ADDR_DEF + 512 * 3);
	
	for(uint16 i=0;i<2048;i++)
	{
		if(i%512 == 0)
		PRINT("\nidx :%u\n",i/512);
		
		PRINT("%02x ",*(char *)(CONFIG_MESH_NVS_ADDR_DEF+i));
	}
	
	while(1);
#endif

	CH57X_BLEInit();
	HAL_Init();
	
	bt_mesh_lib_init();
	
#if 0
	dbg_init();
#endif

	blemesh_on_sync();
	
	while(1)
	{
		TMOS_SystemProcess();
	}
}
