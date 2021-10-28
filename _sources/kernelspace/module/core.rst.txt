.. SPDX-License-Identifier: AGPL-3.0-only

Core structures and functionalities
===================================

Logging
-------

.. code-block:: c
    :dedent:

    #include <log.h>

.. doxygendefine:: pr_fmt
    :project: ums-kmod

ID allocation
-------------

.. code-block:: c
    :dedent:

    #include <idr_l.h>

.. doxygenstruct:: idr_l
    :project: ums-kmod

.. doxygendefine:: IDR_L_INIT
    :project: ums-kmod

.. doxygendefine:: IDR_L_ALLOC
    :project: ums-kmod

.. doxygendefine:: IDR_L_FIND
    :project: ums-kmod

.. doxygendefine:: IDR_L_FOR_EACH
    :project: ums-kmod

.. doxygendefine:: IDR_L_REMOVE
    :project: ums-kmod

.. doxygendefine:: IDR_L_DESTROY
    :project: ums-kmod
