# DUMA

---

[![license-gplv2](https://img.shields.io/badge/license-GPL%20v2-blue.svg)](https://github.com/johnsonjh/duma/blob/master/COPYING-GPL)
[![license-lgplv2.1](https://img.shields.io/badge/license-LGPL%20v2.1-blue.svg)](https://github.com/johnsonjh/duma/blob/master/COPYING-LGPL)
[![fossastatus](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma?ref=badge_shield)
[![codesize](https://img.shields.io/github/languages/code-size/johnsonjh/duma.svg)](https://github.com/johnsonjh/duma)
[![release](https://img.shields.io/github/release/johnsonjh/duma.svg)](https://github.com/johnsonjh/duma/releases/)
[![codacy](https://api.codacy.com/project/badge/Grade/f777934d666b4a6a9672d89b404c4953)](https://app.codacy.com/gh/johnsonjh/duma?utm_source=github.com&utm_medium=referral&utm_content=johnsonjh/duma&utm_campaign=Badge_Grade)
[![codebeat](https://codebeat.co/badges/a0be6809-acda-41a7-96a8-0d46698dc42c)](https://codebeat.co/projects/github-com-johnsonjh-duma-master)
[![deepscan](https://deepscan.io/api/teams/12184/projects/15170/branches/299583/badge/grade.svg)](https://deepscan.io/dashboard#view=project&tid=12184&pid=15170&bid=299583)
[![deepsource-active](https://deepsource.io/gh/johnsonjh/duma.svg/?label=active-issues)](https://deepsource.io/gh/johnsonjh/duma/?ref=repository-badge)
[![deepsource-resolved](https://deepsource.io/gh/johnsonjh/duma.svg/?label=resolved+issues)](https://deepsource.io/gh/johnsonjh/duma/?ref=repository-badge)
[![tickgit](https://img.shields.io/endpoint?url=https://api.tickgit.com/badge?repo=github.com/johnsonjh/duma)](https://www.tickgit.com/browse?repo=github.com/johnsonjh/duma)

---

## Detect Unintended Memory Access

A Red-Zone memory allocator

---

<!-- toc -->

- [DUMA](#duma)
  - [Detect Unintended Memory Access](#detect-unintended-memory-access)
    - [Description](#description)
    - [Installation](#installation)
      - [Binary Packages](#binary-packages)
      - [Building from source](#building-from-source)
        - [GNU Make](#gnu-make)
        - [CMake](#cmake)
        - [Visual Studio](#visual-studio)
      - [Environment Notes](#environment-notes)
        - [Solaris](#solaris)
        - [BSD](#bsd)
        - [ARM CPUs](#arm-cpus)
        - [MIPS CPUs](#mips-cpus)
        - [RISC-V CPUs](#risc-v-cpus)
    - [Usage](#usage)
      - [Static linking](#static-linking)
      - [Dynamic linking](#dynamic-linking)
      - [Recommended Usage](#recommended-usage)
      - [Global and Environment Variables](#global-and-environment-variables)
      - [Word-Alignment and Overrun Detection](#word-alignment-and-overrun-detection)
    - [Catching the Erroneous Line](#catching-the-erroneous-line)
      - [Live (debugger control)](#live-debugger-control)
      - [Post-mortem (core analysis)](#post-mortem-core-analysis)
    - [Debugging your Program](#debugging-your-program)
      - [General Debugging Instructions](#general-debugging-instructions)
      - [Word-Alignment and Overrun Debugging](#word-alignment-and-overrun-debugging)
    - [Memory Usage and Execution Speed](#memory-usage-and-execution-speed)
    - [Memory Leak Detection](#memory-leak-detection)
    - [C++ Memory Operators and Leak Detection](#c-memory-operators-and-leak-detection)
      - [Definition of own member new/delete Operators](#definition-of-own-member-newdelete-operators)
    - [Compilation Notes for Release/Production](#compilation-notes-for-releaseproduction)
    - [NO WARRANTY](#no-warranty)
    - [Diagnostics](#diagnostics)
      - [Bugs](#bugs)
    - [Comparison with other tools](#comparison-with-other-tools)
    - [Availability](#availability)
      - [Releases](#releases)
      - [Git Repositories](#git-repositories)
    - [Issue Tracking](#issue-tracking)
    - [Security Policy](#security-policy)
    - [Authors](#authors)
    - [License](#license)
    - [Version History](#version-history)

<!-- tocstop -->

---

### Description

**DUMA** helps you detect two of the most common programming errors:

1. Software that overruns the boundaries of a `malloc()` memory allocation,
2. Software that touches memory allocations already released by `free()`.

Unlike other `malloc()` debuggers, **DUMA** will detect read accesses as well as
writes, and it will pinpoint the exact instruction that causes an error.

**Electric Fence**, the predecessor of **DUMA**, has been in use at _Pixar_
since 1987, and at many other sites for years.

**DUMA** uses the virtual memory hardware of your computer to place an
inaccessible memory page immediately after (_or before, at the user's option_)
each memory allocation. When software reads or writes this inaccessible page,
the hardware issues a segmentation fault, stopping the program at the offending
instruction. It is then trivial to find the erroneous statement using your
favorite debugger. In a similar manner, memory that has been released by
`free()` is made inaccessible, and any code that touches it will get a
segmentation fault.

Simply linking your application with `libduma.a` will allow you to detect most,
but not all, `malloc()` buffer overruns and accesses of free memory. If you want
to be reasonably sure that you've found all catch-able bugs of this type, you'll
have to read and understand the rest of the documentation.

Besides catching these kind of memory bugs, **DUMA** also provides a means to
detect memory leaks. When using **DUMA** to pinpoint the source of a
memory-leak, some source modification is necessary - at the minimum, adding
`#include "duma.h"` to your source.

---

### Installation

**DUMA** may be installed from compiled binary packages or built
from source code.

---

#### Binary Packages

- [***TODO: Binary packages***]

- Packages available for installation via the distribution's package manager:\
  [![Available distro packages](https://repology.org/badge/vertical-allrepos/duma.svg?header=)](https://repology.org/project/duma/packages)

---

#### Building from source

- The latest release version of **DUMA** is available to download from
  [GitHub](https://github.com/johnsonjh/duma/releases/latest)
  and [other mirrors](https://github.com/johnsonjh/duma#releases).

  - _NOTE_: If the latest release version is more than a few dozen commits
    behind the master branch, you may wish to try the master branch instead
    of the last release. While there are no guarantees made (for any release
    of **DUMA**), best efforts are put forth to ensure the master branch
    is functional and passes tests on Linux and Solaris.
  - Pre-release versions are also available from
    [GitHub](https://github.com/johnsonjh/duma/releases) and others mirrors.

- You can use the _Git_ distributed version control tool to download
  the latest release version, the current development version, or the
  entire history, from any of several
  [source code repositories](https://github.com/johnsonjh/duma#git-repositories).

---

##### GNU Make

**DUMA** may be built using **_GNU_** **Make**.

**_GNU_** **Make** is available for many operating systems, including
_SunOS_ / _Solaris_, _GNU/Hurd_, _AIX_, _HP-UX_, _BSD_, _GNU/Linux_, and
_Microsoft Windows_ (using _Cygwin_ or _UWIN_ for POSIX compatibility, or
natively with the _MSYS_ / _MINGW_ environment.)

**_GNU_** **Make** is often installed as `gmake`, but may be installed under
a different name, such as `mingw32-make`, `mingw64-gmake`, or simply `make`
(_especially on_ **_GNU_** _systems such as Linux_).

Some **_non_**-**_GNU_** "**Make**" systems **_may_** work, but are
**_untested_** and **not** **recommended** for building **DUMA**.

- Inside the unpacked **DUMA** source directory, create and change to a
  new `build` directory
  - `mkdir build && cd build`
- (_Optionally_) review `GNUmakefile` for configuration, compilation,
  and installation options
  - Check the `OS` and `OSTYPE` options
    (_necessary if automated detection is unsuccessful_)
  - Check any other options or variables that may be available
- Build **DUMA**
  - `gmake -f ../GNUmakefile srcdir=../`
    (_for automatic OS detection and defaults; assumes gmake is GNU Make_)
  - `make -f ../GNUmakefile srcdir=../ OSTYPE=cygwin`
    (_for Microsoft Windows with Cygwin_)
  - `mingw32-make -f ../GNUmakefile srcdir=../ OSTYPE=msys`
    (_for Microsoft Windows with MSYS / MINGW via Command Prompt (CMD)_)
  - `make -f ../GNUmakefile srcdir=../ OS=linux`
    (_for most GNU/Linux systems_)
- Test **DUMA**
  - `gmake -f ../GNUmakefile check`
- Install **DUMA**
  - `gmake -f ../GNUmakefile install DESTDIR=/opt/duma`
    - It may be necessary to prefix this command with
      **_su_**, **_sudo_**, **_doas_**, _etc._ to elevate privileges,
      depending on the specified `DESTDIR` or `prefix`
- Test **DUMA** installation
  - `gmake -f ../GNUmakefile installcheck`

---

##### CMake

**CMake** is a portable suite of tools to automate the configuration,
building, testing, packaging, and installation of software.

**DUMA** may be built using **CMake** 3.1 (_December 2014_) or later.

- Inside the unpacked **DUMA** source directory, create and change to a
  new `build` directory
  - `mkdir build && cd build`
- (_Optionally_) interactively configure compilation and installation options
  - `ccmake ..` _or_
  - `ccmake-gui ..`
- Generate the necessary build files
  - `cmake ..`
    - You may also specify options non-interactively, for example
    - `cmake -DCMAKE_BUILD_TYPE="Debug" ..`
    - `cmake -DCMAKE_BUILD_TYPE="Release" ..`
    - `cmake -DCMAKE_INSTALL_PREFIX="/opt/duma" ..`
    - `cmake -DCMAKE_TOOLCHAIN_FILE=/opt/gcc/toolchain_gcc-8.cmake ..`
    - `cmake -G "Visual Studio 16 2019" -A "x64" ..`
      - _Refer to the
        [CMake Manual](https://cmake.org/cmake/help/latest/manual)
        for full details_
- Build **DUMA**
  - `cmake --build .`
- Test **DUMA**
  - `ctest` (_add_ `-V` _or_ `-VV` _for more verbose output_)
- Install **DUMA**
  - `cmake --build . --target "install"`
    - It may be necessary to prefix this command with
      **_su_**, **_sudo_**, **_doas_**, _etc._ to elevate privileges,
      depending on the configured `CMAKE_INSTALL_PREFIX`

---

##### Visual Studio

- Compile and execute `createconf`
  - Verify the `duma_config.h` header file generated by `createconf`
- Compile `dumalib`
  - Customize your project's `INCLUDE` and `LIBS` variables, as required
    for your environment

---

#### Environment Notes

**DUMA** should work out of the box on most systems, however, specific
environments may require additional configuration or warrant special
consideration.

---

##### Solaris

- Both _SPARC_ and _Intel_ processors are supported.

- **DUMA** is tested on _Solaris_ using the GNU toolchain
  (_GNU CC, G++, ld, binutils, etc._)

  - Older _Solaris_ systems, such as _Solaris 10_, using the GNU tools from
    the _Companion CD_ should add `/opt/sfw/bin` and `/opt/sfw/lib/bin` to
    the `PATH`.

  - Newer _Solaris_ systems, such as _Solaris 11.next_ or _OpenIndiana_,
    require similar configuration.

- The _Solaris Studio_ toolchain has not been tested.

---

##### BSD

- **FreeBSD**

  - On _FreeBSD_ 6.2, it is necessary to set `DUMA_DISABLE_BANNER=1`
    before running any program linked with **DUMA**.
  - NOTE: On _FreeBSD_ 7.0 and later, there appears to be an
    incompatability with the new libthr, which is currently being
    investigated.

---

- **NetBSD**

  - On _NetBSD_ 3.1 (`HOSTTYPE=i386` / `OSTYPE=netbsdelf`), one (`1`)
    memory leak is always detected.

  - Installation on _NetBSD_ is untested.

---

##### ARM CPUs

- On systems with _ARM_ processors, it may be necessary to set the
  `DUMA_MEMCPY_OVERLAP` variable, which allows overlapping of `memcpy()`
  regions when the destination address is less than the source address.

---

##### MIPS CPUs

- On systems with 64-bit _MIPS_ processors (_MIPS_-_III_ / _IV_ / _V_, _MIPS64_,
  _КОМДИВ-64_, _OCTEON_, _VR43xx_, _ICE9_), it may be necessary to explicitly
  set the `DUMA_ALIGNMENT` environment variable to eight (`8`) bytes.

---

##### RISC-V CPUs

- On systems using _RISC-V_ processors, the base instruction set uses only
  fixed-length 32-bit instructions that must be aligned on four (`4`) byte
  (_32-bit_) boundaries.

- However, the standard _RISC-V_ encoding scheme supports _compressed_
  and _variable-length_ instructions. These instructions can be any
  number of 16-bit _"instruction parcels"_ in length, and are always
  aligned on two (`2`) byte (_16-bit_) boundaries.

- The _RISC-V_ processor will not fault when accessing an invalid (_unaligned_)
  address. Instead, it will simply use the next valid address. The `DUMA_ALIGNMENT`
  environment variable should be set to a value greater than one (`1`)
  on systems using _RISC-V_ processors.

---

### Usage

#### Static linking

- Link your program with the library `libduma.a`. Make sure you are not linking
  with `-lmalloc`, `-lmallocdebug`, or with other `malloc()` debugger or
  enhancer libraries. You can only use one at a time.

- If your system administrator has installed **DUMA** for public use, you'll be
  able to use the `-lduma` argument to the linker, otherwise you'll have to put
  the path-name for `libduma.a` in the linker's command line.

---

#### Dynamic linking

- If you're using a Bourne-style shell on
  most _UNIX_ systems, the statement `export LD_PRELOAD=libduma.so` will cause
  **DUMA** to be loaded to run all dynamic executables.

- For _Darwin_-based
  systems such as macOS X, use `export DYLD_INSERT_LIBRARIES=libduma.dylib`
  and `export DYLD_FORCE_FLAT_NAMESPACE=1`.

- The included helper command `duma <command>` will run a single command
  under **DUMA**.

- For more details, review the documentation for the dynamic linking
  facility provided by your operating system vendor.

---

#### Recommended Usage

- Some systems will require special arguments to the linker to assure that you
  are using the **DUMA** `malloc()` and not the one from your C library.

- Run your program using a debugger. It's easier to work this way than to create
  a core file and post-mortem debug it. **DUMA** can create huge core files, and
  some operating systems will thus take minutes simply to dump core! Some
  operating systems will not create usable core files from programs that are
  linked with **DUMA**.

- If your program has one of the errors detected by **DUMA**, it will get a
  segmentation fault (`SIGSEGV`) at the offending instruction. Use the debugger
  to locate the erroneous statement, and repair it.

---

#### Global and Environment Variables

**DUMA** has several configuration switches that can be enabled via the shell
environment. These switches change what bugs **DUMA** will detect, so it's
important that you know how to use them.

- You can use the `gdb` command `'set environment variable value'` to set shell
  environment variables only for the program you are going to debug. This is
  useful especially if you are using the shared **DUMA** library.

- `DUMA_ALIGNMENT` - This is an integer that specifies the alignment for any
  memory allocations that will be returned by malloc(), calloc(), and realloc().
  The value is specified in bytes, thus a value of 4 will cause memory to be
  aligned to 32-bit boundaries unless your system doesn't have a 8-bit
  characters. `DUMA_ALIGNMENT` is set to the minimum required alignment specific
  to your environment by default. The minimum required alignment is detected by
  `createconf` and stored in the file `duma_config.h`.

  If your program requires that allocations be aligned to 64-bit boundaries
  you'll have to set this value to 8. This is the case when compiling with the
  `-mips2` flag on _MIPS_-based systems such as those from _SGI_. For some
  architectures the default is defined to even more - x86‑64 uses alignment to
  16 bytes by default.

  **DUMA** internally uses a smaller value if the requested memory size is
  smaller than the alignment value: the next smaller power of 2 is used.

  Thus allocating blocks smaller than `DUMA_ALIGNMENT` may result into smaller
  alignments - for example when allocating 3 bytes, they would be aligned to 2
  byte boundary. This allows better detection of overrun.

  For this reason, you will sometimes want to set `DUMA_ALIGNMENT` to `1` (no
  alignment), so that you can detect overruns of less than your CPU's word size.
  Be sure to read the section '_Word-Alignment and Overrun Detection_' in this
  manual page before you try this.

  To change this value, set `DUMA_ALIGNMENT` in the shell environment to an
  integer value, or call the macro function `DUMA_SET_ALIGNMENT()` from your
  code.

  You don't need to change this setting, if you just need bigger alignment for
  some special buffers. In this case you may use the function
  `memalign(alignment, userSize)`.

- `DUMA_PROTECT_BELOW` - **DUMA** usually places an inaccessible page
  immediately after each memory allocation, so that software that runs past the
  end of the allocation will be detected. Setting `DUMA_PROTECT_BELOW` to `1`
  causes **DUMA** to place the inaccessible page before the allocation in the
  address space, so that under-runs will be detected instead of over-runs.

  To change this value, set `DUMA_PROTECT_BELOW` in the shell environment to an
  integer value, or call the macro function `DUMA_SET_PROTECT_BELOW()` from your
  code.

- `DUMA_SKIPCOUNT_INIT` - **DUMA** usually does its initialization with the
  first memory allocation. On some systems this may collide with initialization
  of pthreads or other libraries and produce a hang. To get **DUMA** work even in
  these situations you can control (with this environment variable) after how
  many allocations the full internal initialization of **DUMA** is done. Default
  is `0`.

- `DUMA_REPORT_ALL_LEAKS` - **DUMA** usually reports only memory leaks where the
  source file-name with line number of the allocating instruction is known.
  Setting this variable to `1` in shell environment reports all memory leaks.
  The default is `0` to avoid reporting of irrelevant memory leaks from
  system/compiler environment: there are many standard libraries leaking memory,
  which by default is no real problem as the system frees up all memory on
  program exit.

- `DUMA_FILL` - When set to a value between `0` and `255`, every byte of
  allocated memory is initialized to that value. This can help detect reads of
  uninitialized memory. When set to `-1`, **DUMA** does not initialize memory on
  allocation, so some memory may filled with zeroes (the operating system
  default on most systems) and some memory will retain the values written to it
  during its last use.

  Per default, **DUMA** will initialize all allocated bytes to `255` (`0xFF`).
  To change this value, set `DUMA_FILL` in the shell environment to an integer
  value, or call the macro function `DUMA_SET_FILL()` from your code.

- `DUMA_SLACKFILL` - As **DUMA** internally allocates memory in whole pages,
  there retains an unused and unprotectable piece of memory: the slack or
  _no-mans-land_. Per default **DUMA** will initialize this area to `170`
  (`0xAA`), which is `10101010` in binary representation.

  To change this value, set `DUMA_SLACKFILL` in the shell environment to an
  integer value.

  **DUMA** automatically checks this area, the _no-mans-land_, at deallocation.
  You can manually induce a check with the macro function `DUMA_CHECK()` for one
  memory block. With the macro function `DUMA_CHECKALL()` all memory blocks get
  checked.

- `DUMA_CHECK_FREQ` - First see `DUMA_SLACKFILL` above for definition of
  _no-mans-land_. Checking the integrity of the*no-mans-land* costs
  performance. This is why this is usually done only at deallocation of a memory
  block. Set this variable to let **DUMA** check all memory blocks
  _no-mans-land_ every *value*th allocation or deallocation. Set this variable
  to 1, to let **DUMA** check at each allocation and deallocation.

  Per default the value `0` is used, which means to check only at deallocation.

- `DUMA_ALLOW_MALLOC_0` - Memory allocation of size zero is _ANSI_ conforming,
  but, often this is the result of a software bug. For this reason **DUMA** may
  trap such calls to malloc() with size zero. I leave this option disabled by
  default, but you are free to trap these calls setting the
  `DUMA_ALLOC_MALLOC_0` in the shell environment to an integer value.

- `DUMA_MALLOC_0_STRATEGY` - This environment variable controls **DUMA**'s
  behavior on `malloc(0)`:

  - `0` - abort program with segfault (previously `ALLOW_MALLOC_0 = 0`)
  - `1` - return NULL pointer
  - `2` - return always the same pointer to some protected page
  - `3` - return mid address of a unique protected page (**_default_**)

    **_ATTENTION_**: Only `1` and `3` are _ANSI_ conforming. But value `1`
    will break most programs, and value `3` strategy most system libraries
    use/implement. All returned pointers can be passed to `free()`.

- `DUMA_NEW_0_STRATEGY` - This environment variable controls **DUMA**'s
  behavior on C++ operator new with size zero:

  - `2` - return always the same pointer to some protected page
  - `3` - return mid address of a unique protected page (**_default_**)

    **_ATTENTION_**: Only `3` is standard conforming. Value `2` may break
    some, but will work for most programs. With value `2` you may reduce the
    memory consumption.

- `DUMA_MALLOC_FAILEXIT` - Many programs do not check for allocation failure.
  This often leads to delayed errors, no more understandable. Set this variable
  to a positive integer in the shell environment to exit the program immediately
  when memory allocation fails. This option is set by default.

- `DUMA_PROTECT_FREE` - **DUMA** usually returns free memory to a pool from
  which it may be re-allocated. If you suspect that a program may be touching
  free memory, set `DUMA_PROTECT_FREE` shell environment to `-1`. This is the
  default and will cause **DUMA** not to re-allocate any memory.

  For programs with many allocations and de-allocations this may lead to the
  consumption of the full address space and thus to the failure of `malloc()`.
  It is important to discriminate between _address space_ and _physical memory_;
  **DUMA** does free the _physical memory_; but the _address space_ is not
  freed. Thus, the _address space_ may be exhausted despite available _physical
  memory_. This is especially important on 32-bit systems. To avoid such
  failures, you may limit the amount of protected de-allocated memory by setting
  `DUMA_PROTECT_FREE` to a positive value. This value in **kB** will be the
  limit for such protected free memory.

  A value of `0` will disable protection of freed memory.

- `DUMA_MAX_ALLOC` - This shell environment variable limits the total memory
  print of a program. This is another way to indirectly limit the sum of freed
  protected memory (see `DUMA_PROTECT_FREE`). By default there is no limit
  (`-1`). A positive value is interpreted in **kB**, which stands for the sum of
  allocated and freed protected memory.

- `DUMA_FREE_ACCESS` - This is a debugging enhancer to catch deallocation of a
  memory block using watch expressions. **DUMA** does a write access to the
  first byte, which may lead a debugger to stop on a watch expression. You have
  to enable this by setting the shell environment variable to non zero. Default
  is disabled.

- `DUMA_SHOW_ALLOC` - Set this shell environment variable to non-zero to let
  DUMA print all allocations and de-allocations to the console. Although this
  generates a lot of messages, this option can be useful to detect inefficient
  code containing many (de)allocations. This is switched off by default.

- `DUMA_SUPPRESS_ATEXIT` - Set this shell environment variable to non-zero when
  DUMA should skip the installation of its exit handler. The exit handler is
  called at the end of the main program and checks for memory leaks, so the
  handler's installation should **_usually_** not be suppressed. One reason for
  doing so regardless are some buggy environments, where calls to the standard C
  library's `atexit()`-function hangs.

- `DUMA_DISABLE_BANNER` - Set this shell environment variable to non-zero to
  suppress the usual start-up message on console. Default is `0`.

- `DUMA_OUTPUT_DEBUG` - Set this shell environment variable to non-zero to
  output all DUMA messages to the debugging console. This option is only
  available on Windows and is off by default.

- `DUMA_OUTPUT_STDOUT` - Set this shell environment variable to non-zero to
  output all DUMA messages to _STDOUT_. This option is off by default.

- `DUMA_OUTPUT_STDERR` - Set this shell environment variable to non-zero to
  output all DUMA messages to _STDERR_. This option is on by default.

- `DUMA_OUTPUT_FILE` - Set this shell environment variable to a file-name where
  all DUMA messages should be written to. This option is off by default.

- `DUMA_OUTPUT_STACKTRACE` - Set this shell environment variable to non-zero to
  output a stacktrace of the allocation that is not freed. This option is
  available only on Windows and is off by default. This option also requires a
  map file generated by the linker.

- `DUMA_OUTPUT_STACKTRACE_MAPFILE` - Set this shell environment variable to the
  map file, when it isn't found. This is very useful when using detours version
  of DUMA. This option is available only on Windows.

- `DUMA_MEMCPY_OVERLAP` - Set this shell environment variable to allow
  overlapping of memcpy regions if the destination address is less than source
  address. (workaround for _ARM_ `memmove`/`memcpy` implementation).

---

#### Word-Alignment and Overrun Detection

There is a conflict between the alignment restrictions that `malloc()` operates
under and the debugging strategy used by **DUMA**. When detecting overruns,
**DUMA** `malloc()` allocates two or more virtual memory pages for each
allocation. The last page is made inaccessible in such a way that any read,
write, or execute access will cause a segmentation fault. Then, **DUMA**
`malloc()` will return an address such that the first byte after the end of the
allocation is on the inaccessible page. Thus, any overrun of the allocation will
cause a segmentation fault.

It follows that the address returned by `malloc()` is the address of the
inaccessible page minus the size of the memory allocation. Unfortunately,
`malloc()` is required to return _word-aligned_ allocations, since many CPUs can
only access a word when its address is aligned. The conflict happens when
software makes a memory allocation using a size that is not a multiple of the
word size, and expects to do word accesses to that allocation. The location of
the inaccessible page is fixed by hardware at a _word-aligned_ address. If
**DUMA** `malloc()` is to return an aligned address, it must increase the size
of the allocation to a multiple of the word size.

In addition, the functions `memalign()` and `valloc()` must honor explicit
specifications on the alignment of the memory allocation, and this, as well can
only be implemented by increasing the size of the allocation. Thus, there will
be situations in which the end of a memory allocation contains some padding
space, and accesses of that padding space will not be detected, even if they are
overruns.

**DUMA** provides the variable `DUMA_ALIGNMENT` so that the user can control the
default alignment used by `malloc()`, `calloc()`, and `realloc()`. To debug
overruns as small as a single byte, you can set `DUMA_ALIGNMENT` to `1`. This
will result in **DUMA** `malloc()` returning unaligned addresses for allocations
with sizes that are not a multiple of the word size. This is not a problem in
most cases, because compilers must pad the size of objects so that alignment
restrictions are honored when storing those objects in arrays. The problem
surfaces when software allocates odd-sized buffers for objects that must be
_word-aligned_. One case of this is software that allocates a buffer to contain
a structure and a string, and the string has an odd size (this example was in a
popular _TIFF_ library).

If word references are made to un-aligned buffers, you will see a bus error
(`SIGBUS`) instead of a segmentation fault. The only way to fix this is to
re-write the offending code to make byte references or not make odd-sized
allocations, or to set `DUMA_ALIGNMENT` to the word size.

Another example of software incompatible with `DUMA_ALIGNMENT` set less than
the system _word-size_ is the `strcmp()` function and other string functions
on _SunOS_ 4, which make _word-sized_ accesses to character strings, and may
attempt to access up to three bytes **_beyond_** the end of a string. These
result in a segmentation fault (`SIGSEGV`). The only way around this is to use
versions of the string functions that perform _byte references_ instead of
_word references_.

---

### Catching the Erroneous Line

Using **DUMA** - and one of the following strategies - you can determine the
offending line(s) of your source code responsible for causing an error.

#### Live (debugger control)

1. Compile your program with debugging information and statically linked to
   DUMA. On some systems, including some Linux distributions, the linking order
   is crucial - **DUMA** must be the last library passed to the linker.
2. Start your program from debugger e.g. with `gdb <program>`
3. Set program environment variables such as
   `'set environment DUMA_PROTECT_BELOW 1'`
4. Set your program arguments with `'set args …'`
5. Run and wait for the segmentation fault

#### Post-mortem (core analysis)

1. Compile your program (**_with_** debugging information).
2. Set `ulimit -c unlimited` to get core files
3. Start your program, choose one of following options
   - Start your program (linked **_statically_** with **DUMA**)
   - Start your program with `duma.sh <your_program>`
4. Wait for a segmentation fault. This should have created a `core[.<pid>]`
   file, which you can examine (i.e. `gdb <program> -c <core file>`)

---

### Debugging your Program

#### General Debugging Instructions

1. Link with `libduma.a` as explained above, ensuring proper linking order.
2. Run your program in a debugger and fix any overruns or accesses to free
   memory.
3. Quit the debugger.
4. Set `DUMA_PROTECT_BELOW = 1` in the shell environment.
5. Repeat step 2, this time repairing under-runs if they occur.
6. Quit the debugger.
7. Optionally, read and install `gdbinit.rc` as `~/.gdbinit` if you are using
   the `gdb` debugger

#### Word-Alignment and Overrun Debugging

- See if you can set `DUMA_ALIGNMENT` to `1`, and repeat step 2.
  - Sometimes this will be too much work, or there will be problems with library
    routines for which you don't have the source, that will prevent you from
    doing this.

---

### Memory Usage and Execution Speed

- Since **DUMA** uses at least two virtual memory pages for each of its
  allocations, it's a terrible memory hog. It may be necessary to configure a
  swap file so the system will have enough virtual memory available. Also, the
  way **DUMA** manipulates memory results in various cache and translation
  buffer entries being flushed with each call to `malloc()` or `free()`. The end
  result is that your program will execute slower and use more resources while
  you are debugging it with **DUMA**.

  - The Linux kernel may limit the number of page mappings per process. See
    `/proc/sys/vm/max_map_count`. You may have to increase this value to allow
    debugging with **DUMA** with a command such as:
    `sysctl -w vm.max_map_count=1000000`

- **Don't** leave `libduma.a` enabled and linked in production software. Use it
  only for debugging. See the section '_Compilation Notes for
  Release / Production_' below.

---

### Memory Leak Detection

- All memory allocation is protocol-ed from **DUMA** together with the file-name
  and line number of the calling function. The `atexit()` function checks if each
  allocated memory block was freed. To disable leak detection add the
  preprocessor definition `DUMA_SO_NO_LEAKDETECTION` or
  `DUMA_LIB_NO_LEAKDETECTION` to `DUMA_OPTIONS` in the Makefile.

  - If a leak is reported without a source file-name or line number, but is
    reproducible with the same pointer, set a conditional breakpoint on the
    function `void * duma_alloc_return( void * address)`, for example, using the
    **gdb** command `'break duma_alloc_return if address==0x123'`

---

### C++ Memory Operators and Leak Detection

- Macros for "`new`" and "`delete`" are defined in `dumapp.h`. These macros give
  file-name and line number of the calling functions to **DUMA**, thus allowing
  the same leak detection reports as for `malloc()` and `free()`. `dumapp.h` needs to be
  included from your source file(s).

  - To disable the C++ `new`, `delete`, `new[]`, and `delete[]` operators, add
    the preprocessor definition `DUMA_NO_CPP_SUPPORT` to `DUMA_OPTIONS` in
    Makefile.

---

#### Definition of own member new/delete Operators

- Definition of own member `new`/`delete` operators for a class will fail
  because the `new`/`delete` keywords are defined as macros from **DUMA**. You
  will have to un-define **DUMA**'s macros with following line:
  `#include "noduma.h"` Then you have to call **DUMA**'s operators directly
  inside your own definition.

- For using **DUMA**'s C++ operators without having the preprocessor macros
  defined, following syntax can be used:

```c++
// const char * file  or  __FILE__ macro
// int          line  or  __LINE__ macro

// scalar new throwing bad_alloc() on error
ptr = new(file,line) type;

// scalar new returning 0 on error
ptr = new(std::nothrow,file,line) type;

// scalar delete
operator delete(ptr,file,line);

// vector new throwing bad_alloc() on error
ptr = new(file,line) type[n];

// vector new returning 0 on error
ptr = new(std::nothrow,file,line) type[n];

// vector delete
operator delete[](ptr, file,line);
```

---

### Compilation Notes for Release/Production

- Set the preprocessor definition `#define DUMA_NO_DUMA` in your Makefiles to
  disable **DUMA** usage - and don't link with **DUMA** library. With
  `DUMA_NO_DUMA` defined, all **DUMA** macro functions get defined, but do
  nothing.

  - This way, you don't have to change your code for release compilation, even
    when using special **DUMA** macros.

---

### NO WARRANTY

- This software has **_NO WARRANTY_**.

  - **_See the [license](#license) for complete details._**

- Much effort has been expended to ensure the correctness of **DUMA**,
  however, it is (in all likelihood) not theoretically possible to make
  software of this nature **_bug-free_**.

  - **DUMA** **_may not_** report all bugs that exist in your code.

  - It may also **_incorrectly_** report bugs that do not exist.

---

### Diagnostics

- **_Segmentation Fault_**: Examine the offending statement for violation of the
  boundaries of a memory allocation.

- **_Bus Error_**: See the section on _'Word-Alignment and Overrun Detection'_
  in this manual.

---

#### Bugs

- Explanation of alignment issues could be improved.

- Some **Sun** systems running **SunOS** **4.1** were reported to signal an
  access to a protected page with `SIGBUS` rather than `SIGSEGV`. This is
  most likely an undocumented "_feature_" of particular _Sun_ hardware
  versions, and not an operating system anomaly.
  - On these systems **DUMA** tests will fail with a _bus error_ unless the
    `Makefile` is modified to define `PAGE_PROTECTION_VIOLATED_SIGNAL`
    as `SIGBUS`.

---

### Comparison with other tools

- **_[pending]_**

---

### Availability

#### Releases

- [GitHub Releases](https://github.com/johnsonjh/duma/releases)

#### Git Repositories

**_NOTE_**: _All source repositories are mirrors with identical contents._

- [GitHub](https://github.com/johnsonjh/duma)
- [GitLab](https://gitlab.com/johnsonjh/duma)
- [SourceHut](https://sr.ht/~trn/duma)
- [NotABug](https://notabug.org/trn/duma)

---

### Issue Tracking

- [GitHub Issue Tracker](https://github.com/johnsonjh/duma/issues)

---

### Security Policy

- [Security Policy and Vulnerability Reporting](https://github.com/johnsonjh/duma/security/policy)

---

### Authors

```text
 Copyright © 2020‑2022 Jeffrey H. Johnson <trnsz@pobox.com>
 Copyright © 2006 Michael Eddington <meddington@gmail.com>
 Copyright © 2002‑2021 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 Copyright © 1987‑1999 Bruce Perens <bruce@perens.com>
```

---

### License

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[![FOSSASmallStatus](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma.svg?type=small)](https://app.fossa.com/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma?ref=badge_small)

- [GNU General Public License v2.0 (GPLv2)](https://tldrlegal.com/license/gnu-general-public-license-v2)

- [GNU Lesser General Public License v2.1 (LGPLv2.1)](<https://tldrlegal.com/license/gnu-lesser-general-public-license-v2.1-(lgpl-2.1)>)

---

### Version History

- [Version History](https://github.com/johnsonjh/duma/blob/master/CHANGELOG.md)

---
