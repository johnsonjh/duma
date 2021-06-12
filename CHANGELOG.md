# Version History

<!-- toc -->

- **Unreleased**
  - [2.6xx](#26xx)
- **2021**
  - [2.5.21](#2521-2021-02-14)
  - [2.5.20](#2520-2021-01-04)
  - [2.5.19](#2519-2021-01-02)
- **2020**
  - [2.5.18](#2518-2020-12-18)
  - [2.5.17](#2517-2020-12-16)
- **2009**
  - [2.5.16](#2516-2009-06-14)
  - [2.5.15](#2515-2009-04-11)
- **2008**
  - [2.5.14](#2514-2008-04-27)
  - [2.5.13](#2513-2008-03-19)
  - [2.5.12](#2512-2008-01-25)
  - [2.5.11](#2511-2008-01-20)
  - [2.5.10](#2510-2008-01-15)
- **2007**
  - [2.5.9](#259-2007-12-23)
  - [2.5.8](#258-2007-08-19)
  - [2.5.7](#257-2007-08-17)
  - [2.5.6](#256-2007-08-15)
  - [2.5.5](#255-2007-08-11)
  - [2.5.4](#254-2007-07-31)
  - [2.5.3](#253-2007-07-15)
  - [2.5.2](#252-2007-07-12)
  - [2.5.1](#251-2007-07-08)
- **2006**
  - [2.5.0](#250-2006-07-01)
  - [2.4.28](#2428-2006-06-18)
  - [2.4.27](#2427-2006-04-23)
- **2005**
  - [2.4.26](#2426-2005-10-28)
  - [2.4.25](#2425-2005-10-21)
  - [2.4.24](#2424-2005-10-10)
  - [2.4.23](#2423-2005-10-08)
  - [2.4.22](#2422-2005-10-06)
  - [2.4.21](#2421-2005-09-29)
  - [2.4.20](#2420-2005-09-27)
  - [2.4.19](#2419-2005-09-20)
  - [2.4.18](#2418-2005-09-16)
  - [2.4.17](#2417-2005-09-10)
  - [2.4.16](#2416-2005-08-28)
  - [2.4.15](#2415-2005-07-24)
  - [2.4.14](#2414-2005-07-15)
  - [2.4.13](#2413-2005-02-11)
  - [2.4.12](#2412-2005-01-05)
  - [2.4.11](#2411-2005-01-03)
- **2004**
  - [2.4.10](#2410-2004-07-21)
- **2002**
  - [2.4.9](#249-2002-12-05)
  - [2.4.8](#248-2002-12-04)
  - [2.4.7](#247-2002-11-22)
  - [2.4.6](#246-2002-11-18)
  - [2.4.5](#245-2002-11-17)
  - [2.4.4](#244-2002-11-03)
  - [2.4.3](#243-2002-10-08)
  - [2.4.2](#242-2002-10-07)
  - [2.4.1](#241-2002-10-06)
  - [2.4.0](#240-2002-10-03)
  - [2.2.2](#222)
  - [2.2.0](#220)
- **1995**
  - [2.0.5](#205-1995-01-20)
- **1994**
  - [2.0.4](#204-1994-09-30)
- **1993**
  - [2.0.3](#203-1993-05-29)
- **1992**
  - [2.0.2](#202)
  - [2.0.1](#201)

<!-- tocstop -->

## 2.6xx

- Update `SECURITY.md`
  - Clarify purpose of document, update, reformat, add _PGP_ details
- Reorganization of files in source tree
- Update shell scripts to improve styling (`shfmt`), tune redirection, add
  pedantic error checking, better POSIX compliance, and appease ShellCheck
- Update `GNUmakefile`
  - Support out-of-tree compilation
  - Remove `shar` target
  - Fix `dos2unix` target
  - Use C++98 compilation mode w/G++
  - Fix macOS X default installation prefix
    - also, allow user to override default prefix
  - Remove `a.out` during `gmake clean`
- Update `duma.c`: Adjust ordering of included headers
- New CMake build support
  - Generate `duma.sh` to use absolute path to installed shared library
  - Use C++98 standard mode if available
  - Explicitly request position independent code
- Fix compilation warnings with GCC 9.3
- Fix compilation on macOS X (ARM64 and Intel processors)
- Create _Attic_ for outdated / deprecated components
- Overhaul and consolidate documentation into `README.md`
  - Optimize for both web browser and plain-text viewing
  - Integrate `README.txt` and `INSTALL` information
- Normalize source code styling (`clang-format` + `uncrustify`)
- Convert `CHANGELOG` to Markdown
  - Normalize format, fix spelling, wording, add dates
  - Squash details of generated `CHANGELOG.md`
- Switch to `git-changelog` generated `CHANGELOG`

## 2.5.21 (2021-02-14)

- Update contact information and documentation
  - No functional changes

## 2.5.20 (2021-01-04)

- Typo fixes and build system updates
  - No functional changes

## 2.5.19 (2021-01-02)

- Other minor changes
- Updated shell scripts
- Fixed _Debian_ release info

## 2.5.18 (2020-12-18)

- Updated documentation

## 2.5.17 (2020-12-16)

- Updated `README` with extra history and information
  (2020-12-15, JJ)
- Added `SECURITY.md`: Add security policy and information for bug reporting
  (2020-12-15, JJ)
- Added `LICENSE`: Merged `COPYING-GPL` and `COPYING-LGPL`
  (2020-08-25, JJ)
- Updated `TODO` with items from 2009-12-30 by _Daniel Trebbien_
  (2020-08-25, JJ)
- Updated `TODO` with items from 2008-05-23 by _Michael Eddington_
  (2020-08-25, JJ)
- No longer build test programs (_for now_) to un-break static builds
  (2020-08-24, JJ)
- Applied various update patches from the _Buildroot Project_
  (2020-10-28, JJ)
- Applied a patch from _Swati Kumari_ for additional `NULL` checks
  (2015-07-30, JJ)
- Applied a patch for bug ID _3035145_ from _Reinhard Katzmann_
  (2010-07-26, JJ)
- Applied a patch for bug ID _2923784_ from _Daniel Trebbien_
  (2009-12-30, JJ)
- Applied a patch for bug ID _2923779_ from _Daniel Trebbien_
  (2009-12-30, JJ)
- Applied a patch for bug ID _2831348_ from an anonymous SourceForge user
  (2009-07-23, JJ)

## 2.5.16 (2009-06-14)

- Add `memmove()` implementation based on `memove()` from `klibc` by _Andris Zeila_:
  http://sf.net/tracker/?func=detail&aid=2792252&group_id=149725&atid=775376
  (2009-05-21, HA)
- Fixed "_Broken build in MSYS_" and "_C++ tests fail_"
  - Fixed `USE_WIN32_CRIT_SECT` implementation in `sem_inc.c`
  - Added _make_ section for `OSTYPE=msys-sh`, which uses _mingw32-gcc_ / _G++_
- Use `USE_WIN32_CRIT_SECT` implementation for _Cygwin_, fixes `testoperator`
  (2009-05-23, HA)
- Remove CVS sub-directories from packaging
- Remove `debian` sub-directory from CVS
  (2009-06-07, HA)
- Fixed "Wrong `LD_PRELOAD` path in `duma.sh`". see
  https://sf.net/tracker/?func=detail&aid=2800139&group_id=149725&atid=775376
  (2009-06-12, HA)
- Fixed "_GNUmakefile is missing DESTDIR_" using submitted patch:
  https://sf.net/tracker/?func=detail&aid=2800137&group_id=149725&atid=775376
- Moved definition of `MAN_INSTALL_DIR` and `DOC_INSTALL_DIR` in `GNUmakefile`
  after definition of `prefix`
  (2009-06-15, HA)
- Removed `GNUmakefile` option `DUMA_SEMAPHORES`
  (2009-06-14, HA)

## 2.5.15 (2009-04-11)

- Added alternative locking implementation in `sem_inc.c` using critical
  sections. Code from _Peter Harris_, see:
  http://code.google.com/p/electric-fence-win32/
- Added _Windows_ threading to `testmt.c`
- Added _win32-msvc.net_ project file
- Use of `WIN32_SEMAPHORES` on _Win32_ / _Cygwin_ in `sem_inc.c`
  - The locking mechanism for multi-threading, with this configuration
    `testmt.c` works with pthreads or the _Win32_ API
- `CreateSemaphore()` now used, _maximum count = initial count = 1_
  (2008-08-03, HA)
- Removed usage of `strlen(`) in `strncpy()`
  (2009-03-19, HA)
- Add patch from [Andre Landwehr](mailto:andrel@cybernoia.de) which fixes
  a race condition when using the preprocessor macro `delete`
  (2009-04-07, HA)
- Fix bug in `strncpy()`: _Roman Jokl_ reported the bug, error check was
  too rigorous
  (2009-04-11, HA)

## 2.5.14 (2008-04-27)

- Added suggested `~/.gdbinit` as `gdbinit.rc`
- Added hint in `mprotectFailed()`
- Fix _GCC_ compiler warnings
- Rename of `sem_inc.h` to `duma_sem.h`, cause needed by `dumapp.h`
  (2008-03-12, HA)
- Update of `README` for _Microsoft_'s `pageheap.exe`, a heap debugging tool
  (2008-04-20, HA)
- Add patch from [Philip Mucci](mailto:mucci@cs.utk.edu) on behalf
  of _SiCortex_
  - Fix for `malloc(0)` which previously always returned `NULL`
  - Added environment switch `DUMA_MALLOC_0_STRATEGY`, which may return a
    special protected piece of memory for `malloc(0)`
  - Moved `malloc` / `SHOW_ALLOC` print outputs before all abort checks
    (2008-04-20, Philip Mucci, HA)
- Remove some warnings for `-Wall -Wextra`, especially for _Cygwin_
- Add detail to some messages when source file name and line number are not available
  (2008-04-21, HA)
- Removed environment variable `DUMA_ALLOW_MALLOC_0`,
  `DUMA_MALLOC_0_STRATEGY = 0` is equivalent to previous
  `DUMA_ALLOW_MALLOC_0 = 0`, default is `3`
  (2008-04-22, HA)
- Return unique pointer for C++ operator `new` for `DUMA_MALLOC_0_STRATEGY = 3`.
  Now, it should really be standards conforming.
  (2008-04-26, HA)
- Fix for C++ operator `new` with size zero (`0`)
- Added environment switch `DUMA_NEW_0_STRATEGY`
  (2008-04-27, HA)

## 2.5.13 (2008-03-19)

- Apply patches from [Philip Mucci](mailto:mucci@cs.utk.edu)
  of [SiCortex](http://sicortex.com):
  - Fix for `pthread_self()` and hang of `testoperators` test case on 64-bit
    _Linux_ platforms (_x86_64_ and _MIPS64_)
  - Added `-Wall` to Makefile
  - Properly version symbolic library to have the version triple (`0.0.0`)
  - Removed `Control-M`'s from the source files
    - Please do not ship _UNIX_ sources with `Control-M` line endings
  - Make sure `ranlib` is run on the static library, various platforms
    require this
  - Add `-DPIC` for shared library builds
  - Added install targets for headers (`INC_INSTALL_DIR`)
  - Added install targets for documentation (`DOC_INSTALL_DIR`)
  - Added separate `test` target
  - Added proper `install` target that also refers to _GNU_ standard `DESTDIR`
    usage
  - Added `distclean` and `clobber` targets to remove all configuration
  - Added `reconfig` target to regenerate `duma_config.h`
  - Changed `Makefile` flow to recursively run if `duma_config.h` doesn't
    exist
  - Fixed return value error in `testmt.c`
  - Fixed args in `thread-test.c`
- Update `Makefile`
  - Updated `PACKAGE_SOURCE`
  - Added new target `dos2unix`
    (2008-03-09, HA)

## 2.5.12 (2008-01-25)

- Enhancement: Add environment variable `DUMA_CHECK_FREQ` to automatically
  check all memories _No-Mans-Land_ at each _n_'th allocation / deallocation. Use
  zero (`0`) for _n_ to deactivate the automatic checks
  (2008-01-24, HA)
- Updated `README.txt` for environment variables `DUMA_CHECK_FREQ` and
  `DUMA_SKIPCOUNT_INIT`
  (2008-01-25, HA)

## 2.5.11 (2008-01-20)

- Bugfix on thread safety in `sem_inc.c`, thanks to Andrew Kornev writing the
  right hint as bug-report: "_[ 1875926 ] non-thread safe `lock()` implementation_"
  (2008-01-20, HA)

## 2.5.10 (2008-01-15)

- Added a small test program to demonstrate crashing on multi-threaded
  programs from
  http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=67838
  (2008-01-15, HA)
- Added HTML comparison charts / text with C source code examples from
  http://www.cs.utexas.edu/~jpmartin/memCheckers.html
  into sub-directory comparisons with kind permission of _Jean-Philippe Martin_
- Update `Makefile` for all C source files
  (2008-01-14, HA)
- Bugfix for `Makefile` target `install` when `DUMASO` is empty; bug was reported
  by _Louis Strous_
  (2008-01-09, HA)
- Added hint where _No-Mans-Land_ was overwritten (below or above user-space)
  (2008-01-03, HA)

## 2.5.9 (2007-12-23)

- Added link to _mpatrol_ in `README`
- Added shell environment variable `DUMA_SKIPCOUNT_INIT`
  (2007-08-22, HA)
- Added section for _NetBSD_ in `Makefile`; there is always one (`1`) memory leak on
  _NetBSD_ 3.1, but the functionality works
  (2007-08-28, HA)
- Tested on `powerpc-apple-darwin8.0.0` inside of _PearPC_ 0.4 on _Windows_:
  - Compiles and passes all confidence tests; trying to debug `example1`
    from within gdb crashes _PearPC_ 0.4; running without gdb creates a
    core dump file that can be used for post-mortem analysis
    (2007-09-02, HA)
- Bugfix for `_duma_strncpy()`: no more calls to `strlen()`, reported by
  [Kamal Neet](mailto:kamalneet@gmail.com)
- Applied patch from [Eddy Pronk](mailto:epronk@users.sourceforge.net)
  submitted to _SourceForge_ with title "_libduma.so on Solaris_"
  (2007-11-13, HA)
- Added minor error checking in `print.c` (about `DUMA_OUTPUT_FILE`)
- Added some comments in `duma.h` and `dumapp.h`
  (2007-12-23, HA)

## 2.5.8 (2007-08-19)

- Fix: Set `DUMA_PREFER_GETENV` on _Mac OS X_; it doesn't link with `environ`
- Fix: _MINGW_ wants an exact declaration for function `DUMA_strerror`
  in `print.h`
- Fix: Add work-around in `testmt.c` to compile on _Microsoft Windows_
- Fix: Call `duma_init()` (`#ifdef DUMA_EXPLICIT_INIT`) in `testoperators.cpp`
- Added notes on pre-packaged `.rpm` files, provided by
  [Dries Verachtert](mailto:dries@ulyssis.org) at
  http://dries.ulyssis.org/apt/packages/duma/info.html
  (2007-08-17, HA)
- Added environment variable `DUMA_REPORT_ALL_LEAKS` defaulting to zero (`0`)
- Added empty definitions for macro functions `DUMA_SET_ALIGNMENT()`,
  `DUMA_SET_PROTECT_BELOW()`, and `DUMA_SET_FILL()`
- Updated `README` on shell environment variable options, added some entries
  to section _"OTHER ALTERNATIVE / ADDITIONAL DEBUGGING SOFTWARE / TOOLS"_ and
  added section _"COMPILATION NOTES FOR RELEASE / PRODUCTION"_
  (2007-08-18, HA)
- Add single-line function `duma_alloc_return()`; all memory allocated from
  **DUMA** is returned through this function. This allows you to set a
  conditional breakpoint on this function to catch a specific allocation
  (2007-08-19, HA)

## 2.5.7 (2007-08-17)

- Reformatting: Converted tabs to spaces
- Added test program `testenv.c` using global environment variable as alternative
  to `getenv()`
  (2007-08-15, HA)
- Added output of some configuration settings in start-up banner
- Switched off leak detection in shared library for all platforms; use the
  static library for finding leaks, with information where the memory was
  allocated
  - _NOTE_: Full leak checking without further information is quite useless. Too many
    "_correct_" programs don't free all memory, causing the system to free the
    memory at program termination. As a result, many platforms / environments
    are reported as "_broken_" by **DUMA**
- Output allocator type (`malloc`, `strdup`, `…`) with leak reporting for each
  non-freed memory block
- Removed option `DUMA_USE_FRAMENO` and the `frameno` variable; I suppose nobody
  is using it, but, additionally, it is not thread-safe
- Added option `DUMA_PREFER_GETENV`; if you prefer standard C library `getenv()`
  over global char `**environ`
- Added function `duma_check(void*)` and macro `DUMA_CHECK()`; it checks the
  _No-Mans-Land_ - especially the unprotected end - of the memory block
- Added function `duma_checkAll()` and macro `DUMA_CHECKALL()`; it checks
  the _No-Mans-Land_ - especially the unprotected end - of all allocated
  memory blocks
  (2007-08-17, HA)

## 2.5.6 (2007-08-15)

- Modified example `Makefile` to not link to pthreads on _Microsoft Windows_
  (2007-08-12, HA)
- Added _win32-msvc.net_ project file for `testoperators.cpp`
- Bugfix: Support for option `DUMA_NO_LEAKDETECTION`; check for this option
  before calling any non-existing operators with leak parameters
- Enhanced comments in `Makefile` for `DUMA_SO__` and `DUMA_LIB__` options
- Changes in `Makefile` to build dynamic library on Mac OS X
- Preloading **DUMA** library now works on Mac OS X
- Added test for _Debian_ bug report _241156_ as `testmt.c`
  (2007-08-14, HA)

## 2.5.5 (2007-08-11)

- Support for _Solaris_ (_Solaris_ 10 using the _GNU_ toolchain)
- Fix for _Solaris_: `strerror()` calls `calloc()`, leading to recursion
- Added preprocessor option `DUMA_NO_STRERROR`
- Internal redesign
  - Collected several global static variables into one structure and several
    global non-static variables to another structure, each with prepended
    and appended protection space
    (2007-08-11, HA)

## 2.5.4 (2007-07-31)

- Support for _Win32_ using _MSYS_ / _MINGW_ when calling _make_ with
  `OSTYPE=msys`; call `mingw32-make OSTYPE=msys` from the _Windows_
  _Command Prompt_, after adding _MINGW_'s `bin` to the _PATH_
  (2007-07-28, HA)
- Echo a single dot (`.`) for _Windows_ _Command Prompt_
  (2007-07-31, HA)

## 2.5.3 (2007-07-15)

- Added simple `Makefile` for examples in sub-directory `example_makes/ex*`
- Modified `Makefile` to avoid build of shared libs / executables specific to OS
- `dumapp.h` now keeps C macro definitions
- Support for _Mac OS X_ when calling _make_ with `OS=osx`
- Support for _Win32_ using _Cygwin_ when calling _make_ with `OSTYPE=cygwin`
  (2007-07-14, HA)
- Bugfix in `createconf.c` for _Win32_ when **_not_** using _Visual C++_
- _Cygwin_ fixes
  - Switch off stacktrace (_before trying to integrate in `Makefile`_)
  - `caddr_t` and `u_int` were already defined
  - Use signals on _Linux_ / _UNIX_ in `dumatest.c`
  - Update `Makefile`
    (2007-07-13, HA)

## 2.5.2 (2007-07-12)

- Other minor corrections
- Added preprocessor option `DUMA_SEMAPHORES` to select pthreads locking
  mechanism: semaphores or mutexes
- Removed automatically generated _Visual C++_ file `detoursexample1.ncb`
  from _CVS_ and archives
  (2007-07-12, HA)

## 2.5.1 (2007-07-08)

- Turned global variables `DUMA_PROTECT_BELOW`, `DUMA_ALIGNMENT`, and `DUMA_FILL`
  into thread local variables
  - Access with new function macros:
    - `DUMA_SET_ALIGNMENT()`
    - `DUMA_SET_PROTECT_BELOW()`
    - `DUMA_SET_FILL()`
- Support for `posix_memalign()`
- Add `pthread_mutex` implementation (_as an alternative to semaphores_)
  (2007-07-03, HA)

## 2.5.0 (2006-07-01)

- Documentation updates
  - Use _Natural Docs_
- _Win32_-specific fixes
  - _Detours_ support for using **DUMA** (_with just binaries_)
  - Compile a **DUMA** DLL
  - **DUMA** can perform stack traces of each allocation
  - Support for _Visual Studio 2005_
  - Support for output to debug console
- Support for output to a file
  - Can control output via environmental variables

## 2.4.28 (2006-06-18)

- Enhancement: Reworked initialization for Request-ID _1458083_ on _SF_;
  checks for recursive calls in `_duma_init()` and `duma_init()`
  (2006-06-17, HA)
- Bugfix in `new[]` operators occurred on _alpha-linux1_ on _SF_'s compile farm
  (2006-06-18, HA)
- Fix in `Makefile`: `DUMA_OPTIONS` was not used when compiling `createconf.o`
- Fix compile error when `DUMA_NO_LEAKDETECTION` was set in `testoperators.cpp`
- Fix to obey `EXPLICIT_INIT` in `testoperators.cpp`
  (2006-06-18, HA)
- Fix: As in Request-ID _1482267_ on _SF_: Updated install directory for manual:
  `$prefix/share/man/man3` should fit the _Filesystem Hierarchy Standard_
  (_FHS_) 2.3 for `/usr` and `/usr/local`
  (2006-06-18, HA)

## 2.4.27 (2006-04-23)

- Published a German-language article about **DUMA** on http://duma.sourceforge.net
- Bugfix: Removed buffer overflow in `DUMA_sprintf()` for long file names
  - Added parameter `maxsize` for output buffers in some functions
  - Incremented `STRING_BUFFER_SIZE` to reduce this risk; bug reported
    by _<loigu@volny.cz>_
    (2006-01-09, HA)

## 2.4.26 (2005-10-28)

- New project file for _Dev-C++_ 4.9.9.2 for `createconf` from
  [Benjamin Berkels](mailto:benjamin.berkels@ins.uni-bonn.de)
- Bugfix and enhancement: Avoid recursion from `sem_init()` in _Linux_ / _UNIX_, where
  `sem_init()` may call `malloc()`, which again calls semaphore locking
  (2005-10-25, HA)
- Bugfix in `delete` macro: _Magic_ must be a single variable! Bug was reported
  by [Benjamin Berkels](mailto:benjamin.berkels@ins.uni-bonn.de)
- Some more _TLS_ preparation
  (2005-10-28, HA)

## 2.4.25 (2005-10-21)

- Bugfix: semaphores were not initialized correctly; this lead to a hang
  - Thanks for reporting (_and providing a test environment_) go to
    [Tim Braun](mailto:braun@informatik.uni-kl.de)
- Fixes for _Dev-C++_ from [Benjamin Berkels](mailto:benjamin.berkels@ins.uni-bonn.de)
  - Updated project files for _Dev-C++_ 4.9.9.2 / _GCC_ 3.3.1
  - Fixed compiler errors affecting _Dev-C++_ 4.9.9.2 / _GCC_ 3.3.1
- Modified `delete` macros
- `DUMA_Magic` now array (`call_depth`)
  (2005-10-21, HA)

## 2.4.24 (2005-10-10)

- New program `testoperators` writes it's own member `new` / `delete` operator with
  file name and line number forwarding to **DUMA**
- Reworked `Makefile`: Added `tstheap_so`, which runs with shared library
- Output in banner now shows which library (_static_ / _shared_) is in use
  (2005-10-09, HA)

## 2.4.23 (2005-10-08)

- Write **DUMA**'s preprocessor flags at compile time to `duma_config.h`
- Update `Makefile`
  (2005-10-07, HA)

## 2.4.22 (2005-10-06)

- C++ `new` operators are now (_more_) standard conforming; allocations of
  size equal to zero (`0`) now returns a non-zero (`!0`) pointer
- New option `DUMA_SUPPRESS_ATEXIT` as environment variable from
  [Tim Braun](mailto:braun@informatik.uni-kl.de) that suppress calls to the C
  library's `atexit()` function for **DUMA**'s leak checking function in buggy
  environments (_where `atexit()` hangs_)
- Implemented alternative for C library's `atexit()` using the _GNU C_ compiler's
  'destructor' function attribute
- Add new preprocessor flag `DUMA_PREFER_ATEXIT` in `Makefile`
- Updated project files for _Dev-C++_ 4.9.9.2 / _GCC_ 3.3.1 from
  [Benjamin Berkels](mailto:benjamin.berkels@ins.uni-bonn.de)
- Updated `TODO`
- Bugfix: `EF_` was not replaced by `DUMA_` in `tstheap.c`
- Bugfix: Fix rules for building test objects; `DUMA_OPTIONS` were not passed
  (2005-10-06, HA)

## 2.4.21 (2005-09-29)

- Modified `Makefile`: name for shared library and symbolic links now
  configurable at start
- Update `README`
- Sort `TODO` by priority
  (2005-10-04, HA)
- Bugfix: Follow `enum _DUMA_FailReturn` in `_duma_allocate()` function
- Bugfix: Initializing `userAddr` in `_duma_allocate()` function may lead to bug
  if allocation failed
- `new` operators now (_nearly_) C++ standard conforming
  (2005-09-29, HA)

## 2.4.20 (2005-09-27)

- Integrated patches from [Zbynek Vyskovsky](kvr@centrum.cz)
  - Use tighter alignment when size is smaller than default alignment
  - Replaced calls to `EF_Exit()` by `EF_Abort()`
  - Modified `README`
    (2005-09-19 - 2005-09-23, ZV)
  - Reworked _Zbynek_'s patch to use `long` in internal address calculations
    - Auto-detect compatible integer types `DUMA_ADDR` and `DUMA_SIZE` in
      `createconf`
    - Auto-detect `DUMA_MIN_ALIGNMENT` in `createconf`
    - Write values to `duma_config.h`
    - Use auto-detected values in code
- Reworked `DUMA_sprintf()` and it's format specifier
- Bugfixed `Makefile`: `DUMA_SO_OPTIONS` also used `DUMA_OPTIONS`
  - Removed `-DDUMA_NO_CPP_SUPPORT`, as `LD_PRELOAD` works for C++ `new`,
    `new[]`, `delete` and `delete[]` operators
    (2005-09-24, HA)
- Bugfixed `reduceProtectedMemory()`: _Windows_ `VirtualFree(,,MEM_RELEASE)` can
  only free whole allocations, not parts
- Bugfix in memory allocation: try allocation in loop until
  `reduceProtectedMemory()` fails de-allocating more memory; long running
  programs with much memory handling should benefit
  (2005-09-26, HA)
- Added project files for _Microsoft Visual C++ .NET_ 2003
- Applied one more patch from _Zbynek Vyskovsky_ concerning alignment; his
  calculation of next lower power of two (2) is much smaller
- Bugfix: return `(void*)0` in `_duma_allocate()`, when `userSize == 0`
  (2005-09-27, HA)

## 2.4.19 (2005-09-20)

- Fixed compile error in `duma.c` introduced in 2.4.18
- Enhanced `Makefile`
  (2005-09-19, HA)
- Added run-time check for `DUMA_PAGE_SIZE` from `duma_config.h`
  (2005-09-17, HA)
- Added check for initialization function `__attribute ((constructor))`
  (2005-09-18, HA)

## 2.4.18 (2005-09-16)

- `DUMA_get_sem()` is no longer called when `DUMA_NO_THREAD_SAFETY` is set
- Split `CHANGELOG` document into two files: `TODO` and `CHANGELOG`
- Created `INSTALL` document
- Modified `Makefile`: added special rules for building shared library
  (2005-09-10, HA)
- Modified `README`
  (2005-09-16, HA)
- Removed option `DUMA_FREE_WIPES`
- Documented options in `README`:
  - `DUMA_SLACKFILL`
  - `DUMA_MALLOC_FAILEXIT`
  - `DUMA_MAX_ALLOC`
  - `DUMA_FREE_ACCESS`
  - `DUMA_SHOW_ALLOC`
  - `DUMA_DISABLE_BANNER`
- Add new preprocessor flag `DUMA_NO_HANG_MSG` in `Makefile`
  (2005-09-16, HA)

## 2.4.17 (2005-09-10)

- Forked _Electric Fence_, because _Bruce Perens_ wrote:
  > … Hayati Ayguen must choose another name for his program
  > … "Electric Fence" is a trademark he is not permitted to use …
- Opened a "_new_" project for **DUMA** (_Detect Unintended Memory Access_)
  - Rename _efence_ to **duma**
  - Rename _Electric Fence_ to **DUMA**
  - Rename `EF_ to DUMA_`
  - Rename `ef_ to duma_` (_and so on …_)
    (2005-09-10, HA)

## 2.4.16 (2005-08-28)

- Added examples for article portability: use standardized `strerror()` instead
  of `sys_nerr` and `sys_errlist[]`
- Bugfix: Error when defining `EF_NO_LEAKDETECTION` in `_eff_allocate()`
- Bugfix: Error when defining `EF_NO_LEAKDETECTION` in `_eff_deallocate()`
- Enhancement: Mark allocations from standard libraries before `ef_init()` is
  called with `lineno = -1` to allow special treatment in `leak_checking`
- Changed default in `eftest` to use two signals: `SIGSEGV` and `SIGBUS` instead of
  single `PAGE_PROTECTION_VIOLATED_SIGNAL`
- Define functions (`memcpy`, `strcpy`, `strcat`) as non-intrinsic for _Visual C++_
  compiler in Release mode
  (2005-08-28, HA)
- Enhancement: Allow alignments bigger than page size
  (2005-08-23, HA)
- Document `EF_NO_THREAD_SAFETY` preprocessor flag in `Makefile`
- Added `EF_EXPLICIT_INIT` preprocessor flag to workaround buggy environments
  (2005-08-21, HA)

## 2.4.15 (2005-07-24)

- Bugfixed page management under _Microsoft Windows_; virtual address space was never
  released. Unfortunately, this "quick" fix avoids memory pooling on _Windows_
- Remove double "_Electric Fence:_" messages when calling `EF_Abort()`
  or `EF_Exit()`
  (2005-07-24, HA)
- Added parameter `ExitOnFail` to `Page_Create()`
- Added warning when allocation failed and `ExitOnFail` is off
  - Extended `_eff_allocate()` for this; prepared new for standard conformity
    (2005-07-20, HA)

## 2.4.14 (2005-07-15)

- Integrated code from [Tim Braun](mailto:braun@informatik.uni-kl.de)
  - New `EF_SHOW_ALLOC` environment variable, which prints allocations and
    deallocation when variable set check if `malloc()` and `free()` are bound
    to _efence_
    (2005-07-15, HA)
- Integrated a patch from [Zbynek Vyskovsky](mailto:kvr@centrum.cz) in his
  words: "_I modified Electric Fence a little, to be able to catch free-ing
  memory via watch point in debugger. The patch (attached) solves the
  problem when you are watching expression for change and suddenly
  realize the memory containing expression disappeared. It has no
  other effect, just makes debugging easier._"
  (2005-07-11, HA)
- Bugfix for internal slot mode `EFST_DEALLOCATED` reported by _Mikolas Patocka_
  error occurred when user-space memory region of freed memory is reused from
  system and freed again: _efence_ claimed it was freeing already freed memory
  (2005-03-02, HA)
- Defining macros / functions, also for `str[n]cpy()` and `str[n]cat()`, checking for
  overlapping memory regions
  (2005-02-17, HA)
- Now defining macros / functions, also for `strdup()` / `memcpy()`
- `memcpy()` checks if memory regions overlap
  (2005-02-16, HA)
- Added `EF_OLD_DEL_MACRO` preprocessor flag to keep compatibility to old
  `DEL_ELEM()` / `DEL_ARRAY()` macros; when not defined, a macro for `delete`
  is defined, which has standard conforming syntax
  (2005-02-14, HA)

## 2.4.13 (2005-02-11)

- Added internal flag saving the source (_allocation_ / _deallocation_) for
  file name and line number
- Added `EF_OLD_NEW_MACRO` preprocessor flag to keep compatibility to old
  `NEW_ELEM()` / `NEW_ARRAY()` macros. When not defined, a macro for `new` is
  defined, which has standards conforming syntax. Unfortunately, you have to
  use `DEL_ELEM` / `DEL_ARRAY` further to utilize file name and line number
  of deallocation calls
  (2005-02-10, HA)
- Reworked / revised _ALL_ internal functions (_!!!_)
- Free-ing already freed memory is now (_better_) detected
- Disallows coalescing memory regions and also stores
  `__FILE__` and `__LINE__` of `free()` to be able to print position
  of first free
- Added `EF_USE_FRAMENO` preprocessor flag. Changed not to support
  `EF_newFrame()` and `EF_delFrame()` by default
  (2005-02-07, HA)
- Applied bug fix posted by _Luis Gonzalez_. Bug was occurring under _SuSE Linux_
  9.1 when calling `eftest` with a higher number (_such as `10000`._)
  (2005-02-02, HA)
- Bugfixed `memalign()` which was not exported as `extern "C"`. This bug lead to
  crashes when free-ing memory allocated by `memalign()`. Bug was reported by
  _Grant Goodyear_ and _Steven G. Johnson_. Now defining macros / functions
  also for `valloc()`
  (2005-02-03, HA)

## 2.4.12 (2005-01-05)

- Added `EF_NO_GLOBAL_MALLOC_FREE` preprocessor flag to work-around buggy
  environments: no `malloc()`, `free()`, `realloc()`, `calloc()` are put into the
  global name-space of the _efence_ library, thus only files which include
  `efence.h` call the _efence_ `malloc()`, `…` replacement functions. This flag
  is also helpful where linking order cannot get controlled e.g. when
  memory is allocated from a library not using _efence_, but `free` is
  called from _efence_ using _efence_. This is a problem when using
  DLL libraries, which are linked against `msvcrt.dll` and its
  `malloc()` / `free()` under _Microsoft Windows_. The same problem applies
  when `libstdc++` / `libgcc` is not linked in correct order
  (2005-01-04, HA)

## 2.4.11 (2005-01-03)

- Bugfixed signal handling in confidence test (`eftest`), which lead to false
  errors with my newly installed _Mandrake Linux_ 10.1
  (2005-01-03, HA)
- Bugfixed checking at `free()`, when `EF_PROTECT_BELOW` flag changed which lead
  to false positives
- Bugfixed error message output to stderr; length of messages were calculated
  too short; fixes functions `EF_Abort()` and `EF_Exit()`
  (2004-08-15, HA)

## 2.4.10 (2004-07-21)

- Added preprocessor macros for `CA_DECLARE`, `CA_DEFINE`, and `CA_REF` in `efence.h`
  for declaration, definition and reference to Checked Arrays. These macros
  allow checks of arrays which are allocated on stack, but they are very
  slow on references cause every reference has to be checked
- Added `EF_ASSERT()` which definitely halts (_for debugging_)
- Precompiled headers have to be switched off in _Microsoft Visual C++_ to get all the
  C++ operators to work, but don't ask me why!
- Implemented all the various `new` / `delete` / `new[]` / `delete[]` C++ operators. The
  implementations are not C++ conforming concerning their behavior added type
  mismatch detection between `malloc()` / `free()`, `new` / `delete`, `new[]` / `delete[]`;
  this is deactivated when `EF_NO_LEAKDETECTION` is set
- Added `EF_MALLOC_FAILEXIT` parameter (_for now, not implemented_)
- Extended semantics of `EF_PROTECT_FREE`
- Added `EF_MAX_ALLOC` parameter
- Bugfix: Why should we ever use `Page_Delete()` when setting up the
  "_dead_" page?
- Bugfix: `EF_PROTECT_FREE` did exactly the opposite of what it should
  do, from Bruce's version 2.2.2 (or maybe earlier)
  (2004-07-16, HA)
- Confidence test now pass again
- Removed problems when used `EF_NO_LEAKDETECTION` preprocessor flag
- `Makefile` works again, also included automatic generation of
  `efence_config.h`
- Added automatic generation and inclusion of `efence_config.h` instead
  function `Page_Size()`
  (2004-07-15, HA)
- Added code for thread safety: Only one thread at once is allowed to enter
  some internal functions. This can be deactivated using the
  `EF_NO_THREAD_SAFETY` preprocessor flag. There may be flaws concerning
  threads, when using the C++ operators for leak detection; use the
  `EF_NO_LEAKDETECTION` preprocessor flag if you encounter problems!
- Some files (`page.c` and `print.c`) have turned to header files, the needed
  functions are now declared static and are no more visible outside
  `efence.c` this way the `NDEBUG` preprocessor flag has been replaced by
  `EF_NO_EFENCE`, `EF_NO_CPP_SUPPORT` - be careful not to include
  _efence_ in your release builds!
  (2004-03-21, HA)

## 2.4.9 (2002-12-05)

- Added checks for No-Mans-Land (_space which cannot get protected from the MMU_);
  this area gets filled with `EF_FILL` on allocation, and gets checked on
  deallocation
  (2002-12-05, HA)

## 2.4.8 (2002-12-04)

- One big fix and many smaller improvements for C++ code. Old macros `NEW_ELEM`,
  `NEW_ARRAY` were not usable when allocating classes, which have
  constructor / destructor; the usage of these macros has not changed
  (2002-12-04, HA)

## 2.4.7 (2002-11-22)

- Support _Borland_'s free command-line tools: _Borland C++ Builder_ 5.5
  - Throw away unnecessary declarations of `malloc()`, `free()`, `…`
    _Borland_ didn't like them, and neither _Microsoft Visual C++_ 6.0, nor _gcc_
    (_Dev-C++_) needs them
  - Added project files in sub-directory `win32-vide` for _VIDE_ 1.24 using
    _Borland C++ Builder_ 5.5 (_see http://www.objectcentral.com_)
    (2002-11-22, HA)

## 2.4.6 (2002-11-18)

- Remove memory leak in eftest
- Allow re-inclusion of `efence.h` after inclusion of `efenceint.h`
- Added some project files in sub-directory `win32-devcpp` for _Dev-C++_ 4.9.6
  using _gcc_ (_See http://www.bloodshed.net/_) and saved its generated
  `Makefile`'s; left `tstheap`'s memory leaks
  (2002-11-18, HA)

## 2.4.5 (2002-11-17)

- Modifications in print routines (`EF_Print()`, `EF_Abort()`, `…`)
- Added direct output to _Visual C++_ debug output window
- Some additions to `Makefile`
  (2002-11-17, HA)

## 2.4.4 (2002-11-03)

- Separation of C++ files: `efencepp.h`, `efencepp.cpp`
- Rename `EF_push()` / `EF_pop()` to `EF_newFrame()` / `EF_delFrame()`
- Add new compile option `EF_NO_LEAKDETECTION`
- Add new compile option `EF_NO_CPP`
- Split `efence.h` to `efence.h` and `efenceint.h`
- Add _Copyright_ and _License_ information to many files
- Rename `COPYING` to `COPYING-GPL`
  (2002-11-03, HA)

## 2.4.3 (2002-10-08)

- Added following preprocessor macros for Release and Debug Mode
  - `#define NEW_ELEM(TYPE) new TYPE`
  - `#define NEW_ARRAY(TYPE,COUNT) new (TYPE)[COUNT]`
  - `#define DEL_ELEM(PTR) delete PTR`
  - `#define DEL_ARRAY(PTR) delete []PTR`
    which are mapped to `malloc()` and `free()` in Debug Mode
- Use these macros to get the file name and line number of a C++-style memory leak
  (2002-10-08, HA)

## 2.4.2 (2002-10-07)

- Added support for implicit and explicit leak checking:
  - `void EF_push(void)` creates a new memory frame
  - `void EF_pop(void)` checks for memory leaks allocated after `EF_push()`;
    this function is called automatically by `onexit()`, even without
    `EF_push()`
    (2002-10-07, HA)

## 2.4.1 (2002-10-06)

- Port to _Windows NT_ (_2000_ / _XP_) by adding sections to following functions:
  - `Page_AllowAccess()`
  - `Page_Create()`
  - `Page_Delete()`
  - `Page_DenyAccess()`
  - `Page_Size()`
    (2002-10-06, HA)

## 2.4.0 (2002-10-03)

- Added support for C++ operators `new`, `new[]`, `delete` and `delete[]`
- Added some project files in sub-directory `win32-msvc/` for
  _Microsoft Visual C++_ 6.0
  (2002-10-03, HA)

## 2.2.2

- Shared library must be position-independent code

## 2.2.0

- Merge in bug-fixes, multi-thread patch, shared library patch, `debian/`
  sub-directory used for building the _Debian_ package

## 2.0.5 (1995-01-20)

- Port to _Linux_
  (1995-01-20)

## 2.0.4 (1994-09-30)

- When `realloc` is passed a zero address, it should work the same way as
  `malloc()`. Fix forward declaration of `mprotect()` in `page.c` to use `void *`,
  not `caddr_t`, for addresses; _IRIX_ 5.0.1 complained about that
  (1994-09-30)

## 2.0.3 (1993-05-29)

- Don't attempt to allow access to a zero-sized page when `EF_ALLOW_MALLOC_0` is set
- Attempt to un-map memory from `Page_Delete()`. If that doesn't work, fall
  back by protecting the page from all references. Un-mapping small
  segments of a mapping used to crash my _SGI IRIX_ 5.0 system; I assume
  that nobody is running 5.0 any longer
  (1993-05-29)

## 2.0.2

- Add support for _DEC Alpha_
- Add `%a` pattern for printing addresses, which assumes they are passed in
  a `void *`

## 2.0.1

- Add work-arounds for kernel and library bugs under _HP-UX_
  - _HP_ has been notified and will repair these soon
