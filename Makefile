PIC= -fPIC
# add "-DEF_NO_CPP" (without quotes) to for not directing new/delete to malloc/free
# add "-DEF_NO_LEAKDETECTION" (without quotes) if you don't want support for leak detection
CFLAGS= -g -DUSE_SEMAPHORE $(PIC)
LIBS= -lpthread

prefix=/usr
BIN_INSTALL_DIR= $(prefix)/bin
LIB_INSTALL_DIR= $(prefix)/lib
MAN_INSTALL_DIR= $(prefix)/man/man3

CC= cc
AR= ar
INSTALL= install

PACKAGE_SOURCE= README CHANGES efence.3 Makefile \
	efence.h efenceint.h efencpp.h \
	efence.c efencepp.cpp page.c print.c \
	eftest.c tstheap.c

OBJECTS= efence.o efencepp.o page.o print.o

all:	libefence.a libefence.so.0.0 tstheap eftest
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
	- rm -f $(OBJECTS) tstheap.o eftest.o tstheap eftest \
	 libefence.a libefence.so.0.0 libefence.cat ElectricFence.shar

roff:
	nroff -man < efence.3 > efence.cat


ElectricFence.shar: $(PACKAGE_SOURCE)
	shar $(PACKAGE_SOURCE) > ElectricFence.shar

shar: ElectricFence.shar

libefence.a: $(OBJECTS)
	- rm -f libefence.a
	$(AR) crv libefence.a $(OBJECTS)

libefence.so.0.0: $(OBJECTS)
	gcc -g -shared -Wl,-soname,libefence.so.0 -o libefence.so.0.0 \
		$(OBJECTS) -lpthread -lc 

tstheap: libefence.a tstheap.o
	- rm -f tstheap
	$(CC) $(CFLAGS) tstheap.o libefence.a -o tstheap $(LIBS)

eftest: libefence.a eftest.o
	- rm -f eftest
	$(CC) $(CFLAGS) eftest.o libefence.a -o eftest $(LIBS)

$(OBJECTS) tstheap.o eftest.o: efence.h

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
