
DUMADIR=../..
CPPFLAGS=-g -O0 -I$(DUMADIR) -include stdlib.h -include new -include dumapp.h
LFLAGS=$(DUMADIR)/libduma.a
ifeq ($(OS), Windows_NT)
  LIBS=
else
  LIBS=-lpthread
endif

CC=gcc
CXX=g++

BIN=example6
OBJ=../../$(BIN).o

all: $(BIN)
	./$(BIN)

clean:
	- rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) $(LFLAGS) $(LIBS) -o $(BIN)

.cpp.o:
	$(CXX) $(CPPFLAGS) -c $< -o $@

