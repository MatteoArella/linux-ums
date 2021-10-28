.. SPDX-License-Identifier: AGPL-3.0-only

Definitions and Data types
==========================

Defines
-------

.. doxygendefine:: UMS_DEV_NAME
    :project: ums-kmod

.. doxygendefine:: ENTER_UMS_SCHED
    :project: ums-kmod

.. doxygendefine:: ENTER_UMS_WORK
    :project: ums-kmod

.. doxygendefine:: IOCTL_CREATE_UMS_CLIST
    :project: ums-kmod

.. doxygendefine:: IOCTL_ENTER_UMS
    :project: ums-kmod

.. doxygendefine:: IOCTL_UMS_SCHED_DQEVENT
    :project: ums-kmod

.. doxygendefine:: IOCTL_DEQUEUE_UMS_CLIST
    :project: ums-kmod

.. doxygendefine:: IOCTL_NEXT_UMS_CTX_LIST
    :project: ums-kmod

.. doxygendefine:: IOCTL_EXEC_UMS_CTX
    :project: ums-kmod

.. doxygendefine:: IOCTL_UMS_YIELD
    :project: ums-kmod

.. doxygendefine:: IOCTL_EXIT_UMS
    :project: ums-kmod

.. doxygendefine:: IOCTL_DELETE_UMS_CLIST
    :project: ums-kmod

Enums
-----

.. doxygenenum:: ums_sched_event_type_e
    :project: ums-kmod

Structs
-------

.. doxygenstruct:: enter_ums_mode_args
    :project: ums-kmod

.. doxygenstruct:: ums_thread_yield_args
    :project: ums-kmod

.. doxygenstruct:: ums_thread_end_args
    :project: ums-kmod

.. doxygenstruct:: ums_sched_event
    :project: ums-kmod

.. doxygenstruct:: dequeue_ums_complist_args
    :project: ums-kmod

.. doxygenstruct:: ums_next_context_list_args
    :project: ums-kmod

Typedefs
--------

.. doxygentypedef:: ums_comp_list_id_t
    :project: ums-kmod

.. doxygentypedef:: ums_sched_event_type_t
    :project: ums-kmod
