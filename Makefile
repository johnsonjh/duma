PIC= -fPIC
# add "-DEF_NO_CPP_SUPPORT" (without quotes) to for not directing new/delete to malloc/free
# add "-DEF_NO_LEAKDETECTION" (without quotes) if you don't want support for leak detection

ifeq ($(OS), Windows_NT)
  ifeq ($(OSTYPE), msys)
    CFLAGS= -g
    CPPFLAGS= -g
    LIBS=
    EFENCESO=
  endif
else
  CFLAGS= -g $(PIC)
  CPPFLAGS= -g $(PIC)
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
	@ echo
	@ echo "Testing Electric Fence."
	@ echo "After the last test, it should print that the test has PASSED."
	./eftest
	./tstheap 3072
	@ echo
	@ echo "Electric Fence confidence test PASSED."
	@ echo

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
	- ./createconf >efence_config.h

createconf: createconf.o
	- rm -f createconf
	$(CC) $(CFLAGS) createconf.o -o createconf

tstheap: libefence.a tstheap.o
	- rm -f tstheap
	$(CC) $(CFLAGS) tstheap.o libefence.a -o tstheap $(LIBS)

eftest: libefence.a eftest.o
	- rm -f eftest
	$(CC) $(CFLAGS) eftest.o libefence.a -o eftest $(LIBS)

eftestpp: libefence.a eftestpp.o
	- rm -f eftestpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) eftestpp.o libefence.a -o eftestpp $(LIBS)

$(OBJECTS) tstheap.o eftest.o eftestpp.o: efence.h

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CPPFLAGS) -c $< -o $@
