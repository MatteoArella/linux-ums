.. SPDX-License-Identifier: AGPL-3.0-only

UMS Scheduler Entry Point Function
==================================

An application's scheduler entry point function is implemented as a
:c:type:`ums_scheduler_entry_point_t` function. The system calls the
application's scheduler entry point function at the following times:

* When a non-UMS thread is converted to a UMS scheduler thread by calling
  :c:func:`enter_ums_scheduling_mode()`.
* When a UMS worker thread calls :c:func:`ums_thread_yield()`.
* When a UMS worker thread terminates.

The :c:type:`ums_reason_t` parameter of the
:c:type:`ums_scheduler_entry_point_t` function specifies the reason that the
entry point function was called. If the entry point function was called because
a new UMS scheduler thread was created, the :c:var:`scheduler_param` parameter
contains data specified by the caller of :c:func:`enter_ums_scheduling_mode()`.
If the entry point function was called because a UMS worker thread yielded, the
:c:var:`scheduler_param` parameter contains data specified by the caller of
:c:func:`ums_thread_yield()`. If the entry point function was called because a
UMS worker thread terminated, the :c:var:`scheduler_param` parameter is NULL.

The scheduler entry point function is responsible for determining the
appropriate next action for the specified thread.

When the scheduler entry point function is called, the application's scheduler
should attempt to retrieve all of the items in its associated completion list
by calling the :c:func:`dequeue_ums_completion_list_items()` function. This
function retrieves a list of UMS thread contexts that have finished processing
in the kernel and are ready to run. The application's scheduler
should not run UMS threads directly from this list because this can cause
unpredictable behavior in the application. Instead, the scheduler should
retrieve all UMS thread contexts by calling the
:c:func:`get_next_ums_list_item()` function once for each context, insert the
UMS thread contexts in the scheduler's ready thread queue, and only then run
UMS threads from the ready thread queue.
