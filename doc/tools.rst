.. SPDX-License-Identifier: AGPL-3.0-only

Tools
*****

Benchmark tool
==============

If the package has been build with ``--enable-benchmark`` option then the
following tool is provided for executing a benchmark of the UMS mode with
respect to the default pthread one:

.. code::

    $ ums-benchmark [OPTION]... <ums|pthread>

Options
-------

.. tabularcolumns:: |l|L|

========================== ====================================================
``-t, --task=TASK``         Benchmark task to execute; default to ``prime``
``-w, --workers=NUM``       Number of UMS worker threads to run
========================== ====================================================
