.. SPDX-License-Identifier: AGPL-3.0-only

UMS Scheduler Thread
====================

A UMS scheduler thread is an ordinary thread that has converted itself to UMS
by calling the :c:func:`enter_ums_scheduling_mode()` function. The system
scheduler determines when the UMS scheduler thread runs based on its priority
relative to other ready threads. The processor on which the scheduler thread
runs is influenced by the thread's affinity, same as for non-UMS threads.

The caller of :c:func:`enter_ums_scheduling_mode()` specifies a completion list
and a :c:type:`ums_scheduler_entry_point_t` entry point function to associate
with the UMS scheduler thread. The system calls the specified entry point
function when it is finished converting the calling thread to UMS. The scheduler
entry point function is responsible for determining the appropriate next action
for the specified thread.

An application might create one UMS scheduler thread for each processor that
will be used to run UMS threads. The application might also set the affinity of
each UMS scheduler thread for a specific logical processor, which tends to
exclude unrelated threads from running on that processor, effectively reserving
it for that scheduler thread. Be aware that setting thread affinity in this way
can affect overall system performance by starving other processes that may be
running on the system.
