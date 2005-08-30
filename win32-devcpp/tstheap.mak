# Project: tstheap
# Makefile created by Dev-C++ 4.9.6.0

CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = ../tstheap.o $(RES)
LIBS =  -L"C:/Programme/Dev-Cpp/lib" dumalib.a
INCS =  -I"C:/Programme/Dev-Cpp/include" 
BIN  = tstheap.exe
CFLAGS = $(INCS) -I.. -s

.PHONY: all all-before all-after clean clean-custom

all: all-before tstheap.exe all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o "tstheap.exe" $(LIBS) $(CFLAGS)

../tstheap.o: ../tstheap.c
	$(CC) -c ../tstheap.c -o ../tstheap.o $(CFLAGS)
