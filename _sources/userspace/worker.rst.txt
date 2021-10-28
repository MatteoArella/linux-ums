.. SPDX-License-Identifier: AGPL-3.0-only

UMS Worker Threads and Completion Lists
=======================================

A UMS worker thread is created by calling :c:func:`ums_pthread_create()` and
specifying a completion list.

A completion list is created by calling the
:c:func:`create_ums_completion_list()` function. A completion list receives UMS
worker threads that have completed execution in the kernel and are ready to
run. Only the system can enqueue worker threads to a completion list.
New UMS worker threads are automatically enqueued to the completion list
specified when the threads were created.

Each UMS scheduler thread is associated with a single completion list. However,
the same completion list can be associated with any number of UMS scheduler
threads, and a scheduler thread can retrieve UMS contexts from any completion
list for which it has a pointer.
