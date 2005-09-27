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
# add "-DDUMA_NO_CPP_SUPPORT" (without quotes)
#    for not directing new/delete to malloc/free
#
# add "-DDUMA_NO_LEAKDETECTION" (without quotes)
#    if you don't want support for leak detection
#
# add "-DDUMA_USE_FRAMENO" (without quotes)
#    if you want to use DUMA_newFrame() and DUMA_delFrame()
#
# add "-DDUMA_PREFER_ATEXIT" (without quotes)
#    if you prefer atexit() over GNU Compiler's function attribute "destructor"
#
# add "-DDUMA_OLD_NEW_MACRO" (without quotes)
#    if you want to use DUMA's old style NEW_ELEM() / NEW_ARRAY() macros.
#    when not defining this option, a standard conform new syntax can get used.
#    unfortunately you have to use DEL_ELEM/DEL_ARRAY further to utilize
#    filename and linenumber of deallocation calls
#
# add "-DDUMA_NO_HANG_MSG" (without quotes)
#    set this if you want to suppress the extra messages around atexit().
#
#
########################################
#
#  preprocessor flags for building the shared library (DUMA_SO_LIBRARY):
#    - DUMA_SO_NO_CPP_SUPPORT
#    - DUMA_SO_NO_LEAKDETECTION
#    - DUMA_SO_PREFER_ATEXIT
#    - DUMA_SO_NO_HANG_MSG
#
#  preprocessor flags for building the static library:
#    - DUMA_LIB_NO_CPP_SUPPORT
#    - DUMA_LIB_NO_LEAKDETECTION
#    - DUMA_LIB_PREFER_ATEXIT
#    - DUMA_LIB_NO_HANG_MSG
#    - DUMA_NO_GLOBAL_MALLOC_FREE
#    - DUMA_EXPLICIT_INIT
#    - DUMA_NO_THREAD_SAFETY
#    - DUMA_USE_FRAMENO
#    - DUMA_OLD_NEW_MACRO
#    - DUMA_OLD_DEL_MACRO
#
########################################

# edit following line
DUMA_OPTIONS =


PIC= -fPIC
DUMA_SO_OPTIONS = $(PIC) -DDUMA_SO_LIBRARY

# for FreeBSD 5.4
# DUMA_OPTIONS += -DPAGE_PROTECTION_VIOLATED_SIGNAL=SIGBUS
#
# for FreeBSD 5.4 if DUMA_EXPLICIT_INIT is not set
# DUMA_OPTIONS += -DDUMA_NO_LEAKDETECTION


ifeq ($(OS), Windows_NT)
  ifeq ($(OSTYPE), msys)
    CURPATH=./
    DUMASO=
    CFLAGS= -g -O0
    CPPFLAGS= -g -O0
    LIBS=
  else
    CURPATH=
    DUMASO=
    CFLAGS= -g -O0
    CPPFLAGS= -g -O0
    LIBS=
  endif
else
  CURPATH=./
  DUMASO=libduma.so.0.0
  DUMASO_LINK1=libduma.so.0
  DUMASO_LINK2=libduma.so
  CFLAGS= -g -O0
  CPPFLAGS= -g -O0
  LIBS=-lpthread
endif


CC=gcc
CXX=g++
AR=ar
INSTALL=install

############################################################

prefix=/usr
BIN_INSTALL_DIR= $(prefix)/bin
LIB_INSTALL_DIR= $(prefix)/lib
MAN_INSTALL_DIR= $(prefix)/man/man3



PACKAGE_SOURCE= README CHANGES duma.3 Makefile \
	duma.h dumapp.h sem_inc.h paging.h print.h duma_hlp.h noduma.h \
	duma.c dumapp.cpp sem_inc.c print.c \
	dumatest.c tstheap.c dumatestpp.cpp \
	createconf.c

OBJECTS = dumapp.o duma.o sem_inc.o print.o

SO_OBJECTS = dumapp_so.o duma_so.o sem_inc_so.o print_so.o

all:	libduma.a $(DUMASO) tstheap dumatest dumatestpp
	@ echo "Testing DUMA."
	@ echo "After the last test, it should print that the test has PASSED."
	$(CURPATH)dumatest
	$(CURPATH)tstheap 3072
	@ echo "DUMA confidence test PASSED."

install: libduma.a duma.3 $(DUMASO)
	$(INSTALL) -m 755 duma.sh $(BIN_INSTALL_DIR)/duma
	$(INSTALL) -m 644 libduma.a $(LIB_INSTALL_DIR)
	$(INSTALL) -m 755 $(DUMASO) $(LIB_INSTALL_DIR)
	- rm -f $(LIB_INSTALL_DIR)/$(DUMASO_LINK1)
	ln -s $(DUMASO) $(LIB_INSTALL_DIR)/$(DUMASO_LINK1)
	- rm -f $(LIB_INSTALL_DIR)/$(DUMASO_LINK2)
	ln -s $(DUMASO) $(LIB_INSTALL_DIR)/$(DUMASO_LINK2)
	$(INSTALL) -m 644 duma.3 $(MAN_INSTALL_DIR)/duma.3

clean:
	- rm -f $(OBJECTS) $(SO_OBJECTS) tstheap.o dumatest.o dumatestpp.o createconf.o \
		tstheap dumatest dumatestpp createconf \
	 libduma.a $(DUMASO) libduma.cat DUMA.shar \
	 duma_config.h

roff:
	nroff -man < duma.3 > duma.cat


DUMA.shar: $(PACKAGE_SOURCE)
	shar $(PACKAGE_SOURCE) > DUMA.shar

shar: DUMA.shar

libduma.a: duma_config.h $(OBJECTS)
	- rm -f libduma.a
	$(AR) crv libduma.a $(OBJECTS)


duma_config.h: createconf
	- $(CURPATH)createconf >duma_config.h

createconf: createconf.o
	- rm -f createconf
	$(CC) $(CFLAGS) $(DUMA_OPTIONS) createconf.o -o createconf

tstheap: libduma.a tstheap.o
	- rm -f tstheap
	$(CC) $(CFLAGS) tstheap.o libduma.a -o tstheap $(LIBS)

dumatest: libduma.a dumatest.o
	- rm -f dumatest
	$(CC) $(CFLAGS) dumatest.o libduma.a -o dumatest $(LIBS)

dumatestpp: libduma.a dumatestpp.o dumapp.h
	- rm -f dumatestpp
	$(CXX) $(CPPFLAGS) dumatestpp.o libduma.a -o dumatestpp $(LIBS)

testoperators: libduma.a testoperators.o dumapp.h
	- rm -f testoperators
	$(CXX) $(CPPFLAGS) testoperators.o libduma.a -o testoperators $(LIBS)

$(OBJECTS) tstheap.o dumatest.o dumatestpp.o: duma.h

ifneq ($(OS), Windows_NT)

$(DUMASO): duma_config.h $(SO_OBJECTS)
	$(CXX) -g -shared -Wl,-soname,$(DUMASO) -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc
	$(CXX) -g -shared -o $(DUMASO) $(SO_OBJECTS) -lpthread -lc

endif


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


#
# default rules
#
#.c.o:
#	$(CC) $(CFLAGS) -c $< -o $@
#
#.cpp.o:
#	$(CXX) $(CPPFLAGS) -c $< -o $@
#


