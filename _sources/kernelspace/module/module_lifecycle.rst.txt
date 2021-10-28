.. SPDX-License-Identifier: AGPL-3.0-only

Module lifecycle
================

The linux kernel module implements all backend services for executing and
yielding UMS worker threads.

Once loaded, the kernel module initializes its caches, it registers the UMS
device and it initializes the UMS ``procfs`` directories:

.. literalinclude:: /../src/module/src/ums_mod.c
    :language: c
    :lines: 11-35
    :lineno-match:
    :dedent:
    :caption: src/module/src/ums_mod.c

When the module is unloaded it destroies every resources associated with it:

.. literalinclude:: /../src/module/src/ums_mod.c
    :language: c
    :lines: 37-45
    :lineno-match:
    :dedent:
    :caption: src/module/src/ums_mod.c
