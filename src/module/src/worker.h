#ifndef WORKER_H
#define WORKER_H

#include "uapi/ums_ioctl.h"

#include "ums.h"
#include "context.h"
#include "complist.h"

struct ums_worker {
	ums_worker_id_t id;
	struct ums_context context;
	struct ums_complist *complist;
};

int enter_ums_worker_mode(struct ums_data *data,
			  struct enter_ums_mode_args *args);

int ums_worker_destroy(struct ums_worker *worker);

#endif /* WORKER_H */
