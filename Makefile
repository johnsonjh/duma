PIC= -fPIC
# add "-DEF_NO_GLOBAL_MALLOC_FREE" (without quotes) to for not defining malloc/free in global namespace
# add "-DEF_NO_CPP_SUPPORT" (without quotes) to for not directing new/delete to malloc/free
# add "-DEF_NO_LEAKDETECTION" (without quotes) if you don't want support for leak detection
# add "-DEF_USE_FRAMENO" (without quotes) if you wan't to use EF_newFrame() and EF_delFrame()
# add "-DEF_OLD_NEW_MACRO" (without quotes) if you wan't to use Electric Fence' old
#                          style NEW_ELEM() / NEW_ARRAY() macros. when not defining
#                          this option, a standard conform new syntax can get used.
#                          unfortunately you have to use DEL_ELEM/DEL_ARRAY further
#                          to utilize filename and linenumber of deallocation calls

EF_OPTIONS =

ifeq ($(OS), Windows_NT)
  ifeq ($(OSTYPE), msys)
    CURPATH=./
    CFLAGS= -g $(EF_OPTIONS)
    CPPFLAGS= -g $(EF_OPTIONS)
    LIBS=
    EFENCESO=
  else
    CURPATH=
    CFLAGS= -g  $(EF_OPTIONS)
    CPPFLAGS= -g $(EF_OPTIONS)
    LIBS=
    EFENCESO=
  endif
else
  CURPATH=./
  CFLAGS= -g $(PIC) $(EF_OPTIONS)
  CPPFLAGS= -g $(PIC) $(EF_OPTIONS)
  LIBS=-lpthread
  EFENCESO=libefence.so.0.0
endif


CC=gcc
CXX=g++
AR=ar
INSTALL=install

prefix=/usr
BIN_INSTALL_DIR= $(prefix)/bin
LIB_INSTALL_DIR= $(prefix)/lib
MAN_INSTALL_DIR= $(prefix)/man/man3


PACKAGE_SOURCE= README CHANGES efence.3 Makefile \
	efence.h efenceint.h efencpp.h paging.h print.h ef_hlp.h \
	efence.c efencepp.cpp \
	eftest.c tstheap.c eftestpp.cpp \
	createconf.c

OBJECTS = efencepp.o efence.o

all:	libefence.a $(EFENCESO) tstheap eftest eftestpp
	@ echo "Testing Electric Fence."
	@ echo "After the last test, it should print that the test has PASSED."
	$(CURPATH)eftest
	$(CURPATH)tstheap 3072
	@ echo "Electric Fence confidence test PASSED."

install: libefence.a efence.3 libefence.so.0.0
	$(INSTALL) -m 755 ef.sh $(BIN_INSTALL_DIR)/ef
	$(INSTALL) -m 644 libefence.a $(LIB_INSTALL_DIR)
	$(INSTALL) -m 755 libefence.so.0.0 $(LIB_INSTALL_DIR)
	- rm -f $(LIB_INSTALL_DIR)/libefence.so.0
	ln -s libefence.so.0.0 $(LIB_INSTALL_DIR)/libefence.so.0
	- rm -f $(LIB_INSTALL_DIR)/libefence.so
	ln -s libefence.so.0.0 $(LIB_INSTALL_DIR)/libefence.so
	$(INSTALL) -m 644 efence.3 $(MAN_INSTALL_DIR)/efence.3

clean:
	- rm -f $(OBJECTS) tstheap.o eftest.o eftestpp.o createconf.o tstheap \
		eftest eftestpp createconf \
	 libefence.a libefence.so.0.0 libefence.cat ElectricFence.shar \
	 efence_config.h

roff:
	nroff -man < efence.3 > efence.cat


ElectricFence.shar: $(PACKAGE_SOURCE)
	shar $(PACKAGE_SOURCE) > ElectricFence.shar

shar: ElectricFence.shar

libefence.a: efence_config.h $(OBJECTS)
	- rm -f libefence.a
	$(AR) crv libefence.a $(OBJECTS)


ifneq ($(OS), Windows_NT)
libefence.so.0.0: efence_config.h $(OBJECTS)
	$(CXX) -g -shared -Wl,-soname,libefence.so.0 -o libefence.so.0.0 \
	$(OBJECTS) -lpthread -lc
endif

efence_config.h: createconf
	- $(CURPATH)createconf >efence_config.h

createconf: createconf.o
	- rm -f createconf
	$(CC) $(CFLAGS) createconf.o -o createconf

tstheap: libefence.a tstheap.o
	- rm -f tstheap
	$(CC) $(CFLAGS) tstheap.o libefence.a -o tstheap $(LIBS)

eftest: libefence.a eftest.o
	- rm -f eftest
	$(CC) $(CFLAGS) eftest.o libefence.a -o eftest $(LIBS)

eftestpp: libefence.a eftestpp.o efencepp.h
	- rm -f eftestpp
	$(CXX) $(CPPFLAGS) eftestpp.o libefence.a -o eftestpp $(LIBS)

$(OBJECTS) tstheap.o eftest.o eftestpp.o: efence.h

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CPPFLAGS) -c $< -o $@
