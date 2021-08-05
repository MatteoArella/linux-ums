.. SPDX-License-Identifier: AGPL-3.0-only

Overview
********

User-mode scheduling (UMS) is a lightweight mechanism that applications can
use to schedule their own threads.
The ability to switch between threads in user mode makes UMS more efficient
than thread pools for managing large numbers of short-duration work items that
require few system calls.

To take advantage of UMS, an application must implement a scheduler component
that manages the application's UMS threads and determines when they should run.

The implementation of a User-mode scheduling is inspired from the UMS
specification available in the `Windows kernel <https://docs.microsoft.com/en-
us/windows/win32/procthread/user-mode-scheduling>`_.