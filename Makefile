
CC=gcc
CFLAGS=-Wall

EXEFILE=calc

default: calc.c stack.c formula.c
	$(CC) $(CFLAGS) calc.c stack.c formula.c -o $(EXEFILE)

clean:
	rm -f *.o

