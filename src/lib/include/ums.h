/* SPDX-License-Identifier: AGPL-3.0-only */

/**
 * @file ums.h
 */

#ifndef UMS_H
#define UMS_H

#include <features.h>
#include "umsdefs.h"

__BEGIN_DECLS

/**
 * @brief Create UMS completion list.
 *
 * A completion list is associated with a UMS scheduler thread when the
 * enter_ums_scheduling_mode() function is called to create the scheduler
 * thread.
 * The system queues newly created UMS worker threads to the completion list.
 *
 * When an application's @ref ums_scheduler_entry_point_t entry point function
 * is called, the application's scheduler should retrieve items from the
 * completion list by calling dequeue_ums_completion_list_items().
 *
 * When a completion list is no longer needed, use the
 * delete_ums_completion_list() to release the list. The list must be empty
 * before it can be released.
 *
 * @param[out] completion_list		pointer to an empty UMS completion
 *					list.
 *
 * @return 0 in case of success, -1 otherwise (with <code>errno</code> setted
 *	   accordingly).
 */
extern int create_ums_completion_list(ums_completion_list_t *completion_list)
	__THROW __nonnull ((1));

/**
 * @brief Create UMS worker thread.
 *
 * @param[out] thread		pointer to an empty pthread.
 * @param[in] ums_attr		pointer to a UMS worker attribute.
 * @param[in] func		UMS worker routine.
 * @param[in] args		parameter to pass to the specified worker
 *				routine.
 *
 * @return 0 in case of success, -1 otherwise (with <code>errno</code> setted
 *	   accordingly).
 */
extern int ums_pthread_create(pthread_t *thread, ums_attr_t *ums_attr,
			     void *(*func)(void *), void *args) __THROW;

/**
 * @brief Converts the calling thread into a user-mode scheduling (UMS)
 *	  scheduler thread.
 *
 * An application's UMS scheduler creates one UMS scheduler thread for each
 * processor that will be used to run UMS threads. The scheduler typically sets
 * the affinity of the scheduler thread for a single processor, effectively
 * reserving the processor for the use of that scheduler thread.
 *
 * When a UMS scheduler thread is created, the system calls the
 * @ref ums_scheduler_entry_point_t entry point function specified with the
 * enter_ums_scheduling_mode() function call. The application's scheduler is
 * responsible for finishing any application-specific initialization of the
 * scheduler thread and selecting a UMS worker thread to run.
 *
 * The application's scheduler selects a UMS worker thread to run by calling
 * execute_ums_thread() with the worker thread's UMS thread context. The worker
 * thread runs until it yields control by calling ums_thread_yield() or
 * terminates.
 * The scheduler thread is then available to run another worker thread.
 *
 * A scheduler thread should continue to run until all of its worker threads
 * reach a natural stopping point: that is, all worker threads have yielded or
 * terminated.
 *
 * @param[in] scheduler_startup_info	A pointer to a structure that specifies
 *					UMS attributes for the thread,
 *					including a completion list and a
 *					@ref ums_scheduler_entry_point_t entry
 *					point function.
 *
 * @return 0 in case of success, -1 otherwise (with <code>errno</code> setted
 *	   accordingly).
 */
extern int enter_ums_scheduling_mode(
	ums_scheduler_startup_info_t *scheduler_startup_info)
		__THROW __nonnull ((1));

/**
 * @brief Retrieves user-mode scheduling (UMS) worker threads from the
 *	  specified UMS completion list.
 *
 * The system queues a UMS worker thread to a completion list when the worker
 * thread is created or when it yields. The dequeue_ums_completion_list_items()
 * function retrieves a pointer to a list of all thread contexts in the
 * specified completion list.
 * The get_next_ums_list_item() function can be used to pop UMS thread contexts
 * off the list into the scheduler's own ready thread queue. The scheduler is
 * responsible for selecting threads to run based on priorities chosen by the
 * application.
 *
 * Do not run UMS threads directly from the list provided by
 * dequeue_ums_completion_list_items(), or run a thread transferred from the
 * list to the ready thread queue before the list is completely empty. This can
 * cause unpredictable behavior in the application.
 *
 * If more than one caller attempts to retrieve threads from a shared
 * completion list, only the first caller retrieves the threads. For subsequent
 * callers, the dequeue_ums_completion_list_items() function blocks until any
 * UMS worker thread are queued to the completion list.
 *
 * @param[in] completion_list		A pointer to the completion list from
 *					which to retrieve worker threads.
 * @param[out] ums_thread_list		A pointer to a @ref ums_context_t
 *					variable. On output, this parameter
 *					receives a pointer to the first UMS
 *					thread context in a list of UMS thread
 *					contexts.
 *
 * @return 0 in case of success, -1 otherwise (with <code>errno</code> setted
 *	   accordingly).
 */
extern int dequeue_ums_completion_list_items(
	ums_completion_list_t completion_list, ums_context_t *ums_thread_list)
		__THROW __nonnull ((2));

/**
 * @brief Returns the next user-mode scheduling (UMS) thread context in a list
 *	  of thread contexts.
 *
 * @param[in] context	A UMS context in a list of thread contexts. This list
 *			is retrieved by the dequeue_ums_completion_list_items()
 *			function.
 *
 * @return If the function succeeds, it returns the next thread context in the
 *	   list.
 *	   If there is no thread context after the context specified by the
 *	   @p context parameter, the function returns 0.
 *	   If the function fails, the return value is -1 (with
 *	   <code>errno</code> setted accordingly).
 */
extern ums_context_t get_next_ums_list_item(ums_context_t context) __THROW;

/**
 * @brief Runs the specified UMS worker thread.
 *
 * The execute_ums_thread() runs the specified UMS worker thread until it
 * yields by calling the ums_thread_yield() function or terminates.
 *
 * When a worker thread yields or terminates the system calls the scheduler
 * thread's @ref ums_scheduler_entry_point_t entry point function.
 *
 * @param[in] context	The UMS thread context of the worker thread to run.
 *
 * @return 0 in case of success, -1 otherwise (with <code>errno</code> setted
 *	   accordingly).
 */
extern int execute_ums_thread(ums_context_t context) __THROW;

/**
 * @brief Yields control to the user-mode scheduling (UMS) scheduler thread on
 *	  which the calling UMS worker thread is running.
 *
 * A UMS worker thread calls the ums_thread_yield() function to cooperatively
 * yield control to the UMS scheduler thread on which the worker thread is
 * running. If a UMS worker thread never calls ums_thread_yield(), the worker
 * thread runs until it is terminated.
 *
 * When control switches to the UMS scheduler thread, the system calls the
 * associated scheduler entry point function with the reason
 * #UMS_SCHEDULER_THREAD_YIELD and the @p scheduler_param parameter specified
 * by the worker thread in the
 * @ref ums_thread_yield() "ums_thread_yield(scheduler_param)" call.
 *
 * The application's scheduler is responsible for rescheduling the worker
 * thread.
 *
 * @param[in] scheduler_param	A parameter to pass to the scheduler thread's
 *				@ref ums_scheduler_entry_point_t function.
 *
 * @return 0 in case of success, -1 otherwise (with <code>errno</code> setted
 *	   accordingly).
 */
extern int ums_thread_yield(void *scheduler_param) __THROW;

/**
 * @brief Deletes the specified user-mode scheduling (UMS) completion list.
 *
 * @param[in] completion_list	A pointer to the UMS completion list to be
 *				deleted. The create_ums_completion_list()
 *				function provides this pointer.
 *
 * @return 0 in case of success, -1 otherwise (with <code>errno</code> setted
 *	   accordingly).
 */
extern int delete_ums_completion_list(ums_completion_list_t *completion_list)
	__THROW __nonnull ((1));

__END_DECLS

#endif /* UMS_H */
