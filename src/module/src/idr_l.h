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

/**
 * IDR structure wrapper with internal concurrency handling.
 */
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

/**
 * @brief Initialise a IDR.
 *
 * @param[in] idr_l		pointer to the IDR
 */
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

/**
 * @brief Allocate a new ID.
 *
 * @param[in] idr_l		pointer to the IDR
 * @param[in] ptr		pointer to be associated with the new ID
 * @param[in] flags		memory allocation flags
 *
 * @return
 * * the newly allocated ID
 * * -ENOMEM			- No memory available
 * * -ENOSPC			- No free IDs could be found
 */
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

/**
 * @brief Return pointer for given ID
 *
 * Looks up the pointer associated with this ID.  A %NULL pointer may
 * indicate that @p id is not allocated or that the %NULL pointer was
 * associated with this ID.
 *
 * This function can be called under <code>rcu_read_lock()</code>.
 *
 * @param[in] idr_l		pointer to the IDR
 * @param[in] id		pointer ID
 *
 * @return the pointer associated with this ID
 */
#define IDR_L_FIND(idr_l, id)				\
({							\
	void *___ptr;					\
	___ptr = idr_find(&(idr_l)->idr, id);		\
	___ptr;						\
})

/**
 * @brief Iterate through all stored pointers
 *
 * The callback function will be called for each entry in @p idr_l, passing
 * the ID, the entry and @p data.
 *
 * If @p func returns anything other than 0, the iteration stops and that
 * value is returned from this function.
 *
 * IDR_L_FOR_EACH() can be called concurrently with IDR_L_ALLOC() and
 * IDR_L_REMOVE() if protected by RCU.  Newly added entries may not be
 * seen and deleted entries may be seen, but adding and removing entries
 * will not cause other entries to be skipped, nor spurious ones to be seen.
 *
 * @param[in] idr_l		pointer to the IDR
 * @param[in] func		function to be called for each pointer
 * @param[in] data		data passed to callback function
 */
#define IDR_L_FOR_EACH(idr_l, func, data)		\
	idr_for_each(idr_l.idr, func, data)

/**
 * @brief Remove an ID from the IDR
 *
 * Removes this ID from the IDR. If the ID was not previously in the IDR,
 * this function returns %NULL.
 *
 * @param[in] idr_l		pointer to the IDR
 * @param[in] id		pointer ID
 *
 * @return the pointer formerly associated with this ID
 */
#define IDR_L_REMOVE(idr_l, id)				\
do {							\
	unsigned long ___flags;				\
	idr_l_lock_irqsave(idr_l, ___flags);		\
	idr_remove(idr_l.idr, id);			\
	idr_l_unlock_irqrestore(idr_l, ___flags);	\
} while (0)

/**
 * @brief Release all internal memory from an IDR
 *
 * After this function is called, the IDR is empty, and may be reused or
 * the data structure containing it may be freed.
 *
 * A typical clean-up sequence for objects stored in an idr tree will use
 * IDR_L_FOR_EACH() to free all objects, if necessary, then IDR_L_DESTROY() to
 * free the memory used to keep track of those objects.
 *
 * @param[in] idr_l		pointer to the IDR
 */
#define IDR_L_DESTROY(idr_l)				\
	idr_destroy(idr_l.idr)

#endif /* IDR_L_H */
