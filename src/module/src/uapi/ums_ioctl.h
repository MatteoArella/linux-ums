#ifndef UMS_IOCTL_H
#define UMS_IOCTL_H

#include <linux/ioctl.h>

typedef int ums_sched_id_t;
typedef int ums_worker_id_t;
typedef int ums_comp_list_id_t;

#define ENTER_UMS_SCHED		0x1
#define ENTER_UMS_WORK		0x2

struct enter_ums_mode_args {
	int flags;
	ums_comp_list_id_t ums_complist;
	union {
		ums_sched_id_t ums_scheduler;
		ums_worker_id_t ums_worker;
	};
};

typedef enum ums_sched_event_type_e {
	SCHEDULER_STARTUP = 0,
	THREAD_BLOCKED,
	THREAD_YIELD
} ums_sched_event_type_t;

struct ums_sched_event {
	ums_sched_event_type_t type;
};

struct ums_sched_dqevent_args {
	ums_sched_id_t ums_scheduler;
	struct ums_sched_event event;
};

struct dequeue_ums_complist_args {
	ums_comp_list_id_t ums_complist;
	ums_worker_id_t ums_context;
};

struct ums_next_context_list_args {
	ums_worker_id_t ums_context;
	ums_worker_id_t ums_next_context;
};

#define UMS_DEV_NAME		"ums"

#define UMS_IOC_MAGIC		0xF0

#define IOCTL_CREATE_UMS_CLIST	_IOR(UMS_IOC_MAGIC, 1, \
				     ums_comp_list_id_t *)

#define IOCTL_ENTER_UMS		_IOWR(UMS_IOC_MAGIC, 2, \
				      struct enter_ums_mode_args *)

#define IOCTL_UMS_SCHED_DQEVENT	_IOWR(UMS_IOC_MAGIC, 3, \
				      struct ums_sched_dqevent_args *)

#define IOCTL_DEQUEUE_UMS_CLIST	_IOWR(UMS_IOC_MAGIC, 4, \
				      struct dequeue_ums_complist_args *)

#define IOCTL_NEXT_UMS_CTX_LIST	_IOWR(UMS_IOC_MAGIC, 5, \
				    struct ums_next_context_list_args *)

#define IOCTL_EXEC_UMS_CTX	_IO(UMS_IOC_MAGIC, 6)
#define IOCTL_UMS_YIELD		_IO(UMS_IOC_MAGIC, 7)

#define IOCTL_DELETE_UMS_CLIST	_IOW(UMS_IOC_MAGIC, 8, \
				     ums_comp_list_id_t)

#endif /* UMS_IOCTL_H */
