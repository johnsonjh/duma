
DUMADIR=../..
CPPFLAGS=-g -O0 -I$(DUMADIR)
LFLAGS=$(DUMADIR)/libduma.a
LIBS=-lpthread

CC=gcc
CXX=g++

BIN=example4
OBJ=../../$(BIN).o

all: $(BIN)
	./$(BIN)

clean:
	- rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) $(LFLAGS) $(LIBS) -o $(BIN)

