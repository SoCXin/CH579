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
#include "health_srv.h"
#include "mesh_trace.h"
#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_main.h"
#include "mesh_cfg_pub.h"
#include "HAL.h"
#include "dbg.h"

#define LED_PIN_UNPRV	GPIO_Pin_12
#define LED_PIN			GPIO_Pin_14

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
    .beacon = BLE_MESH_BEACON_ENABLED,
    .frnd = BLE_MESH_FRIEND_NOT_SUPPORTED,
    .gatt_proxy = BLE_MESH_GATT_PROXY_ENABLED,

    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit = BLE_MESH_TRANSMIT(4, 20),
    .relay_retransmit = BLE_MESH_TRANSMIT(4, 20),
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
	
	net_buf_simple_add_u8(mod->pub->msg, read_led_state(LED_PIN));
	net_buf_simple_add(mod->pub->msg, 1);
	
	return 0;
}

BLE_MESH_MODEL_PUB_DEFINE(generic_pub, generic_onoff_update, 20);

static struct bt_mesh_model root_models[] = {
    BLE_MESH_MODEL_CFG_SRV(&cfg_srv),
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

static const uint8_t dev_uuid[16] = {0x58,0xab,0x67,0x3d,0x9d,0x44,0x47,0x45,0xb2,0xb6,0x7f,0xaf,0x3d,0xca,0x55,0xa7};

static void link_open(bt_mesh_prov_bearer_t bearer)
{
	BT_DBG("");
	set_led_state(LED_PIN_UNPRV, FALSE);
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
	BT_DBG("");
	if (!bt_mesh_is_provisioned())
	{
		set_led_state(LED_PIN_UNPRV, TRUE);
	}
}

static void prov_complete(u16_t net_idx, u16_t addr,
						u8_t flags, u32_t iv_index)
{
	BT_DBG("addr 0x%04x", addr);
	set_led_state(LED_PIN, TRUE);
}

static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
    .output_size = 0,
    .output_actions = BLE_MESH_NO_OUTPUT,
	.link_open = link_open,
	.link_close = link_close,
	.complete = prov_complete,
};

u8C MacAddr[6] = {0xf8,0xa7,0x63,0x6a,0xec,0x3f};

u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];
static u8_t MESH_MEM[2048 + 64];

extern void CH57X_BLEInit(void);
extern void HAL_Init(void);

#define LOCAL_ADV_NAME	"WCH-BLE-MESH"

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
        bt_mesh_prov_enable(BLE_MESH_PROV_GATT);
		set_led_state(LED_PIN_UNPRV, TRUE);
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
	GPIOB_ModeCfg(LED_PIN_UNPRV, GPIO_ModeOut_PP_5mA);
	
	set_led_state(LED_PIN, FALSE);
	set_led_state(LED_PIN_UNPRV, FALSE);

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

#if 1
	dbg_init();
#endif

	blemesh_on_sync();
	
	while(1)
	{
		TMOS_SystemProcess();
	}
}
