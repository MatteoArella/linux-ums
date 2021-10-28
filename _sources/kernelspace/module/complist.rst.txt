.. SPDX-License-Identifier: AGPL-3.0-only

UMS Completion List
===================

Overview
--------

The life cycle of a UMS completion list is bounded to the process that opens
the UMS device and it is represented by the :c:struct:`ums_complist` structure.
Once created it is stored inside the pool of completion lists pointed by
:c:var:`ums_data.comp_lists`.
The completion list pool is based on a :c:struct:`idr_l` structure where a new
ID is allocated for every completion list and it is then used for retrieving a
completion list.

The memory allocated for the :c:struct:`ums_complist` structure is managed
through its :c:member:`ums_complist.refcount` and
:c:member:`ums_complist.rhead`: everyone who needs a reference to the
completion list has to increment its reference counter and decrement it when
it has finished using the completion list; when the reference counter of the
completion list reaches zero the memory allocated to it is released through a
``call_rcu`` directive.

The list of UMS worker's contexts associated with a particular UMS completion
list is pointed by :c:member:`ums_complist.head`; when an UMS scheduler
dequeues the UMS workers from a completion list its
:c:member:`ums_complist.head` is transferred to the first UMS worker's context
of the list who becomes the new handler for retrieving the next UMS worker's
contexts.

Defines
-------

.. doxygendefine:: COMPLIST_ADD_HEAD
    :project: ums-kmod

.. doxygendefine:: COMPLIST_ADD_TAIL
    :project: ums-kmod

Structs
-------

.. doxygenstruct:: ums_complist
    :project: ums-kmod

Functions
---------

.. doxygenfunction:: create_ums_complist
    :project: ums-kmod

.. doxygenfunction:: ums_completion_list_add
    :project: ums-kmod

.. doxygenfunction:: ums_complist_dqcontext
    :project: ums-kmod

.. doxygenfunction:: ums_complist_next_context
    :project: ums-kmod

.. doxygenfunction:: get_ums_complist
    :project: ums-kmod

.. doxygenfunction:: put_ums_complist
    :project: ums-kmod

.. doxygenfunction:: ums_complist_delete
    :project: ums-kmod
