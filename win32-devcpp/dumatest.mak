# Project: dumatest
# Makefile created by Dev-C++ 4.9.6.0

CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = ../dumatest.o $(RES)
LIBS =  -L"C:/Programme/Dev-Cpp/lib" dumalib.a
INCS =  -I"C:/Programme/Dev-Cpp/include" 
BIN  = dumatest.exe
CFLAGS = $(INCS) -I.. -s

.PHONY: all all-before all-after clean clean-custom

all: all-before dumatest.exe all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o "dumatest.exe" $(LIBS) $(CFLAGS)

../dumatest.o: ../dumatest.c
	$(CC) -c ../dumatest.c -o ../dumatest.o $(CFLAGS)
