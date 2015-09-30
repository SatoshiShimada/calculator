
CC=gcc
CFLAGS=-Wall

LINK=-lm

SRCFILE=main.c calc.c stack.c formula.c
EXEFILE=calc

default: calc.c stack.c formula.c
	$(CC) $(CFLAGS) $(LINK) $(SRCFILE) -o $(EXEFILE)

clean:
	rm -f *.o

