
CC=gcc
CFLAGS=-Wall

LINK=-lm

OBJFILE=lecture_jp.c
EXEFILE=calc

all: $(OBJFILE)
	$(CC) $(CFLAGS) $(OBJFILE) -o $(EXEFILE) $(LINK)

clean:
	rm -f *.o

