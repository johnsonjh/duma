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
# add "-DDUMA_SEMAPHORES" (without quotes)
#    for using pthread semaphores
#    elseway pthread mutexes get used
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
DUMA_OPTIONS =

# no leak detection on any platform:
# use static library for finding leaks - with information memory was allocated
# i think leak-checking without further information is quite useless!
# additionaly too many platforms/environments are broken.
DUMA_OPTIONS += -DDUMA_SO_NO_LEAKDETECTION

# some test cases:
#DUMA_OPTIONS += -DDUMA_LIB_NO_LEAKDETECTION
#DUMA_OPTIONS += -DDUMA_NO_THREAD_SAFETY
#DUMA_OPTIONS += -DDUMA_NO_CPP_SUPPORT

PIC= -fPIC -DPIC
DUMA_SO_OPTIONS = $(PIC) -DDUMA_SO_LIBRARY

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
CC=gcc
CXX=g++
AR=ar
RANLIB=ranlib
INSTALL=install
RM=rm
RMFORCE=rm -f
ECHO=echo
ECHOLF=echo

# dynamic dependencies
DUMA_DYN_DEPS = $(DUMASO) tstheap_so$(EXEPOSTFIX) dumatestpp_so$(EXEPOSTFIX)

# some OS specific:

ifeq ($(OS), Windows_NT)
  ifeq ($(OSTYPE), msys)
    # call mingw32-make OSTYPE=msys
    # from Windows command prompt
    # having added the PATH for MINGW/bin
    # using explicit initialization to avoid leak report
    # from __w32_sharedptr_initialize() function
    DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
    RM=del
    RMFORCE=del /F 2>nul
    ECHO=echo
    ECHOLF=echo .
    CURPATH=
    DUMA_DYN_DEPS=
    DUMASO=
    CFLAGS= -g -O0
    CPPFLAGS= -g -O0
    LIBS=
    EXEPOSTFIX=.exe
  else
    ifeq ($(OSTYPE), cygwin)
      # call make OSTYPE=cygwin
      DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
      CURPATH=./
      DUMA_DYN_DEPS=
      DUMASO=
      CFLAGS= -g -O0 -DWIN32
      CPPFLAGS= -g -O0 -DWIN32
      LIBS=
      EXEPOSTFIX=.exe
    else
      DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
      CURPATH=./
      DUMA_DYN_DEPS=
      DUMASO=
      CFLAGS= -g -O0 -DWIN32
      CPPFLAGS= -g -O0 -DWIN32
      LIBS=
      EXEPOSTFIX=.exe
    endif
  endif
else
  ifeq ($(OS), osx)
    # tested on darwin 8.0, which is the base for mac-osx
    # call: make OS=osx
    DUMA_OPTIONS += -DPAGE_PROTECTION_VIOLATED_SIGNAL=SIGBUS
    DUMA_OPTIONS += -DDUMA_SO_PREFER_GETENV
#    DUMA_OPTIONS += -DDUMA_LIB_NO_LEAKDETECTION
    CURPATH=./
#    DUMA_DYN_DEPS=
    DUMASO=libduma.dylib
    DUMASO_LINK1=libduma.dylib
    CFLAGS= -g -O0
    CPPFLAGS= -g -O0
    LIBS=-lpthread
    EXEPOSTFIX=
  else
    ifeq ($(OS), freebsd)
      DUMA_OPTIONS += -DDUMA_NO_THREAD_SAFETY
      DUMA_OPTIONS += -DDUMA_EXPLICIT_INIT
      CURPATH=./
      DUMA_DYN_DEPS=
      DUMASO=
      DUMASO_LINK1=
      CFLAGS= -g -O0
      CPPFLAGS= -g -O0
      LIBS=-lpthread
      EXEPOSTFIX=
    else
      ifeq ($(OS), netbsd)
          CURPATH=./
	  DUMASO=libduma.so.0.0.0
          DUMASO_LINK1=libduma.so.0
          DUMASO_LINK2=libduma.so
          CFLAGS= -g -O0
          CPPFLAGS= -g -O0
          LIBS=-lpthread
          EXEPOSTFIX=
      else
        ifeq ($(OS), solaris)
          DUMA_OPTIONS += -DDUMA_NO_STRERROR
          CURPATH=./
          DUMA_DYN_DEPS=
          DUMASO=libduma.so.0.0.0
          DUMASO_LINK1=libduma.so.0
          DUMASO_LINK2=libduma.so
          CFLAGS= -g -O0
          CPPFLAGS= -g -O0
          LDFLAGS+=-lgcc_s
          LDOPTIONS+=-lgcc_s
          LIBS=-Wl,-R/opt/sfw/lib -lpthread
          EXEPOSTFIX=
        else
          # default is Linux or other full Unix
          CURPATH=./
          DUMASO=libduma.so.0.0.0
          DUMASO_LINK1=libduma.so.0
          DUMASO_LINK2=libduma.so
          CFLAGS= -g -O0 -Wall
          CPPFLAGS= -g -O0 -Wall
          LIBS=-lpthread
          EXEPOSTFIX=
        endif
      endif
    endif
  endif
endif


############################################################

prefix=/usr
BIN_INSTALL_DIR=$(prefix)/bin
LIB_INSTALL_DIR=$(prefix)/lib
MAN_INSTALL_DIR=$(prefix)/share/man/man3
INC_INSTALL_DIR=$(prefix)/include
DOC_INSTALL_DIR=$(prefix)/share/doc/duma

PACKAGE_SOURCE= README.txt CHANGELOG COPYING-GPL COPYING-LGPL duma.3 Makefile \
	duma.h dumapp.h sem_inc.h paging.h print.h duma_hlp.h noduma.h \
	duma.c dumapp.cpp sem_inc.c print.c \
	dumatest.c tstheap.c thread-test.c testmt.c dumatestpp.cpp testoperators.cpp \
	createconf.c

OBJECTS = dumapp.o duma.o sem_inc.o print.o

SO_OBJECTS = dumapp_so.o duma_so.o sem_inc_so.o print_so.o

all:	libduma.a tstheap$(EXEPOSTFIX) dumatest$(EXEPOSTFIX) thread-test$(EXEPOSTFIX) testmt$(EXEPOSTFIX) dumatestpp$(EXEPOSTFIX) testoperators$(EXEPOSTFIX) $(DUMA_DYN_DEPS)

test:
	@ $(ECHOLF)
	@ $(ECHO) "Testing DUMA (static library):"
	$(CURPATH)dumatest$(EXEPOSTFIX)
	$(CURPATH)tstheap$(EXEPOSTFIX) 3072
	$(CURPATH)testoperators$(EXEPOSTFIX)
	@ $(ECHOLF)
	@ $(ECHO) "DUMA static confidence test PASSED."
ifdef DUMASO
	@ $(ECHOLF)
	@ $(ECHO) "Testing DUMA (dynamic library)."
ifeq ($(OS), solaris)
	(LD_PRELOAD=./$(DUMASO) DYLD_INSERT_LIBRARIES=./$(DUMASO) DYLD_FORCE_FLAT_NAMESPACE=1 exec $(CURPATH)tstheap_so 3072)
else
	(export LD_PRELOAD=./$(DUMASO); export DYLD_INSERT_LIBRARIES=./$(DUMASO); export DYLD_FORCE_FLAT_NAMESPACE=1; exec $(CURPATH)tstheap_so 3072)
endif
	@ $(ECHOLF)
	@ $(ECHO) "DUMA dynamic confidence test PASSED."
	@ $(ECHOLF)
endif

install: libduma.a duma.3 $(DUMASO)
	- mkdir -p $(DESTDIR)$(DOC_INSTALL_DIR)
	$(INSTALL) -m 644 README.txt $(DESTDIR)$(DOC_INSTALL_DIR)
	- mkdir -p $(DESTDIR)$(INC_INSTALL_DIR)
	$(INSTALL) -m 644 noduma.h duma.h dumapp.h duma_config.h $(DESTDIR)$(INC_INSTALL_DIR)
	- mkdir -p $(DESTDIR)$(BIN_INSTALL_DIR)
	$(INSTALL) -m 755 duma.sh $(DESTDIR)$(BIN_INSTALL_DIR)/duma
	- mkdir -p $(DESTDIR)$(LIB_INSTALL_DIR)
	$(INSTALL) -m 644 libduma.a $(DESTDIR)$(LIB_INSTALL_DIR)
ifdef DUMASO
	$(INSTALL) -m 755 $(DUMASO) $(DESTDIR)$(LIB_INSTALL_DIR)
endif
ifdef DUMASO_LINK1
	- $(RMFORCE) $(DESTDIR)$(LIB_INSTALL_DIR)/$(DUMASO_LINK1)
	ln -s $(DUMASO) $(DESTDIR)$(LIB_INSTALL_DIR)/$(DUMASO_LINK1)
endif
ifdef DUMASO_LINK2
	- $(RMFORCE) $(DESTDIR)$(LIB_INSTALL_DIR)/$(DUMASO_LINK2)
	ln -s $(DUMASO) $(DESTDIR)$(LIB_INSTALL_DIR)/$(DUMASO_LINK2)
endif
	- mkdir -p $(DESTDIR)$(MAN_INSTALL_DIR)
	$(INSTALL) -m 644 duma.3 $(DESTDIR)$(MAN_INSTALL_DIR)/duma.3

clean:
	- $(RMFORCE) $(OBJECTS) $(SO_OBJECTS) tstheap.o dumatest.o thread-test.o testmt.o dumatestpp.o \
		tstheap_so.o dumatestpp_so.o testoperators.o \
		tstheap$(EXEPOSTFIX) tstheap_so$(EXEPOSTFIX) dumatest$(EXEPOSTFIX) dumatestpp$(EXEPOSTFIX) dumatestpp_so$(EXEPOSTFIX) testoperators$(EXEPOSTFIX) \
		thread-test$(EXEPOSTFIX) testmt$(EXEPOSTFIX) \
		libduma.a $(DUMASO) libduma.cat DUMA.shar 

distclean clobber: clean
	- $(RMFORCE) duma_config.h createconf.o createconf$(EXEPOSTFIX)

roff:
	nroff -man < duma.3 > duma.cat


DUMA.shar: $(PACKAGE_SOURCE)
	shar $(PACKAGE_SOURCE) > DUMA.shar

shar: DUMA.shar

libduma.a: duma_config.h $(OBJECTS)
	- $(RMFORCE) libduma.a
	$(AR) crv libduma.a $(OBJECTS)
	$(RANLIB) libduma.a

duma_config.h: 
	$(MAKE) reconfig

reconfig: createconf$(EXEPOSTFIX) createconf.o createconf.c
	- $(CURPATH)createconf$(EXEPOSTFIX)

dos2unix:
	@ dos2unix --d2u $(PACKAGE_SOURCE)

createconf$(EXEPOSTFIX): createconf.o
	- $(RMFORCE) createconf$(EXEPOSTFIX)
	$(CC) $(CFLAGS) $(DUMA_OPTIONS) createconf.o -o createconf$(EXEPOSTFIX)

tstheap$(EXEPOSTFIX): libduma.a tstheap.o
	- $(RMFORCE) tstheap$(EXEPOSTFIX)
	$(CC) $(CFLAGS) tstheap.o libduma.a -o tstheap$(EXEPOSTFIX) $(LIBS)

dumatest$(EXEPOSTFIX): libduma.a dumatest.o
	- $(RMFORCE) dumatest$(EXEPOSTFIX)
	$(CC) $(CFLAGS) dumatest.o libduma.a -o dumatest$(EXEPOSTFIX) $(LIBS)

dumatestpp$(EXEPOSTFIX): libduma.a dumatestpp.o dumapp.h
	- $(RMFORCE) dumatestpp$(EXEPOSTFIX)
	$(CXX) $(CPPFLAGS) dumatestpp.o libduma.a -o dumatestpp$(EXEPOSTFIX) $(LIBS)

thread-test$(EXEPOSTFIX): libduma.a thread-test.o
	- $(RMFORCE) thread-test$(EXEPOSTFIX)
	$(CC) $(CFLAGS) thread-test.o libduma.a -o thread-test$(EXEPOSTFIX) $(LIBS)

testmt$(EXEPOSTFIX): libduma.a testmt.o
	- $(RMFORCE) testmt$(EXEPOSTFIX)
	$(CC) $(CFLAGS) testmt.o libduma.a -o testmt$(EXEPOSTFIX) $(LIBS)

testoperators$(EXEPOSTFIX): libduma.a testoperators.o dumapp.h
	- $(RMFORCE) testoperators$(EXEPOSTFIX)
	$(CXX) $(CPPFLAGS) testoperators.o libduma.a -o testoperators$(EXEPOSTFIX) $(LIBS)

tstheap_so$(EXEPOSTFIX): tstheap_so.o
	- $(RMFORCE) tstheap_so$(EXEPOSTFIX)
	$(CC) $(CFLAGS) tstheap_so.o -o tstheap_so$(EXEPOSTFIX) $(LIBS)

dumatestpp_so$(EXEPOSTFIX): dumatestpp_so.o
	- $(RMFORCE) dumatestpp_so$(EXEPOSTFIX)
	$(CXX) $(CPPFLAGS) dumatestpp_so.o -o dumatestpp_so$(EXEPOSTFIX) $(LIBS)


$(OBJECTS) tstheap.o dumatest.o thread-test.o testmt.o dumatestpp.o: duma.h

ifeq ($(OS), Windows_NT)
  # do nothing
else
  ifeq ($(OS), osx)

$(DUMASO): duma_config.h $(SO_OBJECTS)
	$(CXX) -g -dynamiclib -Wl -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
	$(CXX) -g -dynamiclib -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc

  else

$(DUMASO): duma_config.h $(SO_OBJECTS)
	$(CXX) -g -shared -Wl,-soname,$(DUMASO) -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
#	$(CXX) -g -shared -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc

  endif

endif


#
# define rules how to build objects for createconf
#
createconf.o:
	$(CC) $(CFLAGS) $(DUMA_OPTIONS) -c createconf.c -o $@


#
# define rules how to build objects for shared library
#

dumapp_so.o:	dumapp.cpp duma.h dumapp.h
	$(CXX) $(CPPFLAGS) $(DUMA_SO_OPTIONS) -c dumapp.cpp -o $@

duma_so.o:	duma.c duma.h duma_config.h
	$(CC) $(CFLAGS) $(DUMA_SO_OPTIONS) -c duma.c -o $@

sem_inc_so.o:	sem_inc.c sem_inc.h
	$(CC) $(CFLAGS) $(DUMA_SO_OPTIONS) -c sem_inc.c -o $@

print_so.o:	print.c print.h
	$(CC) $(CFLAGS) $(DUMA_SO_OPTIONS) -c print.c -o $@

# DUMA_SO_OPTIONS needed cause duma.h is included explicitly
tstheap_so.o:
	$(CC) $(CFLAGS) $(DUMA_SO_OPTIONS) -c tstheap.c -o $@

dumatestpp_so.o:
	$(CXX) $(CPPFLAGS) $(DUMA_SO_OPTIONS) -c dumatestpp.cpp -o $@

#
# define rules how to build objects for static library
#

dumapp.o:	dumapp.cpp duma.h dumapp.h
	$(CXX) $(CPPFLAGS) -c dumapp.cpp -o $@

duma.o:	duma.c duma.h duma_config.h
	$(CC) $(CFLAGS) -c duma.c -o $@

sem_inc.o:	sem_inc.c sem_inc.h
	$(CC) $(CFLAGS) -c sem_inc.c -o $@

print.o:	print.c print.h
	$(CC) $(CFLAGS) -c print.c -o $@


#
# define rules how to build the test objects
#

dumatest.o:	dumatest.c duma.h duma_config.h
	$(CC) $(CFLAGS) -c dumatest.c -o $@

dumatestpp.o:	dumatestpp.cpp duma.h dumapp.h duma_config.h
	$(CXX) $(CPPFLAGS) -c dumatestpp.cpp -o $@

tstheap.o:	tstheap.c duma.h duma_config.h
	$(CC) $(CFLAGS) -c tstheap.c -o $@

testoperators.o:	testoperators.cpp duma.h dumapp.h duma_config.h
	$(CXX) $(CPPFLAGS) -c testoperators.cpp -o $@

thread-test.o:	thread-test.c duma.h duma_config.h
	$(CC) $(CFLAGS) -c thread-test.c -o $@

testmt.o:	testmt.c duma.h duma_config.h
	$(CC) $(CFLAGS) -c testmt.c -o $@


#
# default rules
#
#.c.o:
#	$(CC) $(CFLAGS) -c $< -o $@
#
#.cpp.o:
#	$(CXX) $(CPPFLAGS) -c $< -o $@
#

.PHONY: test install clean distclean dos2unix

