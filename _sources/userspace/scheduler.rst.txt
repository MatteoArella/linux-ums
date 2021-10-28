.. SPDX-License-Identifier: AGPL-3.0-only

UMS Scheduler
=============

An application's UMS scheduler is responsible for creating, managing, and
deleting UMS threads and determining which UMS thread to run. An
application's scheduler performs the following tasks:

1. Creates one UMS scheduler thread for each processor on which the
   application will run UMS worker threads.
2. Creates UMS worker threads to perform the work of the application.
3. Maintains its own ready-thread queue of worker threads that are ready to
   run, and selects threads to run based on the application's scheduling
   policies.
4. Creates and monitors one or more completion lists where the system queues
   threads after they finish processing in the kernel. These include newly
   created worker threads and threads that had yielded.
5. Provides a scheduler entry point function to handles notifications from
   the system. The system calls the entry point function when a scheduler
   thread is created, when a worker thread explicitly yields control, or
   when a worker thread terminates.
6. Performs cleanup tasks for worker threads that have finished running.
7. Performs an orderly shutdown of the scheduler when requested by the
   application.
