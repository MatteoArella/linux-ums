/* SPDX-License-Identifier: AGPL-3.0-only */

/**
 * @file umsdefs.h
 */

#ifndef UMS_DEFS_H
#define UMS_DEFS_H

#include <features.h>
#include <pthread.h>

__BEGIN_DECLS

/**
 * @brief UMS worker thread context.
 */
typedef pid_t ums_context_t;

/**
 * @brief UMS completion list.
 */
typedef int ums_completion_list_t;

/**
 * @brief UMS scheduling proc activation.
 */
typedef union ums_activation_u {
	/**
	 * UMS worker thread context
	 */
	ums_context_t context;
} ums_activation_t;

/**
 * @brief UMS scheduling proc activation reason.
 */
typedef enum ums_reason_e {
	UMS_SCHEDULER_STARTUP = 0,
	UMS_SCHEDULER_THREAD_YIELD,
	UMS_SCHEDULER_THREAD_END
} ums_reason_t;

/**
 * UMS scheduling proc activation due to scheduler thread startup
 */
#define UMS_SCHEDULER_STARTUP UMS_SCHEDULER_STARTUP

/**
 * UMS scheduling proc activation due to worker thread yield
 */
#define UMS_SCHEDULER_THREAD_YIELD UMS_SCHEDULER_THREAD_YIELD

/**
 * UMS scheduling proc activation due to worker thread termination
 */
#define UMS_SCHEDULER_THREAD_END UMS_SCHEDULER_THREAD_END

/**
 * @brief The application-defined user-mode scheduling (UMS) scheduler entry
 * point function associated with a UMS completion list.
 *
 * @param[in] reason		The reason the scheduler entry point is being
 *				called.
 * @param[in] activation	If the @p reason parameter is
 *				#UMS_SCHEDULER_STARTUP or
 *				#UMS_SCHEDULER_THREAD_END, this parameter is
 *				NULL.
 *				If the @p reason parameter is
 *				#UMS_SCHEDULER_THREAD_YIELD, this parameter
 *				contains the UMS thread context of the UMS
 *				worker thread that yielded.
 * @param[in] scheduler_param	If the @p reason parameter is
 *				#UMS_SCHEDULER_STARTUP, this parameter is the
 *				@ref
 *				ums_scheduler_startup_info_t.scheduler_param
 *				member of the @ref ums_scheduler_startup_info_t
 *				structure passed to the
 *				enter_ums_scheduling_mode() function that
 *				triggered the entry point call.
 *				If the @p reason parameter is
 *				#UMS_SCHEDULER_THREAD_YIELD this parameter is
 *				the @p scheduler_param parameter passed to the
 *				ums_thread_yield() function that triggered the
 *				entry point call.
 */
typedef void (*ums_scheduler_entry_point_t)(ums_reason_t reason,
					    ums_activation_t *activation,
					    void *scheduler_param);

/**
 * @brief Specifies attributes for a user-mode scheduling (UMS) scheduler
 * thread. The enter_ums_scheduling_mode() function uses this structure.
 */
typedef struct ums_scheduler_startup_info_s {
	/**
	 * An UMS completion list to associate with the calling thread.
	 */
	ums_completion_list_t completion_list;

	/**
	 * An application-defined @ref ums_scheduler_entry_point_t entry point
	 * function. The system calls this function when the calling thread has
	 * been converted to UMS and is ready to run UMS worker threads.
	 * Subsequently, it calls this function when a UMS worker thread
	 * running on the calling thread yields or terminates.
	 */
	ums_scheduler_entry_point_t ums_scheduler_entry_point;

	/**
	 * An application-defined parameter to pass to the specified
	 * @ref ums_scheduler_entry_point_t function.
	 */
	void *scheduler_param;
} ums_scheduler_startup_info_t;

/**
 * @brief Specifies attributes for a user-mode scheduling (UMS) worker thread.
 */
typedef struct ums_attr_s {
	/**
	 * An UMS completion list to associate with the worker thread.
	 * The newly created worker thread is queued to the specified
	 * completion list.
	 */
	ums_completion_list_t completion_list;

	/**
	 * A pointer to a pthread attributes to configure the worker thread.
	 */
	pthread_attr_t *pthread_attr;
} ums_attr_t;

__END_DECLS

#endif /* UMS_DEFS_H */
