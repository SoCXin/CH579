 /**
  @defgroup mesh_error Global Error Codes
  @{

  @brief Global Error definitions
*/

/* Header guard */
#ifndef MESH_ERROR_H__
#define MESH_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MESH_ERRORS_BASE Error Codes Base number definitions
 * @{ */
#define MESH_ERROR_BASE_NUM      	(0x0)       ///< Global error base
/** @} */

#define NOERR						(MESH_ERROR_BASE_NUM + 0)
#define EINVAL						(MESH_ERROR_BASE_NUM + 1)
#define EALREADY					(MESH_ERROR_BASE_NUM + 2)
#define ESRCH						(MESH_ERROR_BASE_NUM + 3)
#define EBUSY						(MESH_ERROR_BASE_NUM + 4)
#define ENOTCONN					(MESH_ERROR_BASE_NUM + 5)
#define EAGAIN						(MESH_ERROR_BASE_NUM + 6)
#define ENOBUFS						(MESH_ERROR_BASE_NUM + 7)
#define ENOENT						(MESH_ERROR_BASE_NUM + 8)
#define ENOMEM						(MESH_ERROR_BASE_NUM + 9)
#define EEXIST						(MESH_ERROR_BASE_NUM + 10)
#define EIO							(MESH_ERROR_BASE_NUM + 11)
#define EDEADLK						(MESH_ERROR_BASE_NUM + 12)
#define ESPIPE						(MESH_ERROR_BASE_NUM + 13)
#define EACCES						(MESH_ERROR_BASE_NUM + 14)
#define ENXIO						(MESH_ERROR_BASE_NUM + 15)
#define ENOSPC						(MESH_ERROR_BASE_NUM + 16)
#define EBADMSG						(MESH_ERROR_BASE_NUM + 17)
#define E2BIG						(MESH_ERROR_BASE_NUM + 18)
#define ENOTSUP						(MESH_ERROR_BASE_NUM + 19)
#define EADDRINUSE					(MESH_ERROR_BASE_NUM + 20)
#define EMSGSIZE					(MESH_ERROR_BASE_NUM + 21)
#define ECANCELED					(MESH_ERROR_BASE_NUM + 22)
#define ETIMEDOUT					(MESH_ERROR_BASE_NUM + 23)
#define EADDRNOTAVAIL				(MESH_ERROR_BASE_NUM + 24)

#ifdef __cplusplus
}
#endif
#endif // MESH_ERROR_H__

/**
  @}
*/
