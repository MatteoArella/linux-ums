.. SPDX-License-Identifier: AGPL-3.0-only

Procfs
======

Overview
--------

The module exposes inside the ``procfs`` several informations about every
running scheduler thread and worker thread.
In particular for every process that opens the UMS device the following
tree is created:

.. code::

    /proc/ums/<pid>
    ├── schedulers
    │   ├── ...
    │   ├── <scheduler-pid>
    │   │   ├── info
    │   │   └── workers
    │   │       ├── 0 -> /proc/ums/<pid>/workers/<worker-pid>
    │   │       ├── ...
    │   │       └── n -> /proc/ums/<pid>/workers/<worker-pid>
    │   └── ...
    └── workers
        ├── ...
        ├── <worker-pid>
        │   └── info
        └── ...

Inside the :file:`/proc/ums/<pid>/schedulers` there is a folder for each UMS
scheduler thread and inside the :file:`/proc/ums/<pid>/workers` there is a
folder for each UMS worker thread.

The ``info`` file shows some statistics as the pid of the UMS thread, the
number of context switches or the current context state (idle/running).

The :file:`/proc/ums/<pid>/schedulers/<scheduler-pid>/workers` folder contains
a symbolic link for each UMS worker thread the UMS scheduler has dequeued from
the completion list; in particular the links are named from 0 to n and they
targets the :file:`/proc/ums/<pid>/workers/<worker-pid>` which they refer to.

Structs
-------

.. doxygenstruct:: ums_proc_dirs
    :project: ums-kmod

.. doxygenstruct:: ums_scheduler_proc_dirs
    :project: ums-kmod

.. doxygenstruct:: ums_worker_complist_node
    :project: ums-kmod

.. doxygenstruct:: ums_worker_proc_dirs
    :project: ums-kmod

Functions
---------

.. doxygenfunction:: ums_proc_init
    :project: ums-kmod

.. doxygenfunction:: ums_proc_destroy
    :project: ums-kmod

.. doxygenfunction:: ums_proc_dirs_init
    :project: ums-kmod

.. doxygenfunction:: ums_proc_dirs_destroy
    :project: ums-kmod

.. doxygenfunction:: get_context_state
    :project: ums-kmod

.. doxygenfunction:: context_snprintf
    :project: ums-kmod

.. doxygenfunction:: ums_scheduler_proc_register
    :project: ums-kmod

.. doxygenfunction:: ums_scheduler_proc_unregister
    :project: ums-kmod

.. doxygenfunction:: ums_scheduler_proc_register_worker
    :project: ums-kmod

.. doxygenfunction:: ums_scheduler_proc_unregister_worker
    :project: ums-kmod

.. doxygenfunction:: ums_worker_proc_register
    :project: ums-kmod

.. doxygenfunction:: ums_worker_proc_unregister
    :project: ums-kmod
