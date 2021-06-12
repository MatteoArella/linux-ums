#ifndef UMS_H
#define UMS_H

#include <features.h>
#include "umsdefs.h"

__BEGIN_DECLS

extern int create_ums_completion_list(ums_completion_list_t *completion_list)
	__THROW __nonnull ((1));

extern int ums_pthread_create(pthread_t *thread, ums_attr_t *ums_attr,
			     void *(*func)(void *), void *args) __THROW;

extern int enter_ums_scheduling_mode(
	ums_scheduler_startup_info_t *scheduler_startup_info)
		__THROW __nonnull ((1));

extern int dequeue_ums_completion_list_items(
	ums_completion_list_t completion_list, ums_context_t *ums_thread_list)
		__THROW __nonnull ((2));

extern ums_context_t get_next_ums_list_item(ums_context_t context) __THROW;

extern int execute_ums_thread(ums_context_t ums_context) __THROW;

extern int ums_thread_yield(void *scheduler_param) __THROW;

extern int delete_ums_completion_list(ums_completion_list_t *completion_list)
	__THROW __nonnull ((1));

__END_DECLS

#endif /* UMS_H */
