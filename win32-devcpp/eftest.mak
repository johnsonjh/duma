# Project: eftest
# Makefile created by Dev-C++ 4.9.6.0

CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = ../eftest.o $(RES)
LIBS =  -L"C:/Programme/Dev-Cpp/lib" efencelib.a
INCS =  -I"C:/Programme/Dev-Cpp/include" 
BIN  = eftest.exe
CFLAGS = $(INCS) -I.. -s

.PHONY: all all-before all-after clean clean-custom

all: all-before eftest.exe all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o "eftest.exe" $(LIBS) $(CFLAGS)

../eftest.o: ../eftest.c
	$(CC) -c ../eftest.c -o ../eftest.o $(CFLAGS)
