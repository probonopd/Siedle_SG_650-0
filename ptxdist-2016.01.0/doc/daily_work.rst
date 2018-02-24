Various Aspects of Daily Work
=============================

Using an External Kernel Source Tree
------------------------------------

This application note describes how to use an external kernel source
tree within a PTXdist project. In this case the external kernel source
tree is managed by GIT.

Cloning the Linux Kernel Source Tree
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this example we are using the officially Linux kernel development
tree.

::

    jbe@octopus:~$ git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
    [...]
    jbe@octopus:~$ ls -l
    [...]
    drwxr-xr-x 38 jbe  ptx 4096 2015-06-01 10:21 myprj
    drwxr-xr-x 25 jbe  ptx 4096 2015-06-01 10:42 linux
    [...]

Configuring the PTXdist Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note:: assumption is here, the directory ``/myprj`` contains a valid PTXdist project.

To make PTXdist use of this kernel source tree, instead of an archive we
can simply create a link now:

::

    jbe@octopus:~$ cd myprj
    jbe@octopus:~/myprj$ mkdir local_src
    jbe@octopus:~/myprj$ ln -s ~/linux local_src/kernel.<platformname>

.. note:: The ``<platformname>`` in the example above must be replaced by the name of your own platform.

PTXdist will handle it in the same way as a kernel part of the project.
Due to this, we must setup:

-  Some kind of kernel version

-  Kernel configuration

-  Image type used on our target architecture

-  If we want to build modules

-  Patches to be used (or not)

Lets setup these topics. We just add the kernel component to it.

::

    jbe@octopus:~/myprj$ ptxdist platformconfig

We must enable the **Linux kernel** entry first, to enable kernel
building as part of the project. After enabling this entry, we must
enter it, and:

-  Setting up the **kernel version**

-  Setting up the **MD5 sum** of the corresponding archive

-  Selecting the correct image type in the entry **Image Type**.

-  Configuring the kernel within the menu entry **patching &
   configuration**.

   -  If no patches should be used on top of the selected kernel source
      tree, we keep the **patch series file** entry empty. As GIT should
      help us to create these patches for deployment, it should be kept
      empty on default in this first step.

   -  Select a name for the kernel configuration file and enter it into
      the **kernel config file** entry.

.. Important::
  Even if we do not intend to use a kernel archive, we must setup these
  entries with valid content, else PTXdist will fail. Also the archive
  must be present on the host, else PTXdist will start a download.

Now we can leave the menu and store the new setup. The only still
missing component is a valid kernel config file now. We can use one of
the default config files the Linux kernel supports as a starting point.
To do so, we copy one to the location, where PTXdist expects it in the
current project. In a multi platform project this location is the
platform directory usally in ``configs/<platform-directory>``. We must
store the file with a name selected in the platform setup menu (**kernel
config file**).

Work Flow
~~~~~~~~~

Now its up to ourself working on the GIT based kernel source tree and
using PTXdist to include the kernel into the root filesystem.

To configure the kernel source tree, we simply run:

::

    jbe@octopus:~/myprj$ ptxdist kernelconfig

To build the kernel:

::

    jbe@octopus:~/myprj$ ptxdist targetinstall kernel

To rebuild the kernel:

::

    jbe@octopus:~/myprj$ ptxdist drop kernel compile
    jbe@octopus:~/myprj$ ptxdist targetinstall kernel

Discovering Runtime Dependencies
--------------------------------

Often it happens that an application on the target fails to run, because
one of its dependencies is not fulfilled. This section should give some
hints on how to discover these dependencies.

Dependencies on Shared Libraries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Getting the missed shared library for example at runtime is something
easily done: The dynamic linker prints the missing library to the
console.

To check at build time if all other dependencies are present is easy,
too. The architecture specific ``readelf`` tool can help us here. It
comes with the OSELAS.Toolchain and is called via ``<target>-readelf``.

To test the ``foo`` binary from our new package ``FOO``, we simply run:

::

    $ ./selected_toolchain/<target>-readelf -d platform-<platformname>/root/usr/bin/foo | grep NEEDED
     0x00000001 (NEEDED)                     Shared library: [libm.so.6]
     0x00000001 (NEEDED)                     Shared library: [libz.so.1]
     0x00000001 (NEEDED)                     Shared library: [libc.so.6]

We now can check if all of the listed libraries are present in the root
filesystem. This works for shared libraries, too. It is also a way to
check if various configurations of our package are working as expected
(e.g. disabling a feature should also remove the required dependency of
this feature).

Dependencies on other Resources
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sometimes a binary fails to run due to missing files, directories or
device nodes. Often the error message (if any) which the binary creates
in this case is ambiguous. Here the ``strace`` tool can help us, namely
to observe the binary at runtime. ``strace`` shows all the system calls
the binary or its shared libraries are performing.

``strace`` is one of the target debugging tools which PTXdist provides
in its ``Debug Tools`` menu.

After adding strace to the root filesystem, we can use it and observe
our ``foo`` binary:

::

    $ strace usr/bin/foo
    execve("/usr/bin/foo", ["/usr/bin/foo"], [/* 41 vars */]) = 0
    brk(0)                                  = 0x8e4b000
    access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
    open("/etc/ld.so.cache", O_RDONLY)      = 3
    fstat64(3, {st_mode=S_IFREG|0644, st_size=77488, ...}) = 0
    mmap2(NULL, 77488, PROT_READ, MAP_PRIVATE, 3, 0) = 0xb7f87000
    close(3)                                = 0
    open("/lib//lib/libm-2.5.1.so", O_RDONLY) = 3
    read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0p%\0\000"..., 512) = 512
    mmap2(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0xb7f86000
    fstat64(3, {st_mode=S_IFREG|0555, st_size=48272, ...}) = 0
    mmap2(NULL, 124824, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7f67000
    mmap2(0xb7f72000, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xb) = 0xb7f72000
    mmap2(0xb7f73000, 75672, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0xb7f73000
    close(3)                                = 0
    open("/lib/libc.so.6", O_RDONLY)        = 3
    read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\332X\1"..., 512) = 512
    fstat64(3, {st_mode=S_IFREG|0755, st_size=1405859, ...}) = 0
    [...]

Occasionally the output of ``strace`` can be very long and the
interesting parts are lost. So, if we assume the binary tries to open a
nonexisting file, we can limit the output to all ``open`` system calls:

::

    $ strace -e open usr/bin/foo
    open("/etc/ld.so.cache", O_RDONLY)      = 3
    open("/lib/libm-2.5.1.so", O_RDONLY) = 3
    open("/lib/libz.so.1.2.3", O_RDONLY) = 3
    open("/lib/libc.so.6", O_RDONLY)        = 3
    [...]
    open("/etc/foo.conf", O_RDONLY) = -1 ENOENT (No such file or directory)

The binary may fail due to a missing ``/etc/foo.conf``. This could be a
hint on what is going wrong (it might not be the final solution).

Debugging with CPU emulation
----------------------------

If we do not need some target related feature to run our application, we
can also debug it through a simple CPU emulation. Thanks to QEMU we can
run ELF binaries for other architectures than our build host is.

Running an Application made for a different Architecture
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PTXdist creates a fully working root filesystem with all runtime
components in ``root/``. Lets assume we made a PTXdist based project for
a CPU. Part of this project is our application ``myapp`` we are
currently working on. PTXdist builds the root filesystem and also
compiles our application. It also installs it to ``usr/bin/myapp`` in
the root filesystem.

With this preparation we can run it on our build host:

::

    $ cd platform-<platformname>/root
    platform-<platformname>/root$ qemu-<architecture> -cpu <cpu-core> -L . usr/bin/myapp

This command will run the application ``usr/bin/myapp`` built for an
<cpu-core> CPU on the build host and is using all library compontents
from the current directory.

For the stdin and -out QEMU uses the regular mechanism of the build
host’s operating system. Using QEMU in this way let us simply check our
programs. There are also QEMU environments for other architectures
available.

Debugging an Application made for a different Architecture
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Debugging our application is also possible with QEMU. All we need are a
root filesystem with debug symbols available, QEMU and an architecture
aware debugger.

The root filesystem with debug symbols will be provided by PTXdist, the
architecture aware debugger comes with the OSELAS.Toolchain. Two
consoles are required for this debug session in this example. We start
the QEMU in the first console as:

::

    $ cd platform-<platformname>/root-debug
    platform-<platformname>/root-debug$ qemu-<architecture> -g 1234 -cpu <cpu-core> -L . usr/bin/myapp

.. note:: PTXdist always builds two root filesystems. ``root/`` and
  ``root-debug/``. ``root/`` contains all components without debug
  information (all binaries are in the same size as used later on on the
  real target), while all components in ``root-debug/`` still containing
  the debug symbols and are much bigger in size.

The added *-g 1234* parameter lets QEMU wait for a GDB connection to run
the application.

In the second console we start GDB with the correct architecture
support. This GDB comes with the same OSELAS.Toolchain that was also
used to build the project:

::

    $ ./selected_toolchain/<target>-gdbtui platform-<platformname>/root-debug/usr/bin/myapp

This will run a *curses* based GDB. Not so easy to handle (we must enter
all the commands and cannot click with a mouse!), but very fast to take
a quick look at our application.

At first we tell GDB where to look for debug symbols. The correct
directory here is ``root-debug/``.

::

    (gdb) set solib-absolute-prefix platform-<platformname>/root-debug

Next we connect this GDB to the waiting QEMU:

::

    (gdb) target remote localhost:1234
    Remote debugging using localhost:1234
    [New Thread 1]
    0x40096a7c in _start () from root-debug/lib/ld.so.1

As our application is already started, we can’t use the GDB command
``start`` to run it until it reaches ``main()``. We set a breakpoint
instead at ``main()`` and *continue* the application:

::

    (gdb) break main
    Breakpoint 1 at 0x100024e8: file myapp.c, line 644.
    (gdb) continue
    Continuing.
    Breakpoint 1, main (argc=1, argv=0x4007f03c) at myapp.c:644

The top part of the running gdbtui console will always show us the
current source line. Due to the ``root-debug/`` directory usage all
debug information for GDB is available.

Now we can step through our application by using the commands *step*,
*next*, *stepi*, *nexti*, *until* and so on.

.. note:: It might be impossible for GDB to find debug symbols for
  components like the main C runtime library. In this case they where
  stripped while building the toolchain. There is a switch in the
  OSELAS.Toolchain menu to keep the debug symbols also for the C runtime
  library. But be warned: This will enlarge the OSELAS.Toolchain
  installation on your harddisk! When the toolchain was built with the
  debug symbols kept, it will be also possible for GDB to debug C library
  functions our application calls (so it might worth the disk space).

Migration between Releases
--------------------------

To migrate an existing project from within one minor release to the next
one, we do the following step:

::

    ~/my_bsp# ptxdist migrate

PTXdist will ask us for every new configuration entry what to do. We
must read and answer these questions carefully. At least we shouldn’t
answer blindly with ’Y’ all the time because this could lead into a
broken configuration. On the other hand, using ’N’ all to time is more
safer. We can still enable interesting new features later on.

Increasing Build Speed
----------------------

Modern host systems are providing more than one CPU core. To make use of
this additionally computing power recent applications should do their
work in parallel.

Using available CPU Cores
~~~~~~~~~~~~~~~~~~~~~~~~~

PTXdist uses all available CPU cores when building a project by default.
But there are some exceptions:

-  the prepare stage of all autotools build system based packages can
   use only one CPU core. This is due to the fact, the running
   “configure” is a shell script.

-  some packages have a broken buildsystem regarding parallel build.
   These kind of packages build successfully only when building on one
   single CPU core.

-  creating the root filesystem images are also done on a single core
   only

Manually adjusting CPU Core usage
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Manual adjustment of the parallel build behaviour is possible via
command line parameters.

``-ji<number>``
    this defines the number of CPU cores to build a package. The default
    is two times the available CPU cores.

``-je<number>``
    this defines the number of packages to be build in parallel. The
    default is one package at a time.

``-j<number>``
    this defines the number of CPU cores to be used at the same time. These
    cores will be used on a package base and file base.

``-l<number>``
    limit the system load to the given value.

.. Important:: using ``-ji`` and ``-je`` can overload the system
  immediatley. These settings are very hard.

A much softer setup is to just use the ``-j<number>`` parameter. This will run
up to ``<number>`` tasks at the same time which will be spread over everything
to do. This will create a system load which is much user friendly. Even the
filesystem load is smoother with this parameter.

Building in Background
~~~~~~~~~~~~~~~~~~~~~~

To build a project in background PTXdist can be ’niced’.

``-n[<number>]``
    run PTXdist and all of its child processes with the given
    nicelevel <number>. Without a nicelevel the default is 10.

Building Platforms in Parallel
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Due to the fact that more than one platform can exist in a PTXdist
project, all these platforms can be built in parallel within the same
project directory. As they store their results into different platform
subdirectories, they will not conflict. Only PTXdist must be called
differently, because each call must be parametrized individually.

The used Platform Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    $ ptxdist platform <some-platform-config>

This call will create the soft link ``selected_platformconfig`` to the
``<some-platform-config>`` in the project’s directory. After this call,
PTXdist uses this soft link as the default platform to build for.

It can be overwritten temporarily by the command line parameter
``--platformconfig=<different-platform-config>``.

The used Project Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    $ ptxdist select <some-project-config>

This call will create the soft link ``selected_ptxconfig`` to the
``<some-project-config>`` in the project’s directory. After this call,
PTXdist uses this soft link as the default configuration to build the
project.

It can be overwritten temporarily by the command line parameter
``--ptxconfig=<different-project-config>``.

The used Toolchain to Build
^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    $ ptxdist toolchain <some-toolchain-path>

This call will create the soft link ``selected_toolchain`` to the
``<some-toolchain-path>`` in the project’s directory. After this call,
PTXdist uses this soft link as the default toolchain to build the
project with.

It can be overwritten temporarily by the command line parameter
``--toolchain=<different-toolchain-path>``.

By creating the soft links all further PTXdist commands will use these
as the default settings.

By using the three ``--platformconfig``, ``--ptxconfig`` and
``--toolchain`` parameters, we can switch (temporarily) to a completely
different setting. This feature we can use to build everything in one
project.

A few Examples
^^^^^^^^^^^^^^

The project contains two individual platforms, sharing the same
architecture and same project configuration.

::

    $ ptxdist select <project-config>
    $ ptxdist toolchain <architecture-toolchain-path>
    $ ptxdist --platformconfig=<architecture-A> --quiet go &
    $ ptxdist --platformconfig=<architecture-B> go

The project contains two individual platforms, sharing the same project
configuration.

::

    $ ptxdist select <project-config>
    $ ptxdist --platformconfig=<architecture-A> --toolchain=<architecture-A-toolchain-path> --quiet go &
    $ ptxdist --platformconfig=<architecture-B> --toolchain=<architecture-B-toolchain-path> go

The project contains two individual platforms, but they do not share
anything else.

::

    $ ptxdist --select=<project-A-config> --platformconfig=<architecture-A> --toolchain=<architecture-A-toolchain-path> --quiet go &
    $ ptxdist --select=<project-B-config> --platformconfig=<architecture-B> --toolchain=<architecture-B-toolchain-path> go

Running one PTXdist in background and one in foreground would render the
console output unreadable. That is why the background PTXdist uses the
``--quiet`` parameter in the examples above. Its output is still
available in the logfile under the platform build directory tree.

By using more than one virtual console, both PTXdists can run with their
full output on the console.

Using a Distributed Compiler
----------------------------

To increase the build speed of a PTXdist project can be done by doing
more tasks in parallel. PTXdist itself uses all available CPU cores by
default, but is is limited to the local host. For further speedup a
distributed compilation can be used. This is the task of *ICECC* aka
*icecream*. With this feature a PTXdist project can make use of all
available hosts and their CPUs in a local network.

Setting-Up the Distributed Compiler
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

How to setup the distributed compiler can be found on the project’s
homepage at GITHUB:

https://github.com/icecc/icecream.

Read their ``README.md`` for further details.

.. Important:: as of July 2014 you need at least an *ICECC* in its version
  1.x. Older revisions are known to not work.

Enabling PTXdist for the Distributed Compiler
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since the 2014.07 release, PTXdist supports the usage of *ICECC* by
simply enabling a setup switch.

Run the PTXdist setup and navigate to the new *ICECC* menu entry:

::

    $ ptxdist setup
       Developer Options   --->
          [*] use icecc
          (/usr/lib/icecc/icecc-create-env) icecc-create-env path

Maybe you must adapt the ``icecc-create-env path`` to the setting on
your host. Most of the time the default path should work.

How to use the Distributed Compiler with PTXdist
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PTXdist still uses two times the count of cores of the local CPU for
parallel tasks. But if a faster CPU in the net exists, *ICECC* will now
start to do all compile tasks on this/these faster CPU(s) instead of the
local CPU.

To really boost the build speed you must increase the tasks to be done
in parallel manually. Use the ``-ji<x>`` command line option to start
more tasks at the same time. This command line option just effects one
package to build at a time. To more increase the build speed use the
``-je<x>`` command line option as well. This will build also packages in
parallel.

A complete command line could look like this:

::

    $ ptxdist go -ji64 -je8

This command line will run up to 64 tasks in parallel and builds 8
packages at the same time. Never worry again about your local host and
how slow it is. With the help of *ICECC* every host will be a high speed
development machine.

Using pre-build archives
------------------------

PTXdist is a tool which creates all the required parts of a target’s
filesystem to breathe life into it. And it creates these parts from any
kind of source files. If a PTXdist project consists of many packages the
build may take a huge amount of time.

For internal checks we have a so called “ALL-YES” PTXdist project. It
has - like the name suggests - all packages enabled which PTXdist
supports. To build this “ALL-YES” PTXdist project our build server needs
about 6 hours.

Introduction
~~~~~~~~~~~~

While the development of a PTXdist project it is needed to clean and
re-build everything from time to time to get a re-synced project result
which honors all changes made in the project. But since cleaning and
re-building everything from time to time is a very good test case if
some adaptions are still missing or if everything is complete, it can be
a real time sink to do so.

To not lose developer’s temper when doing such tests, PTXdist can keep
archives from the last run which includes all the files the package’s
build system has installed while the PTXdist’s *install* stage runs for
it.

The next time PTXdist should build a package it can use the results from
the last run instead. This feature can drastically reduce the time to
re-build the whole project. But also, this PTXdist feature must handle
with care and so it is not enabled and used as default.

This section describes how to make use of this PTXdist feature and what
pitfalls exists when doing so.

Creating pre-build archives
~~~~~~~~~~~~~~~~~~~~~~~~~~~

To make PTXdist creating pre-build archives, enable this feature prior a
build in the menu:

::

    $ ptxdist menuconfig

        Project Name & Version --->
            [*] create pre-build archives

Now run a regular build of the whole project:

::

    $ ptxdist go

When the build is finished, the directory ``packages`` contains
additional archives files with the name scheme ``*-dev.tar.gz``. These
files are the pre-build archives which PTXdist can use later on to
re-build the project.

Using pre-build archives
~~~~~~~~~~~~~~~~~~~~~~~~

To make PTXdist using pre-build archives, enable this feature prior a
build in the menu:

::

    $ ptxdist menuconfig

        Project Name & Version --->
            [*] use pre-build archives
            (</some/path/to/the/archives>)

With the next build (e.g. ``ptxdist go``) PTXdist will look for a
specific package if its corresponding pre-build archive does exist. If
it does exist and the used hash value in the pre-build archive’s
filename matches, PTXdist will skip all source archive handling
(extract, patch, compile and install) and just extract and use the
pre-build archive’s content.

A regular and save usecase of pre-build archives is:

-  using one pre-build archive pool for one specific PTXdist project.

-  using a constant PTXdist version all the time.

-  using a constant OSELAS.Toolchain() version all the time.

-  no package with a pre-build archive in the project is under
   development.

The hash as a part of the pre-build archive’s filename does only reflect
the package’s configuration made in the menu (``ptxdist menuconfig``).
If this package specific configuration changes, a new hash value will be
the result and PTXdist can select the corresponding correct pre-build
archive.

This hash value change is an important fact, as many things outside and
inside the package can have a big impact of the binary result but
without a hash value change!

Please be careful when using the pre-build archives if you:

-  intend to switch to a different toolchain with the next build.

-  change the patch set applied to the corresponding package, e.g. a
   package is under development.

-  change the hard coded configure settings in the package’s rule file,
   e.g. a package is under development

-  intend to use one pre-build archive pool from different PTXdist
   projects.

To consider all these precautions the generated pre-build archives are
not transfered automatically where the next build expects them. This
must be done manually by the user of the PTXdist project. Doing so, we
can decide on a package by package base if its pre-build archive should
be used or not.

Packages without pre-build archives support
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No host nor cross related packages can be used based on their pre-build
archives. These kind of packages are always (re-)built.

Only target related packages can be used based on their pre-build
archives, but there are a few exceptions:

-  Linux kernel: It has an incomplete install stage, which results into
   an incomplete pre-build archive. Due to this, it cannot be used as a
   pre-build archive

-  Barebox bootloader: It has an incomplete install stage, which results
   into an incomplete pre-build archive. Due to this, it cannot be used
   as a pre-build archive

-  some other somehow broken packages all marked with a
   ``<packagename>_DEVPKG := NO`` in their corresponding rule file

Workflow with pre-build archives
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We starting with an empty PTXdist project and enabling the pre-build
archive feature as mentioned in the previous section. After that
a regular build of the project can be made.

When the build is finished its time to copy all the pre-build archives
of interest where the next build will expect them.
The previous section mentions the step to enable their use. It also
allows to define a directory. The default path of this directory is made
from various other menu settings, to ensure the pre-build archives of
the current PTXdist project do not conflict with pre-build archives of
different PTXdist projects. To get an idea what the final path is, we
can ask PTXdist.

::

    $ ptxdist print PTXCONF_PROJECT_DEVPKGDIR
    /home/jbe/OSELAS.BSP/Pengutronix/OSELAS.BSP-Pengutronix-Generic

If this directory does not exist, we can simply create it:

::

    $ mkdir -p /home/jbe/OSELAS.BSP/Pengutronix/OSELAS.BSP-Pengutronix-Generic

Now its time to copy the pre-build archives to this new directory. We
could simply copy all pre-build archives from the ``/packages``
directory. But we should keep in mind, if any of the related packages
are under development, we must omit its corresponding pre-build archives
in this step.

::

    $ cp platform-<platformname>/packages/*-dev.tar.gz| /home/jbe/OSELAS.BSP/Pengutronix/OSELAS.BSP-Pengutronix-Generic

Use cases
~~~~~~~~~

Some major possible use cases are covered in this section:

-  Speed up a re-build of one single project

-  Sharing pre-build archives between two platforms based on the same
   architecture

To simply speed up a re-build of the whole project (without development
on any of the used packages) we just can copy all ``*-dev.tar.gz``
archives after the first build to the location where PTXdist expects
them at the next build time.

If two platforms are sharing the same architecture it is possible to
share pre-build archives as well. The best way it can work is, if both
platforms are part of the same PTXdist project. They must also share the
same toolchain settings, patch series and rule files. If these
precautions are handled the whole project can be built for the first
platform and these pre-build archives can be used to build the project
for the second platform. This can reduce the required time to build the
second platform from hours to minutes.

Downloading Packages from the Web
---------------------------------

Sometimes it makes sense to get all required source archives at once.
For example prior to a shipment we want to also include all source
archives, to free the user from downloading it by him/herself.

PTXdist supports this requirement by the ``export_src`` parameter. It
collects all required source archives into one given single directory.
To do so, the current project must be set up correctly, e.g. the
``select`` and ``platform`` commands must be ran prior the
``export_src`` step.

If everything is set up correctly we can run the following commands to
get the full list of required archives to build the project again
without an internet connection.

::

    $ mkdir my_archives
    $ ptxdist export_src my_archives

PTXdist will now collect all source archives to the ``my_archives/``
directory.

.. note:: If PTXdist is configured to share one source archive directory for
  all projects, this step will simply copy the source archives from the
  shared source archive directory. Otherwise PTXdist will start to
  download them from the world wide web.

.. _adding_src_autoconf_lib:

Creating a new Autotools Based Library
--------------------------------------

Developing your own library can be one of the required tasks to support
an embedded system. PTXdist comes with an autotoolized library template
which can be simply integrated into a PTXdist project.

Creating the Library Template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Creating the library package can be done by the PTXdist’s *newpackage*
command:

::

    $ ptxdist newpackage src-autoconf-lib

    ptxdist: creating a new 'src-autoconf-lib' package:

    ptxdist: enter package name...........: foo
    ptxdist: enter version number.........: 1
    ptxdist: enter package author.........: Juergen Borleis <jbe@pengutronix.de>
    ptxdist: enter package section........: project_specific

    generating rules/foo.make
    generating rules/foo.in

    local_src/foo does not exist, create? [Y/n] Y
    ./
    ./internal.h
    ./@name@.c
    ./configure.ac
    ./README
    ./COPYING
    ./Makefile.am
    ./lib@name@.pc.in
    ./autogen.sh
    ./lib@name@.h
    ./wizard.sh

After this step the new directory ``local_src/foo`` exists and contains
various template files. All of these files are dedicated to be modified
by yourself.

The content of this directory is:

::

    $ ls -l local_src/foo/
    total 48
    -rw-r--r-- 1 jbe ptx   335 Jun 18 23:00 COPYING
    -rw-r--r-- 1 jbe ptx  1768 Jun 18 23:16 Makefile.am
    -rw-r--r-- 1 jbe ptx  1370 Jun 18 23:16 README
    -rwxr-xr-x 1 jbe ptx   267 Apr 16  2012 autogen.sh
    -rw-r--r-- 1 jbe ptx 11947 Jun 18 23:16 configure.ac
    -rw-r--r-- 1 jbe ptx   708 Jun 18 23:16 foo.c
    -rw-r--r-- 1 jbe ptx   428 Jun 18 23:00 internal.h
    -rw-r--r-- 1 jbe ptx   185 Jun 18 23:16 libfoo.h
    -rw-r--r-- 1 jbe ptx   331 Jun 18 23:16 libfoo.pc.in
    drwxr-xr-x 2 jbe ptx  4096 Jun 18 23:16 m4

Licence related stuff
~~~~~~~~~~~~~~~~~~~~~

COPYING
^^^^^^^

You must think about the licence your library uses. The template file
``COPYING`` contains some links to GPL/LGPL texts you can use. Replace
the ``COPYING’s`` content by one of the listed licence files or
something different. But do not omit this step. Never!

Build system related files
~~~~~~~~~~~~~~~~~~~~~~~~~~

autogen.sh
^^^^^^^^^^

The autotools are using macro files which are easier to read for a
human. But to work with the autotools these macro files must be
converted into executabe shell code first. The ``autogen.sh`` script
does this job for us.

configure.ac
^^^^^^^^^^^^

This is the first part of the autotools based build system. Its purpose
is to collect as much required information as possible about the target
to build the library for. This file is a macro file. It uses a bunch of
M4 macros to define the job to do. The autotools are complex and this
macro file should help you to create a useful and cross compile aware
``configure`` script everybody can use.

This macro file is full of examples and comments. Many M4 macros are
commented out and you can decide if you need them to detect special
features about the target.

Search for the “TODO” keyword and adapt the setting to your needs. After
that you should remove the “TODO” comments to not confuse any user later
on.

Special hints about some M4 macros:

**AC_INIT**
    add the intended revision number (the second argument), an email
    address to report bugs and some web info about your library. The
    intended revision number will be part of the released archive name
    later on. You can keep it in sync with the API\_RELEASE, but you
    must not.

**AC_PREFIX_DEFAULT**
    most of the time you can remove this entry, because most users
    expect the default install path prefix is ``/usr/local`` which is
    always the default if not changed by this macro.

**API_RELEASE**
    defines the API version of your library. This API version will be
    part of the binary library’s name later on.

**LT_CURRENT / LT_REVISION / LT_AGE**
    define the binary compatibility of your library. The rules how these
    numbers are defined are:

    -  library code was modified: ``LT_REVISION++``

    -  interfaces changed/added/removed: ``LT_CURRENT++`` and
       ``LT_REVISION = 0``

    -  interfaces added: ``LT_AGE++``

    -  interfaces removed: ``LT_AGE = 0``

    You must manually change these numbers whenever you change the code
    in your library prior a release.

**CC_CHECK_CFLAGS / CC_CHECK_LDFLAGS**
    if you need special command line parameters given to the compiler or
    linker, don’t add them unconditionally. Always test, if the tools
    can handle the parameter and fail gracefully if not. Use
    CC_CHECK_CFLAGS to check parameters for the compiler and
    CC_CHECK_LDFLAGS for the linker.

**AX_HARDWARE_FP / AX_DETECT_ARMV\***
    sometimes it is important to know for which architecture or CPU the
    current build is for and if it supports hard float or not. Please
    don’t try to guess it. Ask the compiler instead. The M4
    AX\_HARDWARE_FP and AX_DETECT_ARMV\* macros will help you.

**REQUIRES**
    to enrich the generated \*.pc file for easier dependency handling
    you should also fill the REQUIRES variable. Here you can define from
    the package management point of view the dependencies of your
    library. For example if your library depends on the ’udev’ library
    and requires a specific version of it, just add the string
    ``udev >= 1.0.0`` to the REQUIRES variable. Note: the listed
    packages must be comma-separated.

**CONFLICTS**
    if your library conflicts with a different library, add this
    different library to the CONFLICTS variable (from the package
    management point of view).

It might be a good idea to include the API version into the names of the
library’s include file and pkg-config file. For example in the first API
version all files are named like this:

-  /usr/local/lib/libfoo-1.so.0.0.0

-  /usr/local/include/libfoo-1.h

-  /usr/local/lib/pkgconfig/libfoo-1.pc

In this case its simple to create the next generation libfoo without
conflicting with earlier versions of your library: they can co-exist
side by sid.

-  /usr/local/lib/libfoo-1.so.0.0.0

-  /usr/local/lib/libfoo-2.so.0.0.0

-  /usr/local/include/libfoo-1.h

-  /usr/local/include/libfoo-2.h

-  /usr/local/lib/pkgconfig/libfoo-1.pc

-  /usr/local/lib/pkgconfig/libfoo-2.pc

If you want to do so, you must rename the header file and the pc file
accordingly. And adapt the *pkgconfig\_DATA* and *include\_HEADERS*
entries in the ``Makefile.am`` file, and the *AC\_CONFIG\_FILES* in the
``configure.ac`` file.

Makefile.am
^^^^^^^^^^^

Special hints:

**SUBDIR**
    if your project contains more than one sub-directory to build, add
    these directories here. Keep in mind, these directories are visited
    in this order (but never in parallel), so you must handle
    dependencies manually.

**\*_CPPFLAGS / \*_CFLAGS / \*_LIBADD**
    if your library has some optional external dependencies add them on
    demand (external libraries for example). Keep in mind to not mix
    CPPFLAGS and CFLAGS additions. And do not add these additions fixed
    to the \*_CPPFLAGS and \*_CFLAGS variables, let ’configure’ do it
    in a sane way. Whenever you want to forward special things to the
    \*_CPPFLAGS and \*_CFLAGS, don’t forget to add the AM_CPPFLAGS
    and AM\_CFLAGS, else they get lost. Never add libraries to the
    \*_LDFLAGS variable. Always add them to the \*_LIBADD variable
    instead. This is important because the autotools forward all these
    variable based parameters in a specifc order to the tools (compiler
    and linker).

Template file for pkg-config
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

libfoo.pc.in
^^^^^^^^^^^^

This file gets installed to support the *pkg-config* tool for package
management. It contains some important information how to use your
library and also handles its dependencies.

Special hints:

**Name**
    A human-readable name for the library.

**Description**
    add a brief description of your library here

**Version**
    the main revision of the library. Will automatically replaced from
    your settings in ``configure.ac``.

**URL**
    where to find your library. Will automatically replaced from your
    settings in ``configure.ac``.

**Requires.private**
    comma-separated list of packages your library depends on and managed
    by pkg-config. The listed packages gets honored for the static
    linking case and should not be given again in the *Libs.private*
    line. This line will be filled by the *REQUIRES* variable from the
    ``configure.ac``.

**Conflicts**
    list of packages your library conflicts with. Will automatically
    replaced from your CONFLICTS variable settings in ``configure.ac``.

**Libs**
    defines the linker command line content to link your library against
    other applications or libraries

**Libs.private**
    defines the linker command line content to link your library
    against other application or libraries statically. List only
    libraries here which are not managed by pkg-config (e.g. do not
    conflict with packages given in the *Requires*).
    This line will be filled by the *LIBS* variable from the
    ``configure.ac``.

**Cflags**
    required compile flags to make use of your library. Unfortunately
    you must mix CPPFLAGS and CFLAGS here which is a really bad idea.

It is not easy to fully automate the adaption of the pc file. At least
the lines *Requires.private* and *Libs.private* are hardly to fill for
packages which are highly configureable.

Generic template files
~~~~~~~~~~~~~~~~~~~~~~

m4/*
^^^^

M4 macros used in ``configure.ac``.

If you use more no generic M4 macros in your ``configure.ac`` file,
don’t forget to add their source files to the m4 directory. This will
enable any user of your library to re-generate the autotools based files
without providing all dependencies by themself.

Library related files
~~~~~~~~~~~~~~~~~~~~~

README
^^^^^^

Prepared file with some information about the library you provide. Be
kind to the users of your library and write some sentences about basic
features and usage of your library, how to configure it and how to build
it.

libfoo.h
^^^^^^^^

This file will be installed. It defines the API your library provides
and will be used by other applications.

internal.h
^^^^^^^^^^

This file will not be installed. It will be used only at build time of
your library.

foo.c
^^^^^

The main source file of your library. Keep in mind to mark all functions
with the DSO_VISIBLE macro you want to export. All other functions are
kept internaly and you cannot link against them from an external
application.

Note: debugging is hard when all internal functions are hidden. For this
case you should configure the libary with the ``--disable-hide`` or with
``--enable-debug`` which includes switching off hiding functions.

Frequently Asked Questions (FAQ)
--------------------------------

Q: PTXdist does not support to generate some files in a way I need them. What can I do?
A: Everything PTXdist builds is controlled by “package rule files”,
which in fact are Makefiles (``rules/*.make``). If you modify such a
file you can change it’s behaviour in a way you need. It is generally
no good idea to modify the generic package rule files installed by
PTXdist, but it is always possible to copy one of them over into the
``rules/`` directory of a project. Package rule files in the project
will precede global rule files with the same name.

Q: My kernel build fails. But I cannot detect the correct position,
due to parallel building. How can I stop PTXdist to build in parallel?
A: Force PTXdist to stop building in parallel which looks somehow
like:

::

    $ ptxdist -j1 go

Q: I made my own rule file and now I get error messages like

::

    my_project/rules/test.make:30: *** unterminated call to function `call': missing `)'.  Stop.

But line 30 only contains ``@$(call targetinfo, $@)`` and it seems all
right. What does it mean?
A: Yes, this error message is confusing. But it usually only means
that you should check the following (!) lines for missing backslashes
(line separators).

Q: I got a message similar to “package <some name> is empty. not
generating.” What does it mean?
A: The ’ipkg’ tool was advised to generate a new ipkg-packet, but the
folder was empty. Sometime it means a typo in the package name when
the ``install_copy`` macro was called. Ensure all these macros are using
the same package name. Or did you disable a menuentry and now nothing
will be installed?

Q: How do I download all required packages at once?
A: Run this command prior the build:

::

    $ ptxdist make get

This starts to download all required packages in one run. It does
nothing if the archives are already present in the source path. (run
“PTXdist setup” first).

Q: I want to backup the source archives my PTXdist project relys on.
How can I find out what packages my project requires to build?
A: First build your PTXdist project completely and then run the
following command:

::

    $ ptxdist export_src <archive directory>

It copies all archives from where are your source archives stored to
<archive directory> which can be your backup media.

Q: To avoid building the OSELAS toolchain on each development host, I
copied it to another machine. But on this machine I cannot build any
BSP with this toolchain correctly. All applications on the target are
failing to start due to missing libraries.
A: This happens when the toolchain was copied without regarding to
retain links. There are archive programs around that convert links
into real files. When you are using such programs to create a
toolchain archive this toolchain will be broken after extracting it
again. Solution: Use archive programs that retain links as they are
(tar for example). Here an example for a broken toolchain:

::

    $ ll `find . -name "libcrypt*"`
    -rwxr-xr-x 1 mkl ptx 55K 2007-07-25 14:54 ./lib/libcrypt-2.5.so*
    -rwxr-xr-x 1 mkl ptx 55K 2007-07-25 14:54 ./lib/libcrypt.so.1*
    -rw-r--r-- 1 mkl ptx 63K 2007-07-25 14:54 ./usr/lib/libcrypt.a
    -rw-r--r-- 1 mkl ptx 64K 2007-07-25 14:54 ./usr/lib/libcrypt_p.a
    -rwxr-xr-x 1 mkl ptx 55K 2007-07-25 14:54 ./usr/lib/libcrypt.so*

And in contrast, this one is intact:

::

    $ ll `find . -name "libcrypt*"`
    -rwxr-xr-x 1 mkl ptx 55K 2007-11-03 13:30 ./lib/libcrypt-2.5.so*
    lrwxrwxrwx 1 mkl ptx  15 2008-02-20 14:52 ./lib/libcrypt.so.1 -> libcrypt-2.5.so*
    -rw-r--r-- 1 mkl ptx 63K 2007-11-03 13:30 ./usr/lib/libcrypt.a
    -rw-r--r-- 1 mkl ptx 64K 2007-11-03 13:30 ./usr/lib/libcrypt_p.a
    lrwxrwxrwx 1 mkl ptx  23 2008-02-20 14:52 ./usr/lib/libcrypt.so -> ../../lib/libcrypt.so.1*

Q: I followed the instructions how to integrate my own plain source
project into PTXdist. But when I try to build it, I get:

::

    extract: archive=/path/to/my/sources
    extract: dest=/path/to/my/project/build-target
    Unknown format, cannot extract!

But the path exists!
A: PTXdist interprets a ``file://`` (two slashes) in the URL as a
project related relative path. So it searches only in the current
project for the given path. Only ``file:///`` (three slashes) will
force PTXdist to use the path as an absolute one. This means:
``file://bla/blub`` will be used as ``./bla/blub`` and
``file:///friesel/frasel`` as ``/friesel/frasel``.

Q: I want to use more than one kernel revision in my BSP. How can I
avoid maintaining one ptxconfig per kernel?
A: One solution could be to include the kernel revision into the name
of the kernel config file. Instead of the default kernelconfig.target
name you should use ``kernelconfig-<revision>.target``. In the kernel
config file menu entry you should enter
``kernelconfig-$PTXCONF_KERNEL_VERSION.target``. Whenever you change
the linux kernel Version menu entry now, this will ensure using a
different kernel config file, too.

Q: I’m trying to use a JAVA based package in PTXdist. But compiling
fails badly. Does it ever work at Pengutronix?
A: This kind of packages only build correctly when an original SUN VM
SDK is used. Run PTXdist setup and point the Java SDK menu entry to
the installation path of your SUN JAVA SDK.

Q: I made a new project and everythings seems fine. But when I start my
target with the root filesystem generated by PTXdist, it fails with:

::

    cannot run '/etc/init.d/rcS': No such file or directory

A: The error message is confusing. But this script needs ``/bin/sh`` to
run. Most of the time this message occures when ``/bin/sh`` does not
exists. Did you enable it in your busybox configuration?

Q: I have created a path for my source archives and try to make PTXdist
use it. But whenever I run PTXdist now it fails with the following error
message:

::

    /usr/local/bin/ptxdist: archives: command not found

A: In this case the path was ``$HOME/source archives`` which includes a
whitespace in the name of the directory to store the source archives in.
Handling directory or filenames with whitespaces in applications isn’t
trivial and also PTXdist suffers all over the place from this issue. The
only solution is to avoid whitespaces in paths and filenames.

Q: I have adapted my own rule file’s targetinstall stage, but PTXdist
does not install the files. A: Check if the closing ``@$(call
install_finish, [...])`` is present at the end of the targetinsall stage.
If not, PTXdist will not complete this stage.
