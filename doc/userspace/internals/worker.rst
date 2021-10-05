.. SPDX-License-Identifier: AGPL-3.0-only

UMS Worker
==========

A UMS worker thread is a regular pthread that has entered the UMS working
mode.

Enter UMS working mode
----------------------

A UMS worker thread is created by calling the :c:func:`ums_pthread_create()`
function.

The function is implemented as follows:

.. literalinclude:: /../src/lib/src/worker.c
    :language: c
    :lines: 53-75
    :lineno-match:
    :dedent:
    :caption: src/lib/src/worker.c

where ``worker_proc_args_t`` is defined as

.. literalinclude:: /../src/lib/src/worker.c
    :language: c
    :lines: 5-9
    :lineno-match:
    :dedent:
    :caption: src/lib/src/worker.c

The ``worker_wrapper_routine`` is the routine executed by the newly created
pthread: it starts entering the UMS worker mode and then executing the user
specified function.

In particular:

.. literalinclude:: /../src/lib/src/worker.c
    :language: c
    :lines: 11-51
    :lineno-match:
    :dedent:
    :caption: src/lib/src/worker.c

The ``worker_key`` contains the unique ``worker_wrapper_routine`` argument
specific for each worker thread. When the user specified function calls
``pthread_exit()``, returns or is cancelled because of a ``pthread_cancel()``
request, the destructor routine ``destroy_worker_key`` is called releasing
every resource associated with that UMS worker thread.

UMS worker yield
----------------

The yielding of a UMS worker thread is implemented as follows:

.. literalinclude:: /../src/lib/src/worker.c
    :language: c
    :lines: 77-80
    :lineno-match:
    :dedent:
    :caption: src/lib/src/worker.c
