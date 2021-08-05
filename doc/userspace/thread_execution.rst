.. SPDX-License-Identifier: AGPL-3.0-only

UMS Thread Execution
====================

A newly created UMS worker thread is queued to the specified completion list
and does not begin running until the application's UMS scheduler selects it to
run. This differs from non-UMS threads, which the system scheduler
automatically schedules to run.

The scheduler runs a worker thread by calling :c:func:`execute_ums_thread()`
with the worker thread's UMS context. A UMS worker thread runs until it yields
by calling the :c:func:`ums_thread_yield()` function or terminates.
