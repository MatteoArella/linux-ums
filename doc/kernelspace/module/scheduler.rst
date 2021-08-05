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
