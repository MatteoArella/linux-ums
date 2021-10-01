# Linux User-Mode Scheduling

[![Main](https://github.com/MatteoArella/linux-ums/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/MatteoArella/linux-ums/actions/workflows/main.yml)

## Overview

User-mode scheduling (UMS) is a lightweight mechanism that applications can
use to schedule their own threads.
The ability to switch between threads in user mode makes UMS more efficient
than thread pools for managing large numbers of short-duration work items that
require few system calls.

To take advantage of UMS, an application must implement a scheduler component
that manages the application's UMS threads and determines when they should run.

The implementation of a User-mode scheduling is inspired from the UMS
specification available in the [Windows kernel](https://docs.microsoft.com/en-us/windows/win32/procthread/user-mode-scheduling).

The package is made of a Linux Loadable Kernel Module (LKM) that implements all the
backend services for executing and yielding UMS worker threads and a Userspace library which interacts with the LKM.

The code has been tested from linux kernel 4.4 to 5.12.

## Documentation

The online documentation for latest release is available [here](https://matteoarella.github.io/linux-ums).

A PDF version of the documentation is also available [here](https://github.com/MatteoArella/linux-ums/raw/gh-pages/linuxusermodescheduling.pdf).

## Build

### Minimal requirements

- GNU C >= 5.1
- GNU make >= 3.81
- pkg-config
- linux-headers
- sphinx <sup>[1](#f1)</sup>
- doxygen <sup>[1](#f1)</sup>
- breathe <sup>[1](#f1)</sup>
- sphinx_rtd_theme <sup>[1](#f1)</sup>
- latex <sup>[2](#f2)</sup>

<a name="f1">1</a> Optional: needed only to build the documentation

<a name="f2">2</a> Optional: needed only to build the pdf documentation

### Minimal Build

Build the userspace library and the LKM and install them into the system with:
```
$ ./configure
$ make
$ make install
$ sudo ldconfig
$ sudo modprobe ums
```

### Development requirements

The following dependencies are needed in case of development build:

- autotools-dev
- autoconf
- automake
- libtool
- git
- cppcheck <sup>[3](#f2)</sup>

<a name="f3">3</a> Optional: needed only for userspace library static code analysis

### Development Build

For starting developying the package you need to run:

```
$ ./autogen.sh
```

### Build documentation

For build the documentation locally enable it during configuration:

``
$ ./configure --enable-docs
``

Then build the HTML documentation by:

``
$ make html
``

or the PDF documentation by:

``
$ make pdf
``

### Build examples

Enable the examples building during configuration:

``
$ ./configure --enable-examples
``

Then they will be build when invoking ``make``.

### Build benchmark tool

Enable the benchmark tool building during configuration;

``
$ ./configure --enable-benchmark
``

Then it will be build when invoking ``make``.

## License

See [LICENSE](LICENSE) file.
