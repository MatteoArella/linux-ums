.. SPDX-License-Identifier: AGPL-3.0-only

UMS Context
===========

A UMS context represents the state of a UMS thread.

Every UMS context is used for indexing its UMS thread owner by means of the
:c:member:`ums_context.pid` key.

Both a :doc:`UMS scheduler <scheduler>` and a :doc:`UMS worker <worker>`
contains their own UMS context.

The UMS context implements all facilities for switching from UMS schedulers and
UMS workers and viceversa.

Defines
-------

.. doxygendefine:: CONTEXT_RUNNING
    :project: ums-kmod

.. doxygendefine:: CONTEXT_IDLE
    :project: ums-kmod

.. doxygendefine:: __set_context_state
    :project: ums-kmod

.. doxygendefine:: set_context_state
    :project: ums-kmod

Structs
-------

.. doxygenstruct:: ums_context
    :project: ums-kmod

Functions
---------

.. doxygenfunction:: ums_context_init
    :project: ums-kmod

.. doxygenfunction:: ums_context_deinit
    :project: ums-kmod

.. doxygenfunction:: current_context_pid
    :project: ums-kmod

.. doxygenfunction:: prepare_suspend_context
    :project: ums-kmod

.. doxygenfunction:: wake_up_context
    :project: ums-kmod

.. doxygenfunction:: prepare_switch_context
    :project: ums-kmod
