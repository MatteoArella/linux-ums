.. SPDX-License-Identifier: AGPL-3.0-only

.. _uapi_usage:

Usage
=====

In order to use the UMS device include the header:

.. code-block:: c

    #include <ums/ums_ioctl.h>

Open UMS device
---------------

.. code-block:: c

    int ums_dev_fd;

    ums_dev_fd = open("/dev/" UMS_DEV_NAME, O_RDONLY);

Create UMS completion list
--------------------------

.. code-block:: c

    ums_comp_list_id_t comp_list;

    ioctl(ums_dev_fd, IOCTL_CREATE_UMS_CLIST, &comp_list);

Enter UMS scheduler mode
------------------------

.. code-block:: c

    struct enter_ums_mode_args enter_args = {
        .flags = ENTER_UMS_SCHED,
        .ums_complist = comp_list
    };

    ioctl(ums_dev_fd, IOCTL_ENTER_UMS, &enter_args);

Enter UMS worker mode
---------------------

.. code-block:: c

    struct enter_ums_mode_args enter_args = {
        .flags = ENTER_UMS_WORK,
        .ums_complist = comp_list
    };

    ioctl(ums_dev_fd, IOCTL_ENTER_UMS, &enter_args);

Dequeue UMS scheduler event
---------------------------

.. code-block:: c

    struct ums_sched_event event;

    ioctl(ums_dev_fd, IOCTL_UMS_SCHED_DQEVENT, &event);

Dequeue UMS completion list items
---------------------------------

.. code-block:: c

    struct dequeue_ums_complist_args dequeue_args = {
        .ums_complist = comp_list
    };
    ums_context_t context;

    ioctl(ums_dev_fd, IOCTL_DEQUEUE_UMS_CLIST, &dequeue_args);
    context = dequeue_args.ums_context;

Get next UMS context list item
------------------------------

.. code-block:: c

    struct ums_next_context_list_args next_context_args = {
        .ums_context = context
    };
    ums_context_t next_context;

    ioctl(ums_dev_fd, IOCTL_NEXT_UMS_CTX_LIST, &next_context_args);
    next_context = next_context_args.ums_next_context;

Execute UMS worker context
--------------------------

.. code-block:: c

    ums_context_t context;

    ioctl(ums_dev_fd, IOCTL_EXEC_UMS_CTX, context);

UMS worker yield
----------------

.. code-block:: c

    void *yield_param;

    ioctl(ums_dev_fd, IOCTL_UMS_YIELD, yield_param);

Exit UMS worker mode
--------------------

.. code-block:: c

    ioctl(ums_dev_fd, IOCTL_EXIT_UMS);

Delete UMS completion list
--------------------------

.. code-block:: c

    ums_comp_list_id_t comp_list;

    ioctl(ums_dev_fd, IOCTL_DELETE_UMS_CLIST, comp_list);
