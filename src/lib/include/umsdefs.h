#ifndef UMS_DEFS_H
#define UMS_DEFS_H

#include <pthread.h>

typedef int ums_context_t;
typedef int ums_completion_list_t;

typedef union ums_activation_u {
	ums_context_t contex;
} ums_activation_t;

typedef enum ums_reason_e {
	UMS_SCHEDULER_STARTUP = 0,
	#define UMS_SCHEDULER_STARTUP UMS_SCHEDULER_STARTUP

	UMS_SCHEDULER_THREAD_BLOCKED,
	#define UMS_SCHEDULER_THREAD_BLOCKED UMS_SCHEDULER_THREAD_BLOCKED

	UMS_SCHEDULER_THREAD_YIELD
	#define UMS_SCHEDULER_THREAD_YIELD UMS_SCHEDULER_THREAD_YIELD
} ums_reason_t;

typedef struct ums_scheduler_startup_info_s {
	ums_completion_list_t completion_list;
	void (*ums_scheduler_entry_point)(ums_reason_t,
					  ums_activation_t *,
					  void *);
	void *scheduler_param;
} ums_scheduler_startup_info_t;

typedef struct ums_attr_s {
	ums_completion_list_t completion_list;
	ums_context_t ums_context;
	pthread_attr_t* pthread_attr;
} ums_attr_t;

#endif /* UMS_DEFS_H */