// SPDX-License-Identifier: AGPL-3.0-only

#include "ums.h"

#include "scheduler.h"
#include "worker.h"

#include <linux/uaccess.h>
#include <linux/errno.h>

inline int enter_ums_mode(struct ums_data *data,
			  struct enter_ums_mode_args __user *args)
{
	struct enter_ums_mode_args kargs;
	int retval;

	if (copy_from_user(&kargs, args, sizeof(struct enter_ums_mode_args)))
		return -EACCES;

	if (kargs.flags & ENTER_UMS_SCHED)
		retval = enter_ums_scheduler_mode(data, &kargs);
	else if (kargs.flags & ENTER_UMS_WORK)
		retval = enter_ums_worker_mode(data, &kargs);
	else
		return -EINVAL;

	if (!retval) {
		if (copy_to_user(args,
				 &kargs,
				 sizeof(struct enter_ums_mode_args)))
			return -EACCES;
	}

	return retval;
}
