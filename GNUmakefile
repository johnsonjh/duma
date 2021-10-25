############################################################################
# vim: filetype=sh:tabstop=4:tw=76
#
# ************  Make Variables  ************
#
# - "VERBOSE" -or- "V"  (example: 'gmake test VERBOSE=1')
#   Set to '1' to enable verbose build output
#
# **********  DUMA Configuration  **********
#
# - Add "-DDUMA_NO_GLOBAL_MALLOC_FREE"  (no quotes)
#     For not defining malloc / free in global namespace
#
# - Add "-DDUMA_EXPLICIT_INIT"  (no quotes)
#     To do all the "risky" stuff: getenv(), sem_init(), write() ...
#       explicitly from main(). You must call duma_init() explicitly
#       from main() as well. This option avoids leak report messages
#       from allocations made before calling duma_init(). This helps
#       avoid leak warnings caused by the standard system environment.
#
# - Add "-DDUMA_NO_THREAD_SAFETY"  (no quotes)
#     For not supporting multi-threading
#
# - Add "-DDUMA_SO_NO_CPP_SUPPORT"  -or-
#     "-DDUMA_LIB_NO_CPP_SUPPORT"  (no quotes)
#       For not directing new / delete to malloc / free
#
# - Add "-DDUMA_SO_NO_LEAKDETECTION"  -or-
#     "-DDUMA_LIB_NO_LEAKDETECTION"  (no quotes)
#       If you want to disable support for leak detection
#         (set by default when building the shared library)
#
# - Add "-DDUMA_SO_PREFER_ATEXIT"  -or-
#     "-DDUMA_LIB_PREFER_ATEXIT"  (no quotes)
#       If you prefer atexit() over the GNU compiler's function
#         attribute "destructor"
#
# - Add "-DDUMA_SO_PREFER_GETENV"  -or-
#     "-DDUMA_LIB_PREFER_GETENV"  (no quotes)
#       If you prefer standard C library getenv() over the global
#         'char **environ'
#
# - Add "-DDUMA_OLD_NEW_MACRO"  (no quotes)
#     If you want to use DUMA's old-style NEW_ELEM and NEW_ARRAY macros
#     When not defining this option, a standards conforming "new" syntax
#       can be used. Unfortunately, you have to use DEL_ELEM / DEL_ARRAY
#       further to utilize filename and line number of deallocation calls
#
# - Add "-DDUMA_SO_NO_HANG_MSG"  -or-
#     "-DDUMA_LIB_NO_HANG_MSG"  (no quotes)
#       Set this if you want to suppress the output from around atexit()
#
# - Add "-DDUMA_NO_STRERROR"  (no quotes)
#     Set this if you want to suppress calling strerror(), which avoids
#       recursion on specific platforms
#
#  - Preprocessor flags for building the shared library (DUMA_SO_LIBRARY):
#     * DUMA_SO_NO_CPP_SUPPORT
#     * DUMA_SO_NO_LEAKDETECTION
#     * DUMA_SO_PREFER_ATEXIT
#     * DUMA_SO_PREFER_GETENV
#     * DUMA_SO_NO_HANG_MSG
#
#  - Preprocessor flags for building the static library:
#     * DUMA_LIB_NO_CPP_SUPPORT
#     * DUMA_LIB_NO_LEAKDETECTION
#     * DUMA_LIB_PREFER_ATEXIT
#     * DUMA_LIB_PREFER_GETENV
#     * DUMA_LIB_NO_HANG_MSG
#     * DUMA_NO_GLOBAL_MALLOC_FREE
#     * DUMA_EXPLICIT_INIT
#     * DUMA_NO_THREAD_SAFETY
#     * DUMA_OLD_NEW_MACRO
#     * DUMA_OLD_DEL_MACRO
#     * DUMA_NO_STRERROR
#
#  **********  Additional DUMA Configuration  **********
#
#   - FreeBSD 5.4:
#       DUMA_OPTIONS += -DPAGE_PROTECTION_VIOLATED_SIGNAL=SIGBUS
#
#   - FreeBSD 5.4 (with DUMA_EXPLICIT_INIT unset):
#       DUMA_OPTIONS += -DDUMA_NO_LEAKDETECTION
#
#   - Cygwin (also define 'WIN32'):
#       DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
#         also define 'WIN32'
#
#  **********  Option Examples  **********
#
#     *  DUMA_OPTIONS += -DDUMA_LIB_NO_LEAKDETECTION
#     *  DUMA_OPTIONS += -DDUMA_NO_THREAD_SAFETY
#     *  DUMA_OPTIONS += -DDUMA_NO_CPP_SUPPORT
#
#  **************  See the README.md for more information  **************

DUMA_OPTIONS=
DUMA_OPTIONS += -DDUMA_SO_NO_LEAKDETECTION

############################################################################
# Configuration: Default Settings

PIC=-fPIC -DPIC
DUMA_SO_OPTIONS=$(PIC) -DDUMA_SO_LIBRARY
DUMA_SO_VERSION=.0.0.0
CURPATH=./
INSTALL=install
MKDIR=mkdir -p
RM=rm
RMFORCE=rm -f
RMDIR=rmdir
ROFF=nroff
ECHO=printf '%s\n'
ECHOLF=printf '%s\n' ""
ENV=env
LN=ln -s
CC=cc
CXX=c++
LD=ld
AR=ar
RANLIB=ranlib
INSTALL=install
DOS2UNIX=dos2unix
UNAME=uname
TR=tr
GREP=grep
TSTVAL=3072
DEVNULL=/dev/null

############################################################################
# Define: V / VERBOSE

ifdef VERBOSE
 ifndef V
  V=$(VERBOSE)
 endif
endif

ifdef V
 ifndef VERBOSE
  VERBOSE=$(V)
 endif
endif

############################################################################
# Define: VERBAN

ifneq ($(VERBOSE), 1)
 VERBAN=DUMA_DISABLE_BANNER=1
else
 VERBAN=
endif

############################################################################
# Define: VECHO

ifneq ($(VERBOSE), 1)
 VECHO=$(ECHO)
else
 VECHO=true
endif

############################################################################
# Define: Automatic OS detection

ifndef $(OS)
OS=$(shell $(UNAME) -s 2> $(DEVNULL) | \
	  $(TR) '[:upper:]' '[:lower:]' 2> $(DEVNULL))
endif

############################################################################
# Define: Automatic GNU Makefile path detection

mkfile_name := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_path := $(dir $(mkfile_name))

############################################################################
# Define: DUMA dynamic dependencies

DUMA_DYN_DEPS=$(DUMASO) \
	          tstheap_so$(EXEPOSTFIX) \
	          dumatestpp_so$(EXEPOSTFIX) \
	          thread-test_so$(EXEPOSTFIX)

############################################################################
# Configuration: Windows (MSYS) - Command Prompt

ifeq ($(OS), windows_nt)
 ifeq ($(OSTYPE), msys)
  $(info *** Using configuration: OS=windows_nt, OSTYPE=msys)
  # DUMA_EXPLICIT_INIT avoid leak reports from __w32_sharedptr_initialize()
  BSWITCH=101
  DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
  MKDIR=md
  RM=del
  RMFORCE=del /F 2>nul
  RMDIR=rd /q
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

############################################################################
# Configuration: Windows (MSYS) - Bourne Shell

 ifeq ($(OSTYPE), msys-sh)
  $(info *** Using configuration: OS=windows_nt, OSTYPE=msys-sh)
  # DUMA_EXPLICIT_INIT avoid leak reports from __w32_sharedptr_initialize()
  BSWITCH=102
  DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
  CC=mingw32-gcc
  CXX=mingw32-g++ -std=c++98
  DUMA_DYN_DEPS=
  DUMASO=
  CFLAGS=-g -O0
  CPPFLAGS=-g -O0
  LIBS=
  EXEPOSTFIX=.exe
 endif

############################################################################
# Configuration: Windows (Cygwin)

 ifeq ($(OSTYPE), cygwin)
  $(info *** Using configuration: OS=windows_nt, OSTYPE=cygwin)
  BSWITCH=103
  DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
  DUMA_DYN_DEPS=
  DUMASO=
  CFLAGS=-g -O0 -DWIN32
  CPPFLAGS=-g -O0 -DWIN32
  LIBS=
  EXEPOSTFIX=.exe
 endif

############################################################################
# Configuration: Windows NT

 ifndef BSWITCH
  $(info *** Using configuration: OS=windows_nt)
  BSWITCH=100
  DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
  DUMA_DYN_DEPS=
  DUMASO=
  CFLAGS=-g -O0 -DWIN32
  CPPFLAGS=-g -O0 -DWIN32
  LIBS=
  EXEPOSTFIX=.exe
 endif
endif

############################################################################
# Configuration: Darwin (macOS X)

ifeq ($(OS), darwin)
 $(info *** Using configuration: OS=darwin)
 BSWITCH=210
 DUMA_OPTIONS += -DPAGE_PROTECTION_VIOLATED_SIGNAL=SIGBUS
 DUMA_OPTIONS += -DDUMA_SO_PREFER_GETENV
 # DUMA_OPTIONS += -DDUMA_LIB_NO_LEAKDETECTION
 # DUMA_DYN_DEPS=
 DUMASO=libduma.dylib
 DUMASO_LINK1=libduma.dylib
 CFLAGS=-g -O0
 CPPFLAGS=-g -O0
 LIBS=-lpthread
 EXEPOSTFIX=
 prefix?=/opt/duma
endif

############################################################################
# Configuration: FreeBSD

ifeq ($(OS), freebsd)
 $(info *** Using configuration: OS=freebsd)
 BSWITCH=310
 # DUMA_OPTIONS += -DDUMA_NO_THREAD_SAFETY
 DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
 DUMA_DYN_DEPS=
 DUMASO=
 DUMASO_LINK1=
 CFLAGS=-g -O0
 CPPFLAGS=-g -O0
 LIBS=-lpthread
 EXEPOSTFIX=
endif

############################################################################
# Configuration: NetBSD

ifeq ($(OS), netbsd)
 $(info *** Using configuration: OS=netbsd)
 BSWITCH=320
 DUMASO=libduma.so$(DUMA_SO_VERSION)
 DUMASO_LINK1=libduma.so.0
 DUMASO_LINK2=libduma.so
 CFLAGS=-g -O0
 CPPFLAGS=-g -O0
 LIBS=-lpthread
 EXEPOSTFIX=
endif

############################################################################
# Configuration: Solaris

ifeq ($(OS), solaris)
 $(info *** Using configuration: OS=solaris)
 BSWITCH=410
 DUMA_OPTIONS += -DDUMA_NO_STRERROR
 DUMA_DYN_DEPS=
 DUMASO=libduma.so$(DUMA_SO_VERSION)
 DUMASO_LINK1=libduma.so.0
 DUMASO_LINK2=libduma.so
 CFLAGS=-g -O0
 CPPFLAGS=-g -O0
 LDFLAGS += -lgcc_s
 LDOPTIONS += -lgcc_s
 LIBS=-Wl,-R/opt/sfw/lib -lpthread
 EXEPOSTFIX=
endif

############################################################################
# Configuration: Linux (detect compiler)

ifeq ($(OS), linux)
ifeq ($(shell $(CXX) -v 2>&1 | $(GREP) -c "clang version" 2> $(DEVNULL)), 1)
  COMPILERX := clang++
 else
  COMPILERX := g++ -std=c++98
 endif
 export COMPILERX
 ifeq ($(shell $(CC) -v 2>&1 | $(GREP) -c "clang version" 2> $(DEVNULL)), 1)
  COMPILER := clang
 else
  COMPILER := gcc
 endif
 export COMPILER
 CC=${COMPILER}
 CXX=${COMPILERX}
 $(info *** Using CC=${COMPILER})
 $(info *** Using CXX=${COMPILERX})
 ifeq ($(OSTYPE), pie)

############################################################################
# Configuration: Linux (PIE)

  $(info *** Using configuration: OS=linux, OSTYPE=pie)
  CC=${COMPILER} -fpie -fPIE
  CXX=${COMPILERX} -fpie -fPIE
  BSWITCH=510
 else

############################################################################
# Configuration: Linux (common, glibc)

  $(info *** Using configuration: OS=linux)
  CC=${COMPILER}
  CXX=${COMPILERX}
  BSWITCH=610
 endif
 DUMA_OPTIONS += -DDUMA_NO_STRERROR
 DUMASO=libduma.so$(DUMA_SO_VERSION)
 DUMASO_LINK1=libduma.so.0
 DUMASO_LINK2=libduma.so
 CFLAGS=-g -O0 -Wall -Wextra
 CFLAGS+=-U_FORTIFY_SOURCE
 CPPFLAGS=-g -O0 -Wall -Wextra
 CPPFLAGS+=-U_FORTIFY_SOURCE
 LIBS=-lpthread
 EXEPOSTFIX=
endif

############################################################################
# Configuration: Generic UNIX

ifndef BSWITCH
 BSWITCH=810
 $(warning ****** Using generic UNIX fallback configuration ******)
 ifndef OS
  $(warning ****** OS unset ******)
 else
  $(warning ****** Unknown OS ******)
 endif
 DUMASO=libduma.so$(DUMA_SO_VERSION)
 DUMASO_LINK1=libduma.so.0
 DUMASO_LINK2=libduma.so
 CFLAGS=-g -O0
 CPPFLAGS=-g -O0
 LIBS=-lpthread
 EXEPOSTFIX=
endif

############################################################################
# Define: HOST_CFLAGS

ifndef HOST_CFLAGS
 HOST_CFLAGS=$(CFLAGS)
endif

############################################################################
# Define: CC_FOR_BUILD

ifndef CC_FOR_BUILD
 CC_FOR_BUILD=$(CC)
endif

############################################################################
# Define: Default prefix

ifndef prefix
 prefix=/usr
 $(info *** Using default prefix [$(prefix)])
endif

############################################################################
# Define: Default documentation installation prefix

MAN_INSTALL_DIR=$(prefix)/share/man/man3
DOC_INSTALL_DIR=$(prefix)/share/doc/duma

############################################################################
# Define: Default srcdir

ifndef srcdir
 ifdef mkfile_path
  srcdir=$(mkfile_path)
  $(info *** Using default srcdir [$(srcdir)])
 endif
endif

ifndef srcdir
 srcdir=./
 $(info *** Using fallback srcdir [$(srcdir)])
endif

############################################################################
# Define: VPATHs (GNU Make search paths)

vpath %.3              $(srcdir) $(CURDIR)
vpath %.cat            $(srcdir) $(CURDIR)
vpath %.$(exec_prefix) $(srcdir) $(srcdir)src $(srcdir)tests $(CURDIR)
vpath %.$(EXEPOSTFIX)  $(srcdir) $(srcdir)src $(srcdir)tests $(CURDIR)
vpath %.h              $(srcdir) $(srcdir)src $(srcdir)tests $(CURDIR)
vpath %.cpp            $(srcdir) $(srcdir)src $(srcdir)tests $(CURDIR)
vpath %.c              $(srcdir) $(srcdir)src $(srcdir)tests $(CURDIR)

############################################################################
# Define: Default exec_prefix

ifndef exec_prefix
 exec_prefix=$(prefix)
 $(info *** Using default exec_prefix [$(exec_prefix)])
endif

############################################################################
# Define: Default bindir

ifndef bindir
 bindir=$(exec_prefix)/bin
endif

############################################################################
# Define: Default datadir

ifndef datadir
 datadir=$(prefix)/share
endif

############################################################################
# Define: Default sysconfdir

ifndef sysconfdir
 sysconfdir=$(prefix)/etc
endif

############################################################################
# Define: Default libdir

ifndef libdir
 libdir=$(exec_prefix)/lib
endif

############################################################################
# Define: Default includedir

ifndef includedir
 includedir=$(prefix)/include
endif

############################################################################
# Define: Package source files

PACKAGE_SOURCE=$(srcdir)README.md \
			   $(srcdir)CHANGELOG.md \
			   $(srcdir)LICENSE \
			   $(srcdir)COPYING-GPL \
			   $(srcdir)COPYING-LGPL \
			   $(srcdir)duma.3 \
			   $(srcdir)GNUmakefile \
			   $(srcdir)gdbinit.rc \
			   $(srcdir)duma.h \
			   $(srcdir)dumapp.h \
			   $(srcdir)duma_sem.h \
			   $(srcdir)paging.h \
			   $(srcdir)print.h \
			   $(srcdir)duma_hlp.h \
			   $(srcdir)noduma.h \
			   $(srcdir)src/duma.c \
			   $(srcdir)src/dumapp.cpp \
			   $(srcdir)src/sem_inc.c \
			   $(srcdir)src/print.c \
			   $(srcdir)tests/dumatest.c \
			   $(srcdir)tests/tstheap.c \
			   $(srcdir)tests/thread-test.c \
			   $(srcdir)tests/testmt.c \
			   $(srcdir)tests/dumatestpp.cpp \
			   $(srcdir)tests/testoperators.cpp \
			   $(srcdir)createconf.c \
			   $(srcdir)make_git_source_version.sh

############################################################################
# Define: OBJECTS

OBJECTS=dumapp.o \
		duma.o \
		sem_inc.o \
		print.o

############################################################################
# Define: SO_OBJECTS

SO_OBJECTS=dumapp_so.o \
		   duma_so.o \
		   sem_inc_so.o \
		   print_so.o

############################################################################
# Target: "all" (default)

.PHONY: all
all: verinfo.h \
	 libduma.a \
	 $(DUMA_DYN_DEPS)
	@ $(ECHO) "*** Build complete"

############################################################################
# Target: "check" / "test"

.PHONY: check test
check test: libduma.a \
	        tstheap$(EXEPOSTFIX) \
	        dumatest$(EXEPOSTFIX) \
	        thread-test$(EXEPOSTFIX) \
	        testmt$(EXEPOSTFIX) \
	        dumatestpp$(EXEPOSTFIX) \
	        testoperators$(EXEPOSTFIX) \
	        testmemlimit$(EXEPOSTFIX) \
	        $(DUMA_DYN_DEPS)
	@ $(ECHOLF)
	@ $(ECHO) "*** Testing DUMA (static library)"
	@ $(VECHO) "	*** Test: dumatest"
	@ $(VERBAN) "$(CURPATH)dumatest$(EXEPOSTFIX)"
	@ $(VECHO) "	*** Test: dumatestpp"
	@ $(VERBAN) "$(CURPATH)dumatestpp$(EXEPOSTFIX)"
	@ $(VECHO) "	*** Test: tstheap $(TSTVAL)"
	@ $(VERBAN) "$(CURPATH)tstheap$(EXEPOSTFIX)" "$(TSTVAL)"
	@ $(VECHO) "	*** Test: thread-test"
	@ $(VERBAN) "$(CURPATH)thread-test$(EXEPOSTFIX)" > $(DEVNULL)
#	@ $(VECHO) "	*** Test: testmt"
#	@ $(VERBAN) "$(CURPATH)testmt$(EXEPOSTFIX)"
	@ $(VECHO) "	*** Test: testoperators"
	@ $(VERBAN) "$(CURPATH)testoperators$(EXEPOSTFIX)"
	@ $(VECHO) "	*** Test: testmemlimit"
	@ $(VERBAN) "$(CURPATH)testmemlimit$(EXEPOSTFIX)"
	@ $(ECHO) "*** DUMA static confidence test PASSED."
ifdef DUMASO
	 @ $(ECHOLF)
	 @ $(ECHO) "*** Testing DUMA (dynamic library)"
	 @ $(VECHO) "	*** Test: dumatestpp_so"
 ifeq ($(OS), solaris)
	  @ LD_PRELOAD="./$(DUMASO)" \
		  DYLD_INSERT_LIBRARIES="./$(DUMASO)" \
		  DYLD_FORCE_FLAT_NAMESPACE=1 \
		  $(VERBAN) exec "$(CURPATH)dumatestpp_so$(EXEPOSTFIX)" \
	      "$(TSTVAL)"
 else
	  @ (export LD_PRELOAD="./$(DUMASO)"; \
		  export DYLD_INSERT_LIBRARIES="./$(DUMASO)"; \
		  export DYLD_FORCE_FLAT_NAMESPACE=1 ; \
		  $(VERBAN) exec "$(CURPATH)dumatestpp_so$(EXEPOSTFIX)" \
	      "$(TSTVAL)")
 endif
	 @ $(VECHO) "	*** Test: tstheap_so $(TSTVAL)"
 ifeq ($(OS), solaris)
	  @ LD_PRELOAD="./$(DUMASO)" \
		  DYLD_INSERT_LIBRARIES="./$(DUMASO)" \
		  DYLD_FORCE_FLAT_NAMESPACE=1 \
		  $(VERBAN) exec "$(CURPATH)tstheap_so$(EXEPOSTFIX)" \
	      "$(TSTVAL)"
 else
	  @ (export LD_PRELOAD="./$(DUMASO)"; \
		  export DYLD_INSERT_LIBRARIES="./$(DUMASO)"; \
		  export DYLD_FORCE_FLAT_NAMESPACE=1 ; \
		  $(VERBAN) exec "$(CURPATH)tstheap_so$(EXEPOSTFIX)" \
	      "$(TSTVAL)")
 endif
	 @ $(VECHO) "	*** Test: thread-test_so"
 ifeq ($(OS), solaris)
	  @ LD_PRELOAD="./$(DUMASO)" \
		  DYLD_INSERT_LIBRARIES="./$(DUMASO)" \
		  DYLD_FORCE_FLAT_NAMESPACE=1 \
		  $(VERBAN) exec "$(CURPATH)thread-test_so$(EXEPOSTFIX)" \
	      > $(DEVNULL)
 else
	  @ (export LD_PRELOAD="./$(DUMASO)"; \
		  export DYLD_INSERT_LIBRARIES="./$(DUMASO)"; \
		  export DYLD_FORCE_FLAT_NAMESPACE=1 ; \
		  $(VERBAN) exec "$(CURPATH)thread-test_so$(EXEPOSTFIX)") \
		  > $(DEVNULL)
 endif
	 @ $(ECHO) "*** DUMA confidence test PASSED."
	 @ $(ECHOLF)
endif

############################################################################
# Target: "installcheck" / "installtest"

.PHONY: installcheck installtest
installcheck installtest: test
ifdef DUMASO
	 @ $(ECHOLF)
	 @ $(ECHO) "*** Testing installed DUMA (dynamic library):"
	 @ $(ECHOLF)
	 @ $(VECHO) "	*** Test: tstheap_so $(TSTVAL)"
	 @ $(VERBAN) "$(bindir)/duma" "$(CURPATH)tstheap_so" "$(TSTVAL)"
	 @ $(ECHO) "*** DUMA installcheck test PASSED."
	 @ $(ECHOLF)
endif

############################################################################
# Target: "printvars" / "printenv" (display variables of GNUmakefile)

.PHONY: printvars printenv
printvars printenv:
	@echo "OS              	[$(OS)]"
	@echo "OSTYPE          	[$(OSTYPE)]"
	@echo "bswitch         	[$(BSWITCH)]"
	@echo "srcdir          	[$(srcdir)]"
	@echo "prefix          	[$(prefix)]"
	@echo "exec_prefix     	[$(exec_prefix)]"
	@echo "bindir          	[$(bindir)]"
	@echo "datadir         	[$(datadir)]"
	@echo "sysconfdir      	[$(sysconfdir)]"
	@echo "libdir          	[$(libdir)]"
	@echo "includedir      	[$(includedir)]"
	@echo "oldincludedir   	[$(oldincludedir)]"
	@echo "CURDIR          	[$(CURDIR)]"
	@echo "MAN_INSTALL_DIR 	[$(MAN_INSTALL_DIR)]"
	@echo "DOC_INSTALL_DIR 	[$(DOC_INSTALL_DIR)]"
	@echo "VERBOSE         	[$(VERBOSE)]"
	@echo "V               	[$(V)]"
	@echo "VERBAN          	[$(VERBAN)]"
	@echo "MKDIR           	[$(MKDIR)]"
	@echo "MAKE            	[$(MAKE)]"
	@echo "CC              	[$(CC)]"
	@echo "CFLAGS          	[$(CFLAGS)]"
	@echo "CXX             	[$(CXX)]"
	@echo "CPPFLAGS        	[$(CPPFLAGS)]"
	@echo "LD              	[$(LD)]"
	@echo "AR              	[$(AR)]"
	@echo "LIBS            	[$(LIBS)]"
	@echo "RANLIB          	[$(RANLIB)]"
	@echo "INSTALL         	[$(INSTALL)]"
	@echo "RM              	[$(RM)]"
	@echo "RMFORCE         	[$(RMFORCE)]"
	@echo "RMDIR           	[$(RMDIR)]"
	@echo "VECHO           	[$(VECHO)]"
	@echo "ECHO            	[$(ECHO)]"
	@echo "ECHOLF          	[$(ECHOLF)]"
	@echo "PIC             	[$(PIC)]"
	@echo "EXEPOSTFIX      	[$(EXEPOSTFIX)]"
	@echo "CURPATH         	[$(CURPATH)]"
	@echo "DUMA_OPTIONS    	[$(DUMA_OPTIONS)]"
	@echo "DUMA_SO_OPTIONS 	[$(DUMA_SO_OPTIONS)]"
	@echo "OBJECTS         	[$(OBJECTS)]"
	@echo "SO_OBJECTS      	[$(SO_OBJECTS)]"
	@echo "DUMA_SO_VERSION 	[$(DUMA_SO_VERSION)]"
	@echo "DUMASO          	[$(DUMASO)]"
	@echo "DUMASO_LINK1    	[$(DUMASO_LINK1)]"
	@echo "DUMASO_LINK2    	[$(DUMASO_LINK2)]"
	@echo "DUMA_DYN_DEPS   	[$(DUMA_DYN_DEPS)]"
	@echo "PACKAGE_SOURCE  	[$(PACKAGE_SOURCE)]"

############################################################################
# Target "printuk" (display files unknown to git)

.PHONY: printuk
printuk:
	@ $(ECHO) "*** Begin printuk (git untracked files):"
	@ git status -s --untracked-files="all" 2> $(DEVNULL) | \
	    $(GREP) '^? ' || true
	@ $(ECHO) "*** End printuk"

############################################################################
# Target: "printmod" (display files known to git but not up-to-date)

.PHONY: printmod
printmod:
	@ $(ECHO) "*** Begin printmod (git tracked modified files)"
	@ git status -s 2> $(DEVNULL) | \
	    $(GREP) '^\ \?M ' || true
	@ $(ECHO) "*** End printmod"

############################################################################
# Target: "install" (installs DUMA, respecting DESTDIR variable)

.PHONY: install
install: libduma.a \
	     duma.3 \
	     duma_config.h \
	     $(DUMASO)
	- $(MKDIR) "$(DESTDIR)$(DOC_INSTALL_DIR)"
	$(INSTALL) -m 644 \
		"$(srcdir)README.md" \
		"$(DESTDIR)$(DOC_INSTALL_DIR)"
	- $(MKDIR) "$(DESTDIR)$(includedir)"
	$(INSTALL) -m 644 \
		"$(srcdir)noduma.h" \
		"$(srcdir)duma.h" \
		"$(srcdir)dumapp.h" \
		"$(srcdir)duma_sem.h" \
		"$(CURDIR)/duma_config.h" \
		"$(DESTDIR)$(includedir)"
ifdef DUMASO
	 - $(MKDIR) "$(DESTDIR)$(bindir)"
	 $(INSTALL) -m 755 \
		 "$(srcdir)duma.sh" \
		 "$(DESTDIR)$(bindir)/duma"
endif
	- $(MKDIR) "$(DESTDIR)$(libdir)"
	$(INSTALL) -m 644 \
		"$(CURDIR)/libduma.a" \
		"$(DESTDIR)$(libdir)"
ifdef DUMASO
	 $(INSTALL) -m 755 \
		 "$(CURDIR)/$(DUMASO)" \
		 "$(DESTDIR)$(libdir)"
endif
ifdef DUMASO_LINK1
	 - $(RMFORCE) "$(DESTDIR)$(libdir)/$(DUMASO_LINK1)"
	 $(LN) \
		 "$(DUMASO)" \
		 "$(DESTDIR)$(libdir)/$(DUMASO_LINK1)"
endif
ifdef DUMASO_LINK2
	 - $(RMFORCE) "$(DESTDIR)$(libdir)/$(DUMASO_LINK2)"
	 $(LN) \
		 "$(DUMASO)" \
		 "$(DESTDIR)$(libdir)/$(DUMASO_LINK2)"
endif
	 - $(MKDIR) "$(DESTDIR)$(MAN_INSTALL_DIR)"
	 $(INSTALL) -m 644 \
		 "$(srcdir)duma.3" \
		 "$(DESTDIR)$(MAN_INSTALL_DIR)/duma.3"

############################################################################
# Target: "uninstall" (uninstalls DUMA, respects DESTDIR variable)

.PHONY: uninstall
uninstall:
	- $(RMFORCE) "$(DESTDIR)$(DOC_INSTALL_DIR)/README.md"
	- $(RMFORCE) "$(DESTDIR)$(includedir)/noduma.h"
	- $(RMFORCE) "$(DESTDIR)$(includedir)/duma.h"
	- $(RMFORCE) "$(DESTDIR)$(includedir)/dumapp.h"
	- $(RMFORCE) "$(DESTDIR)$(includedir)/duma_sem.h"
	- $(RMFORCE) "$(DESTDIR)$(includedir)/duma_config.h"
	- $(RMFORCE) "$(DESTDIR)$(bindir)/duma"
	- $(RMFORCE) "$(DESTDIR)$(libdir)/libduma.a"
ifdef DUMASO
	 - $(RMFORCE) "$(DESTDIR)$(libdir)/$(DUMASO)"
endif
ifdef DUMASO_LINK1
	 - $(RMFORCE) "$(DESTDIR)$(libdir)/$(DUMASO_LINK1)"
endif
ifdef DUMASO_LINK2
	 - $(RMFORCE) "$(DESTDIR)$(libdir)/$(DUMASO_LINK2)"
endif
	- $(RMFORCE) "$(DESTDIR)$(MAN_INSTALL_DIR)/duma.3"

############################################################################
# Target: "clean" (deletes make output; saves configuration artifacts)

.PHONY: clean
clean:
	- $(RMFORCE) core a.out $(OBJECTS) \
	   $(SO_OBJECTS) \
	   tstheap.o dumatest.o thread-test.o testmt.o dumatestpp.o \
	   tstheap_so.o dumatestpp_so.o testoperators.o testmemlimit.o \
	   testmemlimit$(EXEPOSTFIX) thread-test_so$(EXEPOSTFIX) \
	   tstheap$(EXEPOSTFIX) tstheap_so$(EXEPOSTFIX) \
	   dumatest$(EXEPOSTFIX) dumatestpp$(EXEPOSTFIX) \
	   dumatestpp_so$(EXEPOSTFIX) testoperators$(EXEPOSTFIX) \
	   thread-test$(EXEPOSTFIX) testmt$(EXEPOSTFIX) \
	   libduma.a $(DUMASO) thread-test_so.o libduma.cat

############################################################################
# Target: "distclean" / "realclean" / "clobber" (deletes all make output)

.PHONY: distclean realclean clobber clean
distclean realclean clobber: clean
	- $(RMFORCE) duma_config.h verinfo.h createconf.o \
	   createconf$(EXEPOSTFIX) CMakeCache.txt CMakeFiles/cmake.*cache \
	   "$(srcdir)CMakeFiles/"cmake.*cache
	@ $(RMDIR) "$(srcdir)CMakeFiles" > $(DEVNULL) 2>&1 || true
	@ $(RMDIR) "$(CURDIR)/CMakeFiles" > $(DEVNULL) 2>&1 || true

############################################################################
# Target: "libduma.cat"

libduma.cat: roff

############################################################################
# Target: "roff" (renders catman documentation from roff source)

.PHONY: roff
roff: duma.3
	$(ROFF)  -man < "$(srcdir)duma.3" > "$(CURDIR)/libduma.cat"

############################################################################
# Target: "libduma.a"

libduma.a: duma_config.h \
	       verinfo.h \
	       $(OBJECTS)
	- $(RMFORCE) libduma.a
	$(AR) crv \
	 libduma.a \
	 $(OBJECTS)
	$(RANLIB) libduma.a

############################################################################
# Target: "duma.sh"

# Placeholder to process duma.sh.in to duma.sh

############################################################################
# Target: "verinfo.h"

verinfo.h:
	"$(srcdir)make_git_source_version.sh" || \
	  $(ENV) sh "$(srcdir)make_git_source_version.sh" || \
	   $(SHELL) "$(srcdir)make_git_source_version.sh" || \
	    touch "$(CURPATH)verinfo.h"

############################################################################
# Target: "duma_config.h"

duma_config.h:
	$(MAKE) -f "$(mkfile_name)" reconfig

############################################################################
# Target: "reconfig"

.PHONY: reconfig
reconfig: verinfo.h \
	      createconf$(EXEPOSTFIX) \
	      createconf.o \
	      createconf.c
	- "$(CURPATH)createconf$(EXEPOSTFIX)"

############################################################################
# Target: "dos2unix" (convert source line endings)

.PHONY: dos2unix
dos2unix:
	$(DOS2UNIX) $(PACKAGE_SOURCE)

############################################################################
# Target: "createconf"

createconf$(EXEPOSTFIX): createconf.o
	- $(RMFORCE) createconf$(EXEPOSTFIX)
	$(CC_FOR_BUILD) $(HOST_CFLAGS) $(DUMA_OPTIONS) \
	  createconf.o \
	  -o createconf$(EXEPOSTFIX)

############################################################################
# Target: "tstheap"

tstheap$(EXEPOSTFIX): libduma.a \
	                  tstheap.o
	- $(RMFORCE) tstheap$(EXEPOSTFIX)
	$(CC) $(CFLAGS) -I"./" \
	  tstheap.o libduma.a \
	  -o tstheap$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "dumatest"

dumatest$(EXEPOSTFIX): libduma.a \
	                   dumatest.o
	- $(RMFORCE) dumatest$(EXEPOSTFIX)
	$(CC) $(CFLAGS) -I"./" \
	  dumatest.o libduma.a \
	  -o dumatest$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "dumatestpp"

dumatestpp$(EXEPOSTFIX): libduma.a \
	                     dumatestpp.o \
                         duma_sem.h \
                         dumapp.h
	- $(RMFORCE) dumatestpp$(EXEPOSTFIX)
	$(CXX) -I"./" $(CPPFLAGS) \
	  dumatestpp.o libduma.a \
	  -o dumatestpp$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "thread-test"

thread-test$(EXEPOSTFIX): libduma.a \
	                      thread-test.o
	- $(RMFORCE) thread-test$(EXEPOSTFIX)
	$(CC) -I"./" $(CFLAGS) \
	  thread-test.o libduma.a \
	  -o thread-test$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "thread-test_so"

thread-test_so$(EXEPOSTFIX): thread-test_so.o
	- $(RMFORCE) thread-test_so$(EXEPOSTFIX)
	$(CC) -I"./" $(CFLAGS) \
	  thread-test_so.o \
	  -o thread-test_so$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "testmt"

testmt$(EXEPOSTFIX): libduma.a \
	                 testmt.o
	- $(RMFORCE) testmt$(EXEPOSTFIX)
	$(CC) -I"./" $(CFLAGS) \
      testmt.o libduma.a \
	  -o testmt$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "testoperators"

testoperators$(EXEPOSTFIX): libduma.a \
	                        testoperators.o \
	                        duma_sem.h \
	                        dumapp.h
	- $(RMFORCE) testoperators$(EXEPOSTFIX)
	$(CXX) -I"./" $(CPPFLAGS) \
	  testoperators.o libduma.a \
	  -o testoperators$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "testmemlimit"

testmemlimit$(EXEPOSTFIX): libduma.a \
	                       testmemlimit.o
	- $(RMFORCE) testmemlimit$(EXEPOSTFIX)
	$(CC) -I"./" $(CFLAGS) \
	  testmemlimit.o libduma.a \
	  -o testmemlimit$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "tstheap_so"

tstheap_so$(EXEPOSTFIX): tstheap_so.o
	- $(RMFORCE) tstheap_so$(EXEPOSTFIX)
	$(CC) $(CFLAGS) -I"./" \
	  tstheap_so.o \
	  -o tstheap_so$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "dumatestpp_so"

dumatestpp_so$(EXEPOSTFIX): dumatestpp_so.o
	- $(RMFORCE) dumatestpp_so$(EXEPOSTFIX)
	$(CXX) -I"./" $(CPPFLAGS) \
	  dumatestpp_so.o \
	  -o dumatestpp_so$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "testmemlimit_so"

testmemlimit_so$(EXEPOSTFIX): testmemlimit_so.o
	- $(RMFORCE) testmemlimit_so$(EXEPOSTFIX)
	$(CC) -I"./" $(CFLAGS) \
	  testmemlimit_so.o \
	  -o testmemlimit_so$(EXEPOSTFIX) $(LIBS)

############################################################################
# Target: "$(OBJECTS)"

$(OBJECTS) tstheap.o dumatest.o thread-test.o testmt.o dumatestpp.o: duma.h

############################################################################
# Target: "$(DUMASO)" (Windows NT)

ifeq ($(OS), windows_nt)
 # do nothing
else
 ifeq ($(OS), darwin)

############################################################################
# Target: "$(DUMASO)" (Darwin / macOS / OS X)

  $(DUMASO): duma_config.h \
	         verinfo.h \
	         $(SO_OBJECTS)
	  $(CXX) -g -dynamiclib -Wl \
	    -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
	  $(CXX) -g -dynamiclib \
	    -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
 else

############################################################################
# Target: "$(DUMASO)" (UNIX default)

  $(DUMASO): duma_config.h \
	         verinfo.h \
		     $(SO_OBJECTS)
	  $(CXX) -g -shared -Wl,-soname,$(DUMASO) \
	    -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
#	  $(CXX) -g -shared \
#	    -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
 endif
endif

############################################################################
# Target: "createconf.o" 

createconf.o:
	$(CC_FOR_BUILD) $(HOST_CFLAGS) $(DUMA_OPTIONS) \
	  -c "$(srcdir)"createconf.c \
	  -o $@

############################################################################
# Target: "dumapp_so.o"

dumapp_so.o: src/dumapp.cpp \
	         duma.h \
	         duma_sem.h \
	         dumapp.h
	$(CXX) -I"$(srcdir)" -I"./" $(CPPFLAGS) $(DUMA_SO_OPTIONS) \
	  -c "$(srcdir)src/dumapp.cpp" \
	  -o $@

############################################################################
# Target: "duma_so.o"

duma_so.o: src/duma.c \
	       duma.h \
	       duma_config.h \
	       verinfo.h
	$(CC) -I"$(srcdir)" -I"./" $(CFLAGS) $(DUMA_SO_OPTIONS) \
	  -c "$(srcdir)src/duma.c" \
	  -o $@

############################################################################
# Target: "sem_inc_so.o"

sem_inc_so.o: src/sem_inc.c \
	          duma_sem.h
	$(CC) -I"$(srcdir)" -I"./" $(CFLAGS) $(DUMA_SO_OPTIONS) \
	  -c "$(srcdir)src/sem_inc.c" \
	  -o $@

############################################################################
# Target: "print_so.o"

print_so.o:	src/print.c \
	        print.h
	$(CC) -I"$(srcdir)" -I"./" $(CFLAGS) $(DUMA_SO_OPTIONS) \
	  -c "$(srcdir)src/print.c" \
	  -o $@

############################################################################
# Target: "tstheap_so.o"

tstheap_so.o:
	$(CC) $(CFLAGS) -I"./" $(DUMA_SO_OPTIONS) \
	  -c "$(srcdir)tests/tstheap.c" \
	  -o $@

############################################################################
# Target: "dumatestpp_so.o"

dumatestpp_so.o:
	$(CXX) $(CPPFLAGS) $(DUMA_SO_OPTIONS) \
	  -c "$(srcdir)tests/dumatestpp.cpp" \
	  -o $@

############################################################################
# Target: "testmemlimit_so.o"

testmemlimit_so.o:
	$(CC) $(CFLAGS) $(DUMA_SO_OPTIONS) \
	  -c "$(srcdir)tests/testmemlimit.c" \
	  -o $@

############################################################################
# Target: "testmemlimit.o"

testmemlimit.o:
	$(CC) $(CFLAGS) $(DUMA_SO_OPTIONS) \
	  -c "$(srcdir)tests/experimental/testmemlimit.c" \
	  -o $@

############################################################################
# Target: "dumapp.o"

dumapp.o: src/dumapp.cpp \
	      duma.h \
	      duma_sem.h \
	      dumapp.h
	$(CXX) -I"$(srcdir)" -I"./" $(CPPFLAGS) \
	  -c "$(srcdir)src/dumapp.cpp" \
	  -o $@

############################################################################
# Target: "duma.o"

duma.o:	src/duma.c \
	    duma.h \
	    duma_config.h \
	    verinfo.h
	$(CC) -I"$(srcdir)" -I"./" $(CFLAGS) \
	  -c "$(srcdir)src/duma.c" \
	  -o $@

############################################################################
# Target: "sem_inc.o"

sem_inc.o: src/sem_inc.c \
	       duma_sem.h
	$(CC) -I"$(srcdir)" -I"./" $(CFLAGS) \
	  -c "$(srcdir)src/sem_inc.c" \
	  -o $@

############################################################################
# Target: "print.o"

print.o: src/print.c \
	     print.h
	$(CC) -I"$(srcdir)" -I"./" $(CFLAGS) \
	  -c "$(srcdir)src/print.c" \
	  -o $@

############################################################################
# Target: "dumatest.o"

dumatest.o: tests/dumatest.c \
	        duma.h \
	        duma_config.h \
	        verinfo.h
	$(CC) $(CFLAGS) -I"./" \
	  -c "$(srcdir)tests/dumatest.c" \
	  -o $@

############################################################################
# Target: "dumatestpp.o"

dumatestpp.o: tests/dumatestpp.cpp \
	          duma.h \
	          duma_sem.h \
	          dumapp.h \
	          duma_config.h \
	          verinfo.h
	$(CXX) -I"./" $(CPPFLAGS) \
	  -c "$(srcdir)tests/dumatestpp.cpp" \
	  -o $@

############################################################################
# Target: "tstheap.o"

tstheap.o: tests/tstheap.c \
	       duma.h \
	       duma_config.h \
	       verinfo.h
	$(CC) -I"./" $(CFLAGS) -I"./" \
	  -c "$(srcdir)tests/tstheap.c" \
	  -o $@

############################################################################
# Target: "testoperators.o"

testoperators.o: tests/testoperators.cpp \
	             duma.h \
	             duma_sem.h \
	             dumapp.h \
	             duma_config.h \
	             verinfo.h
	$(CXX) -I"./" $(CPPFLAGS) \
	  -c "$(srcdir)tests/testoperators.cpp" \
	  -o $@

############################################################################
# Target: "thread-test.o"

thread-test.o: tests/thread-test.c \
	           duma.h \
	           duma_config.h \
	           verinfo.h
	$(CC) -I"./" $(CFLAGS) \
	  -c "$(srcdir)tests/thread-test.c" \
	  -o $@

############################################################################
# Target: "thread-test.o"

thread-test_so.o: tests/thread-test.c \
	              duma.h \
	              duma_config.h \
	              verinfo.h
	$(CC) -I"./" $(CFLAGS) \
	  -c "$(srcdir)tests/thread-test.c" \
	  -o $@

############################################################################
# Target: "testmt.o"

testmt.o: tests/testmt.c \
	      duma.h \
	      duma_config.h \
	      verinfo.h
	$(CC) -I"./" $(CFLAGS) \
	  -c "$(srcdir)tests/testmt.c" \
	  -o $@

############################################################################
# Target: FORCE

.PHONY: FORCE
FORCE:

############################################################################

# Local Variables:
# mode: makefile
# tab-width: 4
# End:
