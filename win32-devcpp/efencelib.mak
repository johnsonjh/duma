# Project: EFence
# Makefile created by Dev-C++ 4.9.6.0

CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = ../print.o ../efencepp.o ../page.o ../efence.o $(RES)
LIBS =  -L"C:/Programme/Dev-Cpp/lib" 
INCS =  -I"C:/Programme/Dev-Cpp/include" 
BIN  = efencelib.a
CFLAGS = $(INCS)  -s

.PHONY: all all-before all-after clean clean-custom

all: all-before efencelib.a all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	ar r $(BIN) $(OBJ)
	ranlib $(BIN)

../print.o: ../print.c
	$(CC) -c ../print.c -o ../print.o $(CFLAGS)

../efencepp.o: ../efencepp.cpp
	$(CC) -c ../efencepp.cpp -o ../efencepp.o $(CFLAGS)

../page.o: ../page.c
	$(CC) -c ../page.c -o ../page.o $(CFLAGS)

../efence.o: ../efence.c
	$(CC) -c ../efence.c -o ../efence.o $(CFLAGS)
