.. SPDX-License-Identifier: AGPL-3.0-only

UMS library lifecycle
=====================

All UMS objects are private to each process that opens the UMS device.
Indeed every process that opens the UMS device gets allocated a private pool of
UMS schedulers, UMS workers and completion lists.

Every time that the library is loaded the UMS device is opened and a
per-process file descriptor, named :c:var:`UMS_FILENO`, is initialized.
All subsequentes ``ioctl`` calls for interacting with the kernel module are
performed upon that file descriptor.
Every time that the library is unloaded the UMS device is closed, releasing all
kernel resources that are associated with the process.

That initialization and deinitialization is implemented through constructor and
destructor functions annotated respectively with
``__attribute__((constructor))`` and ``__attribute__((destructor))``
attributes.

In particular the :c:var:`UMS_FILENO` is declared as

.. literalinclude:: /../src/lib/src/private.h
    :language: c
    :lines: 18
    :lineno-match:
    :dedent:
    :caption: src/lib/src/private.h

and it is managed at

.. literalinclude:: /../src/lib/src/hooks.c
    :language: c
    :lines: 15-61
    :lineno-match:
    :dedent:
    :caption: src/lib/src/hooks.c

Due to file descriptors inheritance between parent and child processes, the
``pthread_atfork()`` function registers an handler that is executed in the
child process after ``fork()`` processing completes; that handler is in charge
of closing the :c:var:`UMS_FILENO` and reopening it, resulting thus in two
separate pools of UMS resources for the two processes.
