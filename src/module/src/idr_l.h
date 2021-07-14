/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef IDR_L_H
#define IDR_L_H

#include <linux/version.h>

#if KERNEL_VERSION(4, 17, 0) <= LINUX_VERSION_CODE
#include <linux/xarray.h>
#else
#include <linux/spinlock.h>
#endif

#include <linux/idr.h>

struct idr_l {
	struct idr idr;

#if KERNEL_VERSION(4, 11, 0) <= LINUX_VERSION_CODE && \
	KERNEL_VERSION(4, 17, 0) > LINUX_VERSION_CODE
	spinlock_t lock;
#endif
};

#if KERNEL_VERSION(4, 11, 0) > LINUX_VERSION_CODE
#define idr_l_lock_irqsave(idr_l, flags)			\
	spin_lock_irqsave(&(idr_l)->idr.lock, flags)

#define idr_l_unlock_irqrestore(idr_l, flags)			\
	spin_unlock_irqrestore(&(idr_l)->idr.lock, flags)

#elif KERNEL_VERSION(4, 17, 0) > LINUX_VERSION_CODE
#define idr_l_lock_irqsave(idr_l, flags)			\
	spin_lock_irqsave(&(idr_l)->lock, flags)

#define idr_l_unlock_irqrestore(idr_l, flags)			\
	spin_unlock_irqrestore(&(idr_l)->lock, flags)
#else
#define idr_l_lock_irqsave(idr_l, flags)			\
	xa_lock_irqsave(&(idr_l)->idr.idr_rt, flags)

#define idr_l_unlock_irqrestore(idr_l, flags)			\
	xa_unlock_irqrestore(&(idr_l)->idr.idr_rt, flags)
#endif

#if KERNEL_VERSION(4, 11, 0) <= LINUX_VERSION_CODE && \
	KERNEL_VERSION(4, 17, 0) > LINUX_VERSION_CODE
#define IDR_L_INIT(idr_l)				\
do {							\
	idr_init(&(idr_l)->idr);			\
	spin_lock_init(&(idr_l)->lock);			\
} while (0)
#else
#define IDR_L_INIT(idr_l)				\
	idr_init(&(idr_l)->idr)
#endif

#if KERNEL_VERSION(3, 9, 0) <= LINUX_VERSION_CODE
#define IDR_L_ALLOC(idr_l, ptr, flags)				\
({								\
	int ___retval;						\
	unsigned long ___flags;					\
	idr_preload(flags);					\
	idr_l_lock_irqsave(idr_l, ___flags);			\
	___retval = idr_alloc(idr_l.idr, ptr, 1, -1, flags);	\
	idr_l_unlock_irqrestore(idr_l, ___flags);		\
	idr_preload_end();					\
	___retval;						\
})
#else
#define IDR_L_ALLOC(idr_l, ptr, flags)					\
({									\
	int ___retval, ___id;						\
	unsigned long ___flags;						\
	do {								\
		if (!idr_pre_get(idr_l.idr, flags)) {			\
			___retval = -ENOMEM;				\
			break;						\
		}							\
		idr_l_lock_irqsave(idr_l, ___flags);			\
		___retval = idr_get_new(idr_l.idr, ptr, &___id);	\
		idr_l_unlock_irqrestore(idr_l, ___flags);		\
	} while (___retval == -EAGAIN);					\
	___retval != 0 ? ___retval : ___id;				\
})
#endif

#define IDR_L_FIND(idr_l, id)				\
({							\
	void *___ptr;					\
	___ptr = idr_find(&(idr_l)->idr, id);		\
	___ptr;						\
})

#define IDR_L_FOR_EACH(idr_l, func, data)		\
	idr_for_each(idr_l.idr, func, data)

#define IDR_L_REMOVE(idr_l, id)				\
do {							\
	unsigned long ___flags;				\
	idr_l_lock_irqsave(idr_l, ___flags);		\
	idr_remove(idr_l.idr, id);			\
	idr_l_unlock_irqrestore(idr_l, ___flags);	\
} while (0)

#define IDR_L_DESTROY(idr_l)				\
	idr_destroy(idr_l.idr)

#endif /* IDR_L_H */
