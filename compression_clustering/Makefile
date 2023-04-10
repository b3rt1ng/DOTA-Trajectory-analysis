CC=g++
CFLAGS=-fdiagnostics-color=always -g -Wall -std=c++17 -fPIC -fopenmp
LFLAGS=-lboost_system -lboost_filesystem -fopenmp
SRC=src
OBJ=obj

SRCS=$(wildcard $(SRC)/*.cpp)
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))
BINDIR=bin
BIN=$(BINDIR)/main

all: $(BIN)


release: CFLAGS=-Wall -O3
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJ)/*	
