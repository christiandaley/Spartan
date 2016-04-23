#Makefile for Spartan by Christian Daley
CC = gcc
ifeq ($(BUILD),win)
CFLAGS = -O4 -DNDEBUG -std=c99 -static
else
CFLAGS = -O4 -DNDEBUG -std=c99
endif

all: Spartan

Spartan: bitmasks.o bitscan.o board.o debug.o eval.o history.o killer.o log.o magicmoves.o magicnumbers.o magicnumbersgen.o main.o move.o movegen.o search.o sort.o test.o ttable.o uci.o
	$(CC) $(CFLAGS) -o Spartan bitmasks.o bitscan.o board.o debug.o eval.o history.o killer.o log.o magicmoves.o magicnumbers.o magicnumbersgen.o main.o move.o movegen.o search.o sort.o test.o ttable.o uci.o -lpthread

clean:
	$(RM) Spartan *.o *~
win:
	make "BUILD=win"