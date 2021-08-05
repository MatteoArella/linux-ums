.. SPDX-License-Identifier: AGPL-3.0-only

UMS library lifecycle
=====================

All UMS objects are private to each process that opens the UMS device.

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
    :lines: 19-23
    :lineno-match:
    :dedent:
    :caption: src/lib/src/hooks.c

.. literalinclude:: /../src/lib/src/hooks.c
    :language: c
    :lines: 30-34
    :lineno-match:
    :dedent:
