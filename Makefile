PIC= -fPIC
# add "-DEF_NO_GLOBAL_MALLOC_FREE" (without quotes) to for not defining malloc/free in global namespace
# add "-DEF_EXPLICIT_INIT" (without quotes) to do all the "risky" stuff (getenv(), write(), ..)
#                            explicitly from main(). you have to call ef_init() explicitly from main()
#                            this option also avoids leak error messages for allocations
#                            before ef_init() got called. this helps for leaky standard environments.
# add "-DEF_NO_THREAD_SAFETY" (without quotes) for not supporting multi-threading
# add "-DEF_NO_CPP_SUPPORT" (without quotes) to for not directing new/delete to malloc/free
# add "-DEF_NO_LEAKDETECTION" (without quotes) if you don't want support for leak detection
# add "-DEF_USE_FRAMENO" (without quotes) if you want to use EF_newFrame() and EF_delFrame()
# add "-DEF_OLD_NEW_MACRO" (without quotes) if you want to use Electric Fence' old
#                          style NEW_ELEM() / NEW_ARRAY() macros. when not defining
#                          this option, a standard conform new syntax can get used.
#                          unfortunately you have to use DEL_ELEM/DEL_ARRAY further
#                          to utilize filename and linenumber of deallocation calls
#
# for usage with LD_PRELOAD you should not use the options
#   EF_NO_GLOBAL_MALLOC_FREE, EF_EXPLICIT_INIT and DEF_NO_THREAD_SAFETY
#

EF_OPTIONS = -DEF_EXPLICIT_INIT

# for FreeBSD 5.4
# EF_OPTIONS += -DPAGE_PROTECTION_VIOLATED_SIGNAL=SIGBUS
#
# for FreeBSD 5.4 if EF_EXPLICIT_INIT is not set
# EF_OPTIONS += -DEF_NO_LEAKDETECTION


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
	efence.h efenceint.h efencpp.h sem_inc.h paging.h print.h ef_hlp.h noefence.h \
	efence.c efencepp.cpp sem_inc.c print.c \
	eftest.c tstheap.c eftestpp.cpp \
	createconf.c

OBJECTS = efencepp.o efence.o sem_inc.o print.o

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
