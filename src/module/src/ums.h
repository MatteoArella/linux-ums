/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_H
#define UMS_H

#include "uapi/ums/ums_ioctl.h"
#include "log.h"
#include "idr_l.h"
#include "proc/base.h"

#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/rhashtable.h>

/**
 * @brief UMS device private data
 */
struct ums_data {
	/**
	 * Pool of UMS completion lists
	 */
	struct idr_l comp_lists;

	/**
	 * Pool of UMS schedulers
	 */
	struct rhashtable schedulers;

	/**
	 * Pool of UMS workers
	 */
	struct rhashtable workers;

	/**
	 * Procfs base dirs
	 */
	struct ums_proc_dirs dirs;
};

/**
 * @brief Initialize UMS device caches
 *
 * @return 0 in case of success, -ERRNO otherwise
 */
int ums_caches_init(void);

/**
 * @brief Destroy UMS device caches
 */
void ums_caches_destroy(void);

/**
 * @brief Enter UMS mode
 *
 * Enter UMS mode as a UMS scheduler or a UMS worker depending on
 * @p args::flags.
 *
 * **Context**: Process context. May sleep.
 *
 * @param[in] data		pointer to the UMS data
 * @param[in] args		userspace pointer for args
 *
 * @return
 * * 0				- OK
 * * -EACCESS			- Bad userspace pointer
 * * -ENOMEM			- No memory available
 * * -EINVAL			- Invalid args
 */
int enter_ums_mode(struct ums_data *data,
		   struct enter_ums_mode_args __user *args);

#endif /* UMS_H */
