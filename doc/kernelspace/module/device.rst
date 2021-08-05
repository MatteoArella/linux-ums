.. SPDX-License-Identifier: AGPL-3.0-only

UMS Device
==========

The UMS device is registered once the kernel module is loaded.
It supports three operations: open, ioctl and release.

Open
----

When the UMS device is opened all required resources are allocated and
initialized. In particular the private data of the device points to a local
:c:struct:`ums_data` struct.

Release
-------

When the UMS device is closed all UMS resources that are still present in the
:c:struct:`ums_data` struct are released.

Structs
-------

.. doxygenstruct:: ums_data
    :project: ums-kmod

Functions
---------

.. doxygenfunction:: ums_caches_init
    :project: ums-kmod

.. doxygenfunction:: ums_caches_destroy
    :project: ums-kmod

.. doxygenfunction:: register_ums_device
    :project: ums-kmod

.. doxygenfunction:: unregister_ums_device
    :project: ums-kmod

.. doxygenfunction:: enter_ums_mode
    :project: ums-kmod
