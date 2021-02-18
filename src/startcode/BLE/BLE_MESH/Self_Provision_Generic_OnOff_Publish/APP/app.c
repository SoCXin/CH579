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

#include "mesh_trace.h"

#include "cfg_srv.h"
#include "cfg_cli.h"

#include "health_srv.h"
#include "health_cli.h"

#include "mesh_trace.h"
#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_main.h"
#include "mesh_cfg_pub.h"
#include "HAL.h"
#include "dbg.h"

#define LED_PIN	GPIO_Pin_14

static inline void toggle_led_state(u32_t led_pin)
{
	GPIOB_InverseBits(led_pin);
}

static inline BOOL read_led_state(u32_t led_pin)
{
	return !GPIOB_ReadPortPin(led_pin);
}

static inline void set_led_state(u32_t led_pin, BOOL on)
{
	on ? GPIOB_ResetBits(led_pin) : GPIOB_SetBits(led_pin);
}

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BLE_MESH_RELAY_ENABLED,
    .beacon = BLE_MESH_BEACON_DISABLED,
    .frnd = BLE_MESH_FRIEND_NOT_SUPPORTED,
    .gatt_proxy = BLE_MESH_GATT_PROXY_NOT_SUPPORTED,

    .default_ttl = 7,

    /* 6 transmissions with 10ms interval */
    .net_transmit = BLE_MESH_TRANSMIT(5, 10),
    .relay_retransmit = BLE_MESH_TRANSMIT(5, 10),
};

static struct bt_mesh_health_srv health_srv;

BLE_MESH_HEALTH_PUB_DEFINE(health_pub, 8);

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
    {BLE_MESH_MODEL_OP_2(0x82, 0x02), 1, gen_onoff_set},
    {BLE_MESH_MODEL_OP_2(0x82, 0x03), 1, gen_onoff_set_unack},
    BLE_MESH_MODEL_OP_END,
};

static int generic_onoff_update(struct bt_mesh_model *mod)
{
	static u32_t count;
	//BT_DBG("count %d", ++count);
	PRINT("%d %ld\n", count++, TMOS_GetSystemClock());
	
	toggle_led_state(LED_PIN);
	
	/* generic onoff set unack message */
	bt_mesh_model_msg_init(mod->pub->msg, BLE_MESH_MODEL_OP_2(0x82, 0x03));
	
	net_buf_simple_add_u8(mod->pub->msg, read_led_state(LED_PIN));

	return 0;
}

BLE_MESH_MODEL_PUB_DEFINE(generic_pub, generic_onoff_update, 80);

static const u8_t net_key[16] 	= {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};
static const u8_t dev_key[16] 	= {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};
static const u8_t app_key[16] 	= {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};
static const u16_t net_idx 		=	0x0000;
static const u16_t app_idx 		=	0x0001;
static const u32_t iv_index		=	0x00000000;
static u16_t addr 				= 	0x0001;
static u8_t flags				=	0x00;

static void cfg_cli_rsp_handler(const cfg_cli_status_t *val)
{
	int err;

	if (val->cfgHdr.status == 0xFF)
	{
		BT_ERR("Opcode 0x%04x, timeout", val->cfgHdr.opcode);
		return;
	}

	switch (val->cfgHdr.opcode)
	{
		case OP_APP_KEY_ADD:
			BT_DBG("Application Key Added");

			err = bt_mesh_cfg_mod_app_bind(net_idx, addr, addr, app_idx, BLE_MESH_MODEL_ID_GEN_ONOFF_SRV);
			if (err)
			{
				BT_ERR("Unable to Binding OnOff Model (err %d)", err);
				return;
			}
			break;

		case OP_MOD_APP_BIND:
			BT_DBG("OnOff Model Binded");

			struct bt_mesh_cfg_mod_pub pub;

			pub.addr = 0xC000;
			pub.app_idx = app_idx;
			pub.cred_flag = false;
			pub.period = BT_MESH_PUB_PERIOD_SEC(2);
			pub.transmit = 0;
			pub.ttl = BLE_MESH_TTL_DEFAULT;

			err = bt_mesh_cfg_mod_pub_set(net_idx, addr, addr, BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, &pub);
			if (err)
			{
				BT_ERR("Unable to Set OnOff Model Publish (err %d)", err);
				return;
			}
			break;

		case OP_MOD_PUB_SET:
			BT_DBG("OnOff Model Publish Seted");
			break;

		default:
			BT_ERR("Unknown Opcode 0x%04x", val->cfgHdr.opcode);
			break;
	}
}

struct bt_mesh_cfg_cli cfg_cli = 
{
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

static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
    .output_size = 0,
    .output_actions = BLE_MESH_NO_OUTPUT,
};

u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];
static u8_t MESH_MEM[4048];


#define LOCAL_ADV_NAME	"WCH-BLE-MESH"

static void self_configure(void)
{
	int err;
	
	BT_DBG("");
	
	err = bt_mesh_cfg_app_key_add(net_idx, addr, net_idx, app_idx, app_key);
	if (err)
	{
		BT_ERR("Unable to adding Application key (err %d)", err);
		return;
	}
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
		
		self_configure();
    }

    BT_DBG("Mesh initialized");
}

u8C MacAddr[6] = {0x7f,0x01,0x02,0x03,0x04,0x05};

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

#ifdef DEBUG
	dbg_init();
#endif

	blemesh_on_sync();

	while(1)
	{
		TMOS_SystemProcess();
	}
}
