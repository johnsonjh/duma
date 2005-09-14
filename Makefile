#
# add "-DDUMA_NO_GLOBAL_MALLOC_FREE" (without quotes)
#   for not defining malloc/free in global namespace
#
# add "-DDUMA_EXPLICIT_INIT" (without quotes)
#    to do all the "risky" stuff getenv(), write(), .. explicitly from main().
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
# add "-DDUMA_OLD_NEW_MACRO" (without quotes)
#    if you want to use DUMA's old style NEW_ELEM() / NEW_ARRAY() macros.
#    when not defining this option, a standard conform new syntax can get used.
#    unfortunately you have to use DEL_ELEM/DEL_ARRAY further to utilize
#    filename and linenumber of deallocation calls
#
# add "-DDUMA_NO_HANG_MSG" (without quotes)
#    set this if you want to suppress the extra messages around atexit().
#

DUMA_OPTIONS = -DDUMA_EXPLICIT_INIT

# for usage with LD_PRELOAD you should not use the options
#   DUMA_NO_GLOBAL_MALLOC_FREE, DUMA_EXPLICIT_INIT and DUMA_NO_THREAD_SAFETY
#
DUMA_SO_OPTIONS = -DDUMA_NO_CPP_SUPPORT

# for FreeBSD 5.4
# DUMA_OPTIONS += -DPAGE_PROTECTION_VIOLATED_SIGNAL=SIGBUS
#
# for FreeBSD 5.4 if DUMA_EXPLICIT_INIT is not set
# DUMA_OPTIONS += -DDUMA_NO_LEAKDETECTION

PIC= -fPIC

CC=gcc
CXX=g++
AR=ar
INSTALL=install

prefix=/usr
BIN_INSTALL_DIR= $(prefix)/bin
LIB_INSTALL_DIR= $(prefix)/lib
MAN_INSTALL_DIR= $(prefix)/man/man3


PACKAGE_SOURCE= README CHANGES duma.3 Makefile \
	duma.h dumaint.h dumapp.h sem_inc.h paging.h print.h duma_hlp.h noduma.h \
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

install: libduma.a duma.3 libduma.so.0.0
	$(INSTALL) -m 755 duma.sh $(BIN_INSTALL_DIR)/duma
	$(INSTALL) -m 644 libduma.a $(LIB_INSTALL_DIR)
	$(INSTALL) -m 755 libduma.so.0.0 $(LIB_INSTALL_DIR)
	- rm -f $(LIB_INSTALL_DIR)/libduma.so.0
	ln -s libduma.so.0.0 $(LIB_INSTALL_DIR)/libduma.so.0
	- rm -f $(LIB_INSTALL_DIR)/libduma.so
	ln -s libduma.so.0.0 $(LIB_INSTALL_DIR)/libduma.so
	$(INSTALL) -m 644 duma.3 $(MAN_INSTALL_DIR)/duma.3

clean:
	- rm -f $(OBJECTS) $(SO_OBJECTS) tstheap.o dumatest.o dumatestpp.o createconf.o \
		tstheap dumatest dumatestpp createconf \
	 libduma.a libduma.so.0.0 libduma.cat DUMA.shar \
	 duma_config.h

# define special objects for build of shared library

dumapp_so.o:	dumapp.cpp duma.h dumapp.h
	$(CXX) -g $(CPPFLAGS) $(DUMA_SO_OPTIONS) -c dumapp.cpp -o $@

duma_so.o:	duma.c duma.h duma_config.h
	$(CC) -g $(CFLAGS) $(DUMA_SO_OPTIONS) -c duma.c -o $@

sem_inc_so.o:	sem_inc.c sem_inc.h
	$(CC) -g $(CFLAGS) $(DUMA_SO_OPTIONS) -c sem_inc.c -o $@

print_so.o:	print.c print.h
	$(CC) -g $(CFLAGS) $(DUMA_SO_OPTIONS) -c print.c -o $@

ifeq ($(OS), Windows_NT)
  ifeq ($(OSTYPE), msys)
    CURPATH=./
    CFLAGS= -g $(DUMA_OPTIONS)
    CPPFLAGS= -g $(DUMA_OPTIONS)
    LIBS=
    DUMASO=
  else
    CURPATH=
    CFLAGS= -g  $(DUMA_OPTIONS)
    CPPFLAGS= -g $(DUMA_OPTIONS)
    LIBS=
    DUMASO=
  endif
else
  CURPATH=./
  CFLAGS= -g $(PIC) $(DUMA_OPTIONS)
  CPPFLAGS= -g $(PIC) $(DUMA_OPTIONS)
  LIBS=-lpthread
  DUMASO=libduma.so.0.0
endif

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
	$(CC) $(CFLAGS) createconf.o -o createconf

tstheap: libduma.a tstheap.o
	- rm -f tstheap
	$(CC) $(CFLAGS) tstheap.o libduma.a -o tstheap $(LIBS)

dumatest: libduma.a dumatest.o
	- rm -f dumatest
	$(CC) $(CFLAGS) dumatest.o libduma.a -o dumatest $(LIBS)

dumatestpp: libduma.a dumatestpp.o dumapp.h
	- rm -f dumatestpp
	$(CXX) $(CPPFLAGS) dumatestpp.o libduma.a -o dumatestpp $(LIBS)

$(OBJECTS) tstheap.o dumatest.o dumatestpp.o: duma.h

ifneq ($(OS), Windows_NT)

libduma.so.0.0: duma_config.h $(SO_OBJECTS)
	$(CXX) -g -shared -Wl,-soname,libduma.so.0 -o libduma.so.0.0 \
	$(SO_OBJECTS) -lpthread -lc

endif



.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CPPFLAGS) -c $< -o $@
