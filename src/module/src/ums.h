/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_H
#define UMS_H

#include "uapi/ums_ioctl.h"
#include "log.h"

#include <linux/init.h>
#include <linux/idr.h>
#include <linux/errno.h>
#include <linux/version.h>

struct idr_l {
	struct idr idr;
	spinlock_t lock;
};

struct ums_data {
	struct idr_l comp_lists;
	struct idr_l schedulers;
	struct idr_l workers;
};

#define IDR_L_INIT(idr_l)				\
do {							\
	idr_init(idr_l.idr);				\
	spin_lock_init(idr_l.lock);			\
} while (0)

#if KERNEL_VERSION(3, 9, 0) <= LINUX_VERSION_CODE
#define IDR_L_ALLOC(idr_l, ptr, flags)				\
({								\
	int retval;						\
	idr_preload(flags);					\
	spin_lock(idr_l.lock);					\
	retval = idr_alloc(idr_l.idr, ptr, 1, -1, flags);	\
	spin_unlock(idr_l.lock);				\
	idr_preload_end();					\
	retval;							\
})
#else
#define IDR_L_ALLOC(idr_l, ptr, flags)				\
({								\
	int retval, id;						\
	do {							\
		if (!idr_pre_get(idr_l.idr, flags)) {		\
			retval = -ENOMEM;			\
			break;					\
		}						\
		spin_lock(idr_l.lock);				\
		retval = idr_get_new(idr_l.idr, ptr, &id);	\
		spin_unlock(idr_l.lock);			\
	} while (retval == -EAGAIN);				\
	retval != 0 ? retval : id;				\
})
#endif

#define IDR_L_FIND(idr_l, id)				\
({							\
	void *ptr;					\
	spin_lock(idr_l.lock);				\
	ptr = idr_find(idr_l.idr, id);			\
	spin_unlock(idr_l.lock);			\
	ptr;						\
})

#define IDR_L_FOR_EACH(idr_l, func, data)		\
do {							\
	spin_lock(idr_l.lock);				\
	idr_for_each(idr_l.idr, func, data);		\
	spin_unlock(idr_l.lock);			\
} while (0)

#define IDR_L_REMOVE(idr_l, id)				\
do {							\
	spin_lock(idr_l.lock);				\
	idr_remove(idr_l.idr, id);			\
	spin_unlock(idr_l.lock);			\
} while (0)

#define IDR_L_DESTROY(idr_l)				\
do {							\
	spin_lock(idr_l.lock);				\
	idr_destroy(idr_l.idr);				\
	spin_unlock(idr_l.lock);			\
} while (0)

int ums_caches_init(void);
void ums_caches_destroy(void);

int enter_ums_mode(struct ums_data *data,
		   struct enter_ums_mode_args __user *args);

int ums_sched_dqevent(struct ums_data *data,
		      struct ums_sched_dqevent_args __user *args);

#endif /* UMS_H */
