# Project: DUMA
# Makefile created by Dev-C++ 4.9.6.0

CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = ../print.o ../dumapp.o ../page.o ../duma.o $(RES)
LIBS =  -L"C:/Programme/Dev-Cpp/lib" 
INCS =  -I"C:/Programme/Dev-Cpp/include" 
BIN  = dumalib.a
CFLAGS = $(INCS)  -s

.PHONY: all all-before all-after clean clean-custom

all: all-before dumalib.a all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	ar r $(BIN) $(OBJ)
	ranlib $(BIN)

../print.o: ../print.c
	$(CC) -c ../print.c -o ../print.o $(CFLAGS)

../dumapp.o: ../dumapp.cpp
	$(CC) -c ../dumapp.cpp -o ../dumapp.o $(CFLAGS)

../page.o: ../page.c
	$(CC) -c ../page.c -o ../page.o $(CFLAGS)

../duma.o: ../duma.c
	$(CC) -c ../duma.c -o ../duma.o $(CFLAGS)
