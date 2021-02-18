
/**
 * @file
 * @brief Public API for FLASH drivers
 */

#ifndef DRIVERS_FLASH_H_
#define DRIVERS_FLASH_H_

/**
 * @brief FLASH Interface
 * @defgroup flash_interface FLASH Interface
 * @ingroup io_interfaces
 * @{
 */

#include "mesh_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*flash_api_read)(int offset, void *data, unsigned int len);
typedef int (*flash_api_write)(int offset, const void *data, unsigned int len);
typedef int (*flash_api_erase)(int offset, unsigned int len);
typedef int (*flash_api_write_protection)(bool enable);

struct flash_config_info {
	u8_t nvs_sector_cnt;
	u8_t nvs_write_size;
	u16_t nvs_sector_size;
	u32_t nvs_store_baddr;
};

struct flash_driver_api {
	flash_api_read read;
	flash_api_write write;
	flash_api_erase erase;
	flash_api_write_protection write_protection;
};

struct device
{
	struct flash_driver_api api;
	struct flash_config_info info;
};

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* DRIVERS_FLASH_H_ */
