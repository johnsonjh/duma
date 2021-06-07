# DUMA configuration:
#
# add "-DDUMA_NO_GLOBAL_MALLOC_FREE" (without quotes)
#   for not defining malloc/free in global namespace
#
# add "-DDUMA_EXPLICIT_INIT" (without quotes)
#    to do all the "risky" stuff getenv(), sem_init(), write() .. explicitly
#      from main().
#    you have to call duma_init() explicitly from main() this option also
#    avoids leak error messages for allocations before duma_init() got called.
#    this helps for leaky standard environments.
#
# add "-DDUMA_NO_THREAD_SAFETY" (without quotes)
#    for not supporting multi-threading
#
# add "-DDUMA_SO_NO_CPP_SUPPORT" or "-DDUMA_LIB_NO_CPP_SUPPORT" (without quotes)
#    for not directing new/delete to malloc/free
#
# add "-DDUMA_SO_NO_LEAKDETECTION" or "-DDUMA_LIB_NO_LEAKDETECTION" (without quotes)
#    if you don't want support for leak detection
#
# add "-DDUMA_SO_PREFER_ATEXIT" or "-DDUMA_LIB_PREFER_ATEXIT" (without quotes)
#    if you prefer atexit() over GNU Compiler's function attribute "destructor"
#
# add "-DDUMA_SO_PREFER_GETENV" or "-DDUMA_LIB_PREFER_GETENV" (without quotes)
#    if you prefer standard C library getenv() over global char **environ
#
# add "-DDUMA_OLD_NEW_MACRO" (without quotes)
#    if you want to use DUMA's old style NEW_ELEM() / NEW_ARRAY() macros.
#    when not defining this option, a standard conform new syntax can get used.
#    unfortunately you have to use DEL_ELEM/DEL_ARRAY further to utilize
#    filename and linenumber of deallocation calls
#
# add "-DDUMA_SO_NO_HANG_MSG" or "-DDUMA_LIB_NO_HANG_MSG" (without quotes)
#    set this if you want to suppress the extra messages around atexit().
#
# add "-DDUMA_NO_STRERROR" (without quotes)
#    set this if you want to suppress calls to strerror() to avoid recursion
#    on specific platforms.
#
########################################
#
#  preprocessor flags for building the shared library (DUMA_SO_LIBRARY):
#    - DUMA_SO_NO_CPP_SUPPORT
#    - DUMA_SO_NO_LEAKDETECTION
#    - DUMA_SO_PREFER_ATEXIT
#    - DUMA_SO_PREFER_GETENV
#    - DUMA_SO_NO_HANG_MSG
#
#  preprocessor flags for building the static library:
#    - DUMA_LIB_NO_CPP_SUPPORT
#    - DUMA_LIB_NO_LEAKDETECTION
#    - DUMA_LIB_PREFER_ATEXIT
#    - DUMA_LIB_PREFER_GETENV
#    - DUMA_LIB_NO_HANG_MSG
#    - DUMA_NO_GLOBAL_MALLOC_FREE
#    - DUMA_EXPLICIT_INIT
#    - DUMA_NO_THREAD_SAFETY
#    - DUMA_OLD_NEW_MACRO
#    - DUMA_OLD_DEL_MACRO
#    - DUMA_NO_STRERROR
#
########################################

# edit following line
DUMA_OPTIONS=

# no leak detection on any platform:
# use static library for finding leaks - with information memory was allocated
# i think leak-checking without further information is quite useless!
# additionaly too many platforms/environments are broken.
DUMA_OPTIONS += -DDUMA_SO_NO_LEAKDETECTION

# some test cases:
#DUMA_OPTIONS += -DDUMA_LIB_NO_LEAKDETECTION
#DUMA_OPTIONS += -DDUMA_NO_THREAD_SAFETY
#DUMA_OPTIONS += -DDUMA_NO_CPP_SUPPORT

PIC=-fPIC -DPIC
DUMA_SO_OPTIONS=$(PIC) -DDUMA_SO_LIBRARY

# for FreeBSD 5.4
# DUMA_OPTIONS += -DPAGE_PROTECTION_VIOLATED_SIGNAL=SIGBUS
#
# for FreeBSD 5.4 if DUMA_EXPLICIT_INIT is not set
# DUMA_OPTIONS += -DDUMA_NO_LEAKDETECTION
#
# for cygwin environment on Windows
# DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
# also define 'WIN32'

# some defaults:
INSTALL=install
RM=rm
RMFORCE=rm -f
ECHO=echo
ECHOLF=echo .
CC=cc
CXX=c++
LD=ld
AR=ar
RANLIB=ranlib
INSTALL=install

ifndef $(OS)
 OS=$(shell uname -s 2>/dev/null | tr '[:upper:]' '[:lower:]' 2>/dev/null || true)
endif

# dynamic dependencies
DUMA_DYN_DEPS=$(DUMASO) tstheap_so$(EXEPOSTFIX) dumatestpp_so$(EXEPOSTFIX)

# some OS specifics:
ifeq ($(OS), windows_nt)
  ifeq ($(OSTYPE), msys)
	$(info using settings for OS=Windows_NT, OSTYPE=msys)
    # call mingw32-make OSTYPE=msys
    # from Windows command prompt
    # having added the PATH for MINGW/bin
    # using explicit initialization to avoid leak report
    # from __w32_sharedptr_initialize() function
    BSWITCH=101
    DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
    RM=del
    RMFORCE=del /F 2>nul
    ECHO=echo
    ECHOLF=echo .
    CURPATH=
    DUMA_DYN_DEPS=
    DUMASO=
    CFLAGS=-g -O0
    CPPFLAGS=-g -O0
    LIBS=
    EXEPOSTFIX=.exe
  endif
  ifeq ($(OSTYPE), msys-sh)
	$(info using settings for OS=Windows_NT, OSTYPE=msys-sh)
    # call mingw32-make OSTYPE=msyssh
    # from MSYS shell
    # having added the PATH for MINGW/bin
    # using explicit initialization to avoid leak report
    # from __w32_sharedptr_initialize() function
    BSWITCH=102
    DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
    CC=mingw32-gcc
    CXX=mingw32-g++ -std=c++11
    RM=rm
    RMFORCE=rm -f
    ECHO=echo
    ECHOLF=echo
    CURPATH=./
    DUMA_DYN_DEPS=
    DUMASO=
    CFLAGS=-g -O0
    CPPFLAGS=-g -O0
    LIBS=
    EXEPOSTFIX=.exe
  endif
  ifeq ($(OSTYPE), cygwin)
	$(info using settings for OS=Windows_NT, OSTYPE=cygwin)
    # call make OSTYPE=cygwin
    BSWITCH=103
    DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
    CURPATH=./
    DUMA_DYN_DEPS=
    DUMASO=
    CFLAGS=-g -O0 -DWIN32 -Wall -Wextra
    CPPFLAGS=-g -O0 -DWIN32 -Wall -Wextra
    LIBS=
    EXEPOSTFIX=.exe
  endif
  ifndef BSWITCH
	$(info using settings for OS=Windows_NT)
    BSWITCH=100
    DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
    CURPATH=./
    DUMA_DYN_DEPS=
    DUMASO=
    CFLAGS=-g -O0 -DWIN32
    CPPFLAGS=-g -O0 -DWIN32
    LIBS=
    EXEPOSTFIX=.exe
  endif
endif

ifeq ($(OS), darwin)
  $(info using settings for OS=darwin)
  # call: make OS=darwin
  BSWITCH=210
  DUMA_OPTIONS += -DPAGE_PROTECTION_VIOLATED_SIGNAL=SIGBUS
  DUMA_OPTIONS += -DDUMA_SO_PREFER_GETENV
# DUMA_OPTIONS += -DDUMA_LIB_NO_LEAKDETECTION
  CURPATH=./
# DUMA_DYN_DEPS=
  DUMASO=libduma.dylib
  DUMASO_LINK1=libduma.dylib
  CFLAGS=-g -O0
  CPPFLAGS=-g -O0
  LIBS=-lpthread
  EXEPOSTFIX=
  prefix?=/opt/duma
endif

ifeq ($(OS), freebsd)
  $(info using settings for OS=freebsd)
  BSWITCH=310
  DUMA_OPTIONS += -DDUMA_NO_THREAD_SAFETY
  DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
  CURPATH=./
  DUMA_DYN_DEPS=
  DUMASO=
  DUMASO_LINK1=
  CFLAGS=-g -O0
  CPPFLAGS=-g -O0
  LIBS=-lpthread
  EXEPOSTFIX=
endif

ifeq ($(OS), netbsd)
  $(info using settings for OS=netbsd)
  BSWITCH=320
  CURPATH=./
  DUMASO=libduma.so.0.0.0
  DUMASO_LINK1=libduma.so.0
  DUMASO_LINK2=libduma.so
  CFLAGS=-g -O0
  CPPFLAGS=-g -O0
  LIBS=-lpthread
  EXEPOSTFIX=
endif

ifeq ($(OS), solaris)
  $(info using settings for OS=solaris)
  BSWITCH=410
  DUMA_OPTIONS += -DDUMA_NO_STRERROR
  CURPATH=./
  DUMA_DYN_DEPS=
  DUMASO=libduma.so.0.0.0
  DUMASO_LINK1=libduma.so.0
  DUMASO_LINK2=libduma.so
  CFLAGS=-g -O0
  CPPFLAGS=-g -O0
  LDFLAGS += -lgcc_s
  LDOPTIONS += -lgcc_s
  LIBS=-Wl,-R/opt/sfw/lib -lpthread
  EXEPOSTFIX=
endif

ifeq ($(OS), linux)
 ifeq ($(shell $(CXX) -v 2>&1 | grep -c "clang version"), 1)
  COMPILERX := clang++
 else
  COMPILERX := g++ -std=c++11
 endif
 export COMPILERX
 ifeq ($(shell $(CC) -v 2>&1 | grep -c "clang version"), 1)
  COMPILER := clang
 else
  COMPILER := gcc
 endif
 export COMPILER
 CC=${COMPILER}
 CXX=${COMPILERX}
 $(info using CC=${COMPILER})
 $(info using CXX=${COMPILERX})
 ifeq ($(OSTYPE), pie)
  $(info using settings for OS=linux, OSTYPE=pie)
  # Linux PIE-mode with GNU compiler and GNU tools.
  CC=${COMPILER} -fpie -fPIE
  CXX=${COMPILERX} -fpie -fPIE
  BSWITCH=510
  else
   $(info using settings for OS=linux)
   CC=${COMPILER}
   CXX=${COMPILERX}
   BSWITCH=610
  endif
  CURPATH=./
  DUMASO=libduma.so.0.0.0
  DUMASO_LINK1=libduma.so.0
  DUMASO_LINK2=libduma.so
  CFLAGS=-g -O0 -Wall -Wextra
  CPPFLAGS=-g -O0 -Wall -Wextra
  LIBS=-lpthread
  EXEPOSTFIX=
  RM=rm
  RMFORCE=rm -f
  ECHO=echo
  ECHOLF=printf '\n'
endif

ifndef BSWITCH
  # default is generic full "UNIX"
  BSWITCH=810
  $(warning using default options. OS/OSTYPE not set or contain unknown values!)
  CURPATH=./
  DUMASO=libduma.so.0.0.0
  DUMASO_LINK1=libduma.so.0
  DUMASO_LINK2=libduma.so
  CFLAGS=-g -O0 -Wall -Wextra
  CPPFLAGS=-g -O0 -Wall -Wextra
  LIBS=-lpthread
  EXEPOSTFIX=
endif

ifndef HOST_CFLAGS
  HOST_CFLAGS=$(CFLAGS)
endif

ifndef CC_FOR_BUILD
  CC_FOR_BUILD=$(CC)
endif

############################################################

ifndef prefix
  prefix=/usr
  $(info using default prefix [$(prefix)])
endif

MAN_INSTALL_DIR=$(prefix)/share/man/man3
DOC_INSTALL_DIR=$(prefix)/share/doc/duma

ifndef srcdir
  srcdir=.
  $(info using default srcdir [$(srcdir)])
endif

ifndef exec_prefix
  exec_prefix=$(prefix)
  $(info using default exec_prefix [$(exec_prefix)])
endif

# The directory for installing executable programs that users can run.
ifndef bindir
  bindir=$(exec_prefix)/bin
endif

# The directory for installing read-only architecture independent data files.
ifndef datadir
  datadir=$(prefix)/share
endif

# The directory for installing read-only data files that pertain to a single machine--that is to say, files for configuring a host.
ifndef sysconfdir
  sysconfdir=$(prefix)/etc
endif

# The directory for object files and libraries of object code.
ifndef libdir
  libdir=$(exec_prefix)/lib
endif

# The directory for installing header files to be included by user programs with the C `#include' preprocessor directive.
ifndef includedir
  includedir=$(prefix)/include
endif

PACKAGE_SOURCE=README.md CHANGELOG LICENSE COPYING-GPL COPYING-LGPL duma.3 GNUmakefile gdbinit.rc \
	duma.h dumapp.h duma_sem.h paging.h print.h duma_hlp.h noduma.h \
	src/duma.c src/dumapp.cpp src/sem_inc.c src/print.c \
	tests/dumatest.c tests/tstheap.c tests/thread-test.c tests/testmt.c tests/dumatestpp.cpp tests/testoperators.cpp \
	createconf.c make_git_source_version.sh

OBJECTS=dumapp.o duma.o sem_inc.o print.o

SO_OBJECTS=dumapp_so.o duma_so.o sem_inc_so.o print_so.o

# Make all the top-level targets the makefile knows about.
all:	verinfo.h libduma.a $(DUMA_DYN_DEPS)
	@ $(ECHO) "Build complete - you can now run make test."

# Perform self tests on the program this makefile builds.
check test: tstheap$(EXEPOSTFIX) dumatest$(EXEPOSTFIX) thread-test$(EXEPOSTFIX) testmt$(EXEPOSTFIX) dumatestpp$(EXEPOSTFIX) testoperators$(EXEPOSTFIX) $(DUMA_DYN_DEPS)
	@ $(ECHOLF)
	@ $(ECHO) "Testing DUMA (static library):"
	$(CURPATH)dumatest$(EXEPOSTFIX)
	@ $(ECHOLF)
	$(CURPATH)tstheap$(EXEPOSTFIX) 3072
	@ $(ECHOLF)
	$(CURPATH)testoperators$(EXEPOSTFIX)
	@ $(ECHOLF)
	@ $(ECHO) "DUMA static confidence test PASSED."
ifdef DUMASO
	@ $(ECHOLF)
	@ $(ECHO) "Testing DUMA (dynamic library)."
ifeq ($(OS), solaris)
	LD_PRELOAD=./$(DUMASO) DYLD_INSERT_LIBRARIES=./$(DUMASO) DYLD_FORCE_FLAT_NAMESPACE=1 exec $(CURPATH)tstheap_so 3072
else
	(export LD_PRELOAD=./$(DUMASO); export DYLD_INSERT_LIBRARIES=./$(DUMASO); export DYLD_FORCE_FLAT_NAMESPACE=1 ; exec $(CURPATH)tstheap_so 3072)
endif
	@ $(ECHOLF)
	@ $(ECHO) "DUMA dynamic confidence test PASSED."
	@ $(ECHOLF)
	@ $(ECHO) "You may now run make install and then installcheck"
	@ $(ECHOLF)
endif

# Perform installation tests (if any).
# The user must build and install the program before running the tests.
installcheck:
ifdef DUMASO
	@ $(ECHOLF)
	@ $(ECHO) "Testing installed DUMA (dynamic library)."
	@ $(bindir)/duma $(CURPATH)tstheap_so 3072
	@ $(ECHOLF)
	@ $(ECHO) "DUMA installcheck test PASSED."
	@ $(ECHOLF)
endif

# Print variables of GNUmakefile
printvars:
	@echo OS              [$(OS)]
	@echo OSTYPE          [$(OSTYPE)]
	@echo bswitch         [$(BSWITCH)]
	@echo srcdir          [$(srcdir)]
	@echo prefix          [$(prefix)]
	@echo exec_prefix     [$(exec_prefix)]
	@echo bindir          [$(bindir)]
	@echo datadir         [$(datadir)]
	@echo sysconfdir      [$(sysconfdir)]
	@echo libdir          [$(libdir)]
	@echo includedir      [$(includedir)]
ifneq ($(oldincludedir),)
	@echo oldincludedir   [$(oldincludedir)]
else
	@echo oldincludedir empty
endif
	@echo MAN_INSTALL_DIR [$(MAN_INSTALL_DIR)]
	@echo DOC_INSTALL_DIR [$(DOC_INSTALL_DIR)]
	@echo MAKE            [$(MAKE)]
	@echo CC              [$(CC)]
	@echo CFLAGS          [$(CFLAGS)]
	@echo CXX             [$(CXX)]
	@echo CPPFLAGS        [$(CPPFLAGS)]
	@echo LD              [$(LD)]
	@echo AR              [$(AR)]
	@echo LIBS            [$(LIBS)]
	@echo RANLIB          [$(RANLIB)]
	@echo INSTALL         [$(INSTALL)]
	@echo RM              [$(RM)]
	@echo RMFORCE         [$(RMFORCE)]
	@echo ECHO            [$(ECHO)]
	@echo ECHOLF          [$(ECHOLF)]
	@echo PIC             [$(PIC)]
	@echo EXEPOSTFIX      [$(EXEPOSTFIX)]
	@echo CURPATH         [$(CURPATH)]
	@echo DUMA_OPTIONS    [$(DUMA_OPTIONS)]
	@echo DUMA_SO_OPTIONS [$(DUMA_SO_OPTIONS)]
	@echo OBJECTS         [$(OBJECTS)]
	@echo SO_OBJECTS      [$(SO_OBJECTS)]
	@echo DUMASO          [$(DUMASO)]
	@echo DUMASO_LINK1    [$(DUMASO_LINK1)]
	@echo DUMASO_LINK2    [$(DUMASO_LINK2)]
	@echo DUMA_DYN_DEPS   [$(DUMA_DYN_DEPS)]
	@echo PACKAGE_SOURCE  [$(PACKAGE_SOURCE)]

# Print filenames unknown to git
printuk:
	- git status -s --untracked-files=all 2>/dev/null | grep '^? '

# Print filenames known to git but not "up-to-date" (modified)
printmod:
	- git status -s 2>/dev/null |grep '^\ \?M '

# Copy the executable file into a directory that users typically search for
# commands; copy any auxiliary files that the executable uses into the
# directories where it will look for them.
install: libduma.a duma.3 $(DUMASO)
	- mkdir -p $(DESTDIR)$(DOC_INSTALL_DIR)
	$(INSTALL) -m 644 README.md $(DESTDIR)$(DOC_INSTALL_DIR)
	- mkdir -p $(DESTDIR)$(includedir)
	$(INSTALL) -m 644 noduma.h duma.h dumapp.h duma_sem.h duma_config.h $(DESTDIR)$(includedir)
	- mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 duma.sh $(DESTDIR)$(bindir)/duma
	- mkdir -p $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 libduma.a $(DESTDIR)$(libdir)
ifdef DUMASO
	$(INSTALL) -m 755 $(DUMASO) $(DESTDIR)$(libdir)
endif
ifdef DUMASO_LINK1
	- $(RMFORCE) $(DESTDIR)$(libdir)/$(DUMASO_LINK1)
	ln -s $(DUMASO) $(DESTDIR)$(libdir)/$(DUMASO_LINK1)
endif
ifdef DUMASO_LINK2
	- $(RMFORCE) $(DESTDIR)$(libdir)/$(DUMASO_LINK2)
	ln -s $(DUMASO) $(DESTDIR)$(libdir)/$(DUMASO_LINK2)
endif
	- mkdir -p $(DESTDIR)$(MAN_INSTALL_DIR)
	$(INSTALL) -m 644 duma.3 $(DESTDIR)/$(MAN_INSTALL_DIR)/duma.3

# Delete all the installed files that the `install' target would create
uninstall:
	- $(RMFORCE) $(DESTDIR)$(DOC_INSTALL_DIR)/README.md
	- $(RMFORCE) $(DESTDIR)$(includedir)/noduma.h
	- $(RMFORCE) $(DESTDIR)$(includedir)/duma.h
	- $(RMFORCE) $(DESTDIR)$(includedir)/dumapp.h
	- $(RMFORCE) $(DESTDIR)$(includedir)/duma_sem.h
	- $(RMFORCE) $(DESTDIR)$(includedir)/duma_config.h
	- $(RMFORCE) $(DESTDIR)$(bindir)/duma
	- $(RMFORCE) $(DESTDIR)$(libdir)/libduma.a
ifdef DUMASO
	- $(RMFORCE) $(DESTDIR)$(libdir)/$(DUMASO)
endif
ifdef DUMASO_LINK1
	- $(RMFORCE) $(DESTDIR)$(libdir)/$(DUMASO_LINK1)
endif
ifdef DUMASO_LINK2
	- $(RMFORCE) $(DESTDIR)$(libdir)/$(DUMASO_LINK2)
endif
	- $(RMFORCE) $(DESTDIR)$(MAN_INSTALL_DIR)/duma.3

# Delete all files that are normally created by running make.
clean:
	- $(RMFORCE) $(OBJECTS) $(SO_OBJECTS) tstheap.o dumatest.o thread-test.o testmt.o dumatestpp.o a.out \
		tstheap_so.o dumatestpp_so.o testoperators.o \
		tstheap$(EXEPOSTFIX) tstheap_so$(EXEPOSTFIX) dumatest$(EXEPOSTFIX) dumatestpp$(EXEPOSTFIX) dumatestpp_so$(EXEPOSTFIX) testoperators$(EXEPOSTFIX) \
		thread-test$(EXEPOSTFIX) testmt$(EXEPOSTFIX) \
		libduma.a $(DUMASO) libduma.cat

# Any of these targets might be defined to delete more files than `clean' does.
# For example, this would delete configuration files or links that you would
# normally create as preparation for compilation, even if the makefile itself
# cannot create these files.
distclean realclean clobber: clean
	- $(RMFORCE) duma_config.h verinfo.h createconf.o createconf$(EXEPOSTFIX)

roff:
	nroff -man < duma.3 > duma.cat

libduma.a: duma_config.h verinfo.h $(OBJECTS)
	- $(RMFORCE) libduma.a
	$(AR) crv libduma.a $(OBJECTS)
	$(RANLIB) libduma.a

verinfo.h: FORCE
	- $(CURPATH)make_git_source_version.sh
	$(shell touch verinfo.h || true)

duma_config.h:
	$(MAKE) reconfig

reconfig: createconf$(EXEPOSTFIX) createconf.o createconf.c
	- $(CURPATH)createconf$(EXEPOSTFIX)

dos2unix:
	@ dos2unix --d2u $(PACKAGE_SOURCE)

createconf$(EXEPOSTFIX): createconf.o
	- $(RMFORCE) createconf$(EXEPOSTFIX)
	$(CC_FOR_BUILD) $(HOST_CFLAGS) $(DUMA_OPTIONS) createconf.o -o createconf$(EXEPOSTFIX)

tstheap$(EXEPOSTFIX): libduma.a tstheap.o
	- $(RMFORCE) tstheap$(EXEPOSTFIX)
	$(CC) $(CFLAGS) tstheap.o libduma.a -o tstheap$(EXEPOSTFIX) $(LIBS)

dumatest$(EXEPOSTFIX): libduma.a dumatest.o
	- $(RMFORCE) dumatest$(EXEPOSTFIX)
	$(CC) $(CFLAGS) dumatest.o libduma.a -o dumatest$(EXEPOSTFIX) $(LIBS)

dumatestpp$(EXEPOSTFIX): libduma.a dumatestpp.o duma_sem.h dumapp.h
	- $(RMFORCE) dumatestpp$(EXEPOSTFIX)
	$(CXX) $(CPPFLAGS) dumatestpp.o libduma.a -o dumatestpp$(EXEPOSTFIX) $(LIBS)

thread-test$(EXEPOSTFIX): libduma.a thread-test.o
	- $(RMFORCE) thread-test$(EXEPOSTFIX)
	$(CC) $(CFLAGS) thread-test.o libduma.a -o thread-test$(EXEPOSTFIX) $(LIBS)

testmt$(EXEPOSTFIX): libduma.a testmt.o
	- $(RMFORCE) testmt$(EXEPOSTFIX)
	$(CC) $(CFLAGS) testmt.o libduma.a -o testmt$(EXEPOSTFIX) $(LIBS)

testoperators$(EXEPOSTFIX): libduma.a testoperators.o duma_sem.h dumapp.h
	- $(RMFORCE) testoperators$(EXEPOSTFIX)
	$(CXX) $(CPPFLAGS) testoperators.o libduma.a -o testoperators$(EXEPOSTFIX) $(LIBS)

testmemlimit$(EXEPOSTFIX): libduma.a testmemlimit.o
	- $(RMFORCE) testmemlimit$(EXEPOSTFIX)
	$(CC) $(CFLAGS) testmemlimit.o libduma.a -o testmemlimit$(EXEPOSTFIX) $(LIBS)

tstheap_so$(EXEPOSTFIX): tstheap_so.o
	- $(RMFORCE) tstheap_so$(EXEPOSTFIX)
	$(CC) $(CFLAGS) tstheap_so.o -o tstheap_so$(EXEPOSTFIX) $(LIBS)

dumatestpp_so$(EXEPOSTFIX): dumatestpp_so.o
	- $(RMFORCE) dumatestpp_so$(EXEPOSTFIX)
	$(CXX) $(CPPFLAGS) dumatestpp_so.o -o dumatestpp_so$(EXEPOSTFIX) $(LIBS)

testmemlimit_so$(EXEPOSTFIX): testmemlimit_so.o
	- $(RMFORCE) testmemlimit_so$(EXEPOSTFIX)
	$(CC) $(CFLAGS) testmemlimit_so.o -o testmemlimit_so$(EXEPOSTFIX) $(LIBS)

$(OBJECTS) tstheap.o dumatest.o thread-test.o testmt.o dumatestpp.o: duma.h

ifeq ($(OS), windows_nt)
  # do nothing
else
  ifeq ($(OS), darwin)

$(DUMASO): duma_config.h verinfo.h $(SO_OBJECTS)
	$(CXX) -g -dynamiclib -Wl -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
	$(CXX) -g -dynamiclib -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc

  else

$(DUMASO): duma_config.h verinfo.h $(SO_OBJECTS)
	$(CXX) -g -shared -Wl,-soname,$(DUMASO) -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
#	$(CXX) -g -shared -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc

  endif

endif

#
# define rules how to build objects for createconf
#
createconf.o:
	$(CC_FOR_BUILD) $(HOST_CFLAGS) $(DUMA_OPTIONS) -c createconf.c -o $@

#
# define rules how to build objects for shared library
#

dumapp_so.o:	src/dumapp.cpp duma.h duma_sem.h dumapp.h
	$(CXX) -I./ $(CPPFLAGS) $(DUMA_SO_OPTIONS) -c src/dumapp.cpp -o $@

duma_so.o:	src/duma.c duma.h duma_config.h verinfo.h
	$(CC) -I./ $(CFLAGS) $(DUMA_SO_OPTIONS) -c src/duma.c -o $@

sem_inc_so.o:	src/sem_inc.c duma_sem.h
	$(CC) -I./ $(CFLAGS) $(DUMA_SO_OPTIONS) -c src/sem_inc.c -o $@

print_so.o:	src/print.c print.h
	$(CC) -I./ $(CFLAGS) $(DUMA_SO_OPTIONS) -c src/print.c -o $@

# DUMA_SO_OPTIONS needed cause duma.h is included explicitly
tstheap_so.o:
	$(CC) $(CFLAGS) $(DUMA_SO_OPTIONS) -c tests/tstheap.c -o $@

dumatestpp_so.o:
	$(CXX) $(CPPFLAGS) $(DUMA_SO_OPTIONS) -c tests/dumatestpp.cpp -o $@

testmemlimit_so.o:
	$(CC) $(CFLAGS) $(DUMA_SO_OPTIONS) -c tests/testmemlimit.c -o $@

#
# define rules how to build objects for static library
#

dumapp.o:	src/dumapp.cpp duma.h duma_sem.h dumapp.h
	$(CXX) -I./ $(CPPFLAGS) -c src/dumapp.cpp -o $@

duma.o:	src/duma.c duma.h duma_config.h verinfo.h
	$(CC) -I./ $(CFLAGS) -c src/duma.c -o $@

sem_inc.o:	src/sem_inc.c duma_sem.h
	$(CC) -I./ $(CFLAGS) -c src/sem_inc.c -o $@

print.o:	src/print.c print.h
	$(CC) -I./ $(CFLAGS) -c src/print.c -o $@

#
# define rules how to build the test objects
#

dumatest.o:	tests/dumatest.c duma.h duma_config.h verinfo.h
	$(CC) $(CFLAGS) -c tests/dumatest.c -o $@

dumatestpp.o:	tests/dumatestpp.cpp duma.h duma_sem.h dumapp.h duma_config.h verinfo.h
	$(CXX) $(CPPFLAGS) -c tests/dumatestpp.cpp -o $@

tstheap.o:	tests/tstheap.c duma.h duma_config.h verinfo.h
	$(CC) $(CFLAGS) -c tests/tstheap.c -o $@

testoperators.o:	tests/testoperators.cpp duma.h duma_sem.h dumapp.h duma_config.h verinfo.h
	$(CXX) $(CPPFLAGS) -c tests/testoperators.cpp -o $@

thread-test.o:	tests/thread-test.c duma.h duma_config.h verinfo.h
	$(CC) $(CFLAGS) -c tests/thread-test.c -o $@

testmt.o:	tests/testmt.c duma.h duma_config.h verinfo.h
	$(CC) $(CFLAGS) -c tests/testmt.c -o $@

#
# default rules
#
#.c.o:
#	$(CC) $(CFLAGS) -c $< -o $@
#
#.cpp.o:
#	$(CXX) $(CPPFLAGS) -c $< -o $@
#

.PHONY: check test installcheck install uninstall clean distclean realclean clobber dos2unix printvars printuk printmod FORCE
FORCE:
