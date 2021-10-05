.. SPDX-License-Identifier: AGPL-3.0-only

UMS Scheduler
=============

A UMS scheduler thread is a regular pthread that has entered the UMS scheduling
mode.

Enter UMS scheduling mode
-------------------------

A regular pthread is converted into a UMS scheduler thread performing a
``ioctl`` call with :c:macro:`IOCTL_ENTER_UMS` request parameter;
in particular the :c:func:`enter_ums_scheduling_mode()` begins with:

.. literalinclude:: /../src/lib/src/scheduler.c
    :language: c
    :lines: 13-27
    :lineno-match:
    :dedent:
    :caption: src/lib/src/scheduler.c

where the ``enter_ums_mode`` is defined at

.. literalinclude:: /../src/lib/src/private.h
    :language: c
    :lines: 22-25
    :lineno-match:
    :dedent:
    :caption: src/lib/src/private.h

After a pthread has entered UMS scheduling mode it starts an infinite loop
waiting for UMS scheduler events. All scheduling activities are passed from
kernel space through :c:struct:`ums_sched_event` events and then proxied to the
UMS :c:type:`ums_scheduler_entry_point_t`.

.. literalinclude:: /../src/lib/src/scheduler.c
    :language: c
    :lines: 29-67
    :lineno-match:
    :dedent:
    :caption: src/lib/src/scheduler.c

Execute UMS worker thread context
---------------------------------

The execution of a worker thread context is implemented as follows:

.. literalinclude:: /../src/lib/src/scheduler.c
    :language: c
    :lines: 72-75
    :lineno-match:
    :dedent:
    :caption: src/lib/src/scheduler.c
