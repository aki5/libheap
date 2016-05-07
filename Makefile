
CC=gcc
CFLAGS=-O2 -fomit-frame-pointer

PROGS=\
	binoheap\
	binoheap2

all: $(PROGS)

test: all
	./binoheap
	./binoheap2

binoheap: binoheap.o
	$(CC) -o $@ binoheap.o

binoheap2: binoheap2.o
	$(CC) -o $@ binoheap2.o

clean:
	rm -f *.o $(PROGS)
