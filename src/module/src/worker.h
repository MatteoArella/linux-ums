/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef WORKER_H
#define WORKER_H

#include "uapi/ums/ums_ioctl.h"

#include "ums.h"
#include "context.h"
#include "complist.h"
#include "proc/worker.h"

struct ums_worker {
	struct ums_context context;
	struct ums_complist *complist;

	struct ums_worker_proc_dirs dirs;
};

int enter_ums_worker_mode(struct ums_data *data,
			  struct enter_ums_mode_args *args);

int ums_thread_yield(struct ums_data *data, void __user *args);

int ums_thread_end(struct ums_data *data);

void ums_worker_destroy(struct ums_worker *worker);

#endif /* WORKER_H */
