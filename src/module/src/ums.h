/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_H
#define UMS_H

#include "uapi/ums_ioctl.h"
#include "log.h"
#include "idr_l.h"
#include "proc/base.h"

#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/rhashtable.h>

struct ums_data {
	struct idr_l comp_lists;
	struct rhashtable context_table;
	struct ums_proc_dirs dirs;
};

int ums_caches_init(void);
void ums_caches_destroy(void);

int enter_ums_mode(struct ums_data *data,
		   struct enter_ums_mode_args __user *args);

int ums_sched_dqevent(struct ums_data *data,
		      struct ums_sched_event __user *args);

#endif /* UMS_H */
