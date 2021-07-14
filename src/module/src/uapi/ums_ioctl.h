#ifndef UMS_IOCTL_H
#define UMS_IOCTL_H

#include <linux/ioctl.h>
#include <linux/types.h>

typedef int ums_comp_list_id_t;

#define ENTER_UMS_SCHED		0x1
#define ENTER_UMS_WORK		0x2

struct enter_ums_mode_args {
	int flags;
	ums_comp_list_id_t ums_complist;
};

typedef enum ums_sched_event_type_e {
	SCHEDULER_STARTUP = 0,
	THREAD_BLOCKED,
	THREAD_YIELD,
	THREAD_TERMINATED
} ums_sched_event_type_t;

struct ums_thread_yield {
	pid_t context;
	void *scheduler_params;
};

struct ums_thread_end {
	pid_t context;
};

struct ums_sched_event {
	ums_sched_event_type_t type;
	union {
		struct ums_thread_yield yield_params;
		struct ums_thread_end end_params;
	};
};

struct dequeue_ums_complist_args {
	ums_comp_list_id_t ums_complist;
	pid_t ums_context;
};

struct ums_next_context_list_args {
	pid_t ums_context;
	pid_t ums_next_context;
};

#define UMS_DEV_NAME		"ums"

#define UMS_IOC_MAGIC		0xF0

#define IOCTL_CREATE_UMS_CLIST	_IOR(UMS_IOC_MAGIC, 1, \
				     ums_comp_list_id_t *)

#define IOCTL_ENTER_UMS		_IOW(UMS_IOC_MAGIC, 2, \
				     struct enter_ums_mode_args *)

#define IOCTL_UMS_SCHED_DQEVENT	_IOR(UMS_IOC_MAGIC, 3, \
				      struct ums_sched_event *)

#define IOCTL_DEQUEUE_UMS_CLIST	_IOWR(UMS_IOC_MAGIC, 4, \
				      struct dequeue_ums_complist_args *)

#define IOCTL_NEXT_UMS_CTX_LIST	_IOWR(UMS_IOC_MAGIC, 5, \
				    struct ums_next_context_list_args *)

#define IOCTL_EXEC_UMS_CTX	_IOW(UMS_IOC_MAGIC, 6, pid_t)

#define IOCTL_UMS_YIELD		_IOW(UMS_IOC_MAGIC, 7, void *)

#define IOCTL_EXIT_UMS		_IO(UMS_IOC_MAGIC, 8)

#define IOCTL_DELETE_UMS_CLIST	_IOW(UMS_IOC_MAGIC, 9, \
				     ums_comp_list_id_t)

#endif /* UMS_IOCTL_H */
