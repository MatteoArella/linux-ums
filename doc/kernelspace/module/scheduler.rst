.. SPDX-License-Identifier: AGPL-3.0-only

UMS Scheduler
=============

Overview
--------

The life cycle of a UMS scheduler is bounded to the process that opens the UMS
device and it is represented by the :c:struct:`ums_scheduler` structure.
Once created, it is stored inside the pool of schedulers pointed by
:c:var:`ums_data.schedulers`.
The kernel ``rhashtable`` structure has been choosen for representing the
schedulers pool since the number of UMS schedulers that are going to be created
isn't known in advance. In this way the size of the hashtable will be
automatically adjusted providing better performances.
Every UMS scheduler is indexed inside the pool by its
:c:var:`ums_scheduler.context`.

UMS scheduling activities are notified to userland process by means of
:c:macro:`IOCTL_UMS_SCHED_DQEVENT` ``ioctl`` calls.
After a userland thread converts itself to UMS scheduling thread a
:c:struct:`ums_event_node` of type
:c:var:`ums_sched_event_type_e.SCHEDULER_STARTUP` is posted to the newly
created scheduler's :c:member:`ums_scheduler.event_q`; when an UMS worker
thread yields or terminates a :c:struct:`ums_event_node` of type
:c:var:`ums_sched_event_type_e.THREAD_YIELD` or
:c:var:`ums_sched_event_type_e.THREAD_TERMINATED` is posted to the scheduler's
event queue.

Every :c:struct:`ums_event_node` is allocated by a dedicated slab cache in
order to speed up allocation time. The dedicated slab cache is created when the
UMS module is loaded by calling :c:func:`ums_scheduling_cache_create()` and
destroied when the module is unloaded by calling
:c:func:`ums_scheduling_cache_destroy()`.

When a UMS scheduler is created it is registered to the dedicated UMS procfs
at :file:`/proc/ums/<pid>/schedulers/<scheduler-pid>`, where ``<pid>`` is the
PID of the process that opens the UMS device and ``<scheduler-pid>`` is the PID
of the UMS scheduler.
Once a UMS scheduler terminates it is unregistered from the UMS procfs.

For every UMS worker that is dequeued by a UMS scheduler from its UMS
completion list a symlink is created inside
:file:`/proc/ums/<pid>/schedulers/<scheduler-pid>/workers` folder and it is
subsequently deleted when the UMS scheduler executes one of them.
In this way at any time the
:file:`/proc/ums/<pid>/schedulers/<scheduler-pid>/workers` folder represents
the list of UMS workers that an UMS scheduler is owning.

Defines
-------

.. doxygendefine:: EVENT_ADD_HEAD
    :project: ums-kmod

.. doxygendefine:: EVENT_ADD_TAIL
    :project: ums-kmod

Structs
-------

.. doxygenstruct:: ums_scheduler
    :project: ums-kmod

.. doxygenstruct:: ums_event_node
    :project: ums-kmod

Functions
---------

.. doxygenfunction:: ums_scheduling_cache_create
    :project: ums-kmod

.. doxygenfunction:: ums_scheduling_cache_destroy
    :project: ums-kmod

.. doxygenfunction:: enter_ums_scheduler_mode
    :project: ums-kmod

.. doxygenfunction:: alloc_ums_event
    :project: ums-kmod

.. doxygenfunction:: free_ums_event
    :project: ums-kmod

.. doxygenfunction:: enqueue_ums_sched_event
    :project: ums-kmod

.. doxygenfunction:: ums_scheduler_destroy
    :project: ums-kmod

.. doxygenfunction:: exec_ums_context
    :project: ums-kmod
