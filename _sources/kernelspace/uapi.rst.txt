.. SPDX-License-Identifier: AGPL-3.0-only

uAPI
====

The kernel module creates a miscellaneous character device located at
:file:`/dev/ums` with which the user application can interact through ``ioctl``
calls.
All UMS objects are private to each process that opens the UMS device.
Indeed every process that opens the UMS device gets allocated a private pool of
UMS schedulers, UMS workers and completion lists.

.. toctree::
    :maxdepth: 3

    uapi/definitions
    uapi/usage
