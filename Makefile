
CC=gcc
CFLAGS=-Wall

LINK=-lm

OBJFILE=main.o calc.o stack.o formula.o
EXEFILE=calc

all: $(OBJFILE)
	$(CC) $(CFLAGS) $(OBJFILE) -o $(EXEFILE) $(LINK)

clean:
	rm -f *.o

