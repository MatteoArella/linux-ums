.. SPDX-License-Identifier: AGPL-3.0-only

UMS Worker
==========

Overview
--------

The life cycle of a UMS worker is bounded to the process that opens the UMS
device and it is represented by the :c:struct:`ums_worker` structure.
Once created, it is stored inside the pool of workers pointed by
:c:var:`ums_data.workers`.
The kernel ``rhashtable`` structure has been choosen for representing the
workers pool since the number of UMS workers that are going to be created isn't
known in advance. In this way the size of the hashtable will be automatically
adjusted providing better performances.
Every UMS worker is indexed inside the pool by its :c:var:`ums_worker.context`.

Structs
-------

.. doxygenstruct:: ums_worker
    :project: ums-kmod

Functions
---------

.. doxygenfunction:: enter_ums_worker_mode
    :project: ums-kmod

.. doxygenfunction:: ums_worker_yield
    :project: ums-kmod

.. doxygenfunction:: ums_worker_end
    :project: ums-kmod

.. doxygenfunction:: ums_worker_destroy
    :project: ums-kmod
