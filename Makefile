
CC=gcc
CFLAGS=-O2 -fomit-frame-pointer -W -Wall

PROGS=\
	binheap\
	binoheap\
	binoheap2

all: $(PROGS)

test: all
	./binheap
	./binoheap
	./binoheap2

binheap: testmain.o binheap.o
	$(CC) -o $@ testmain.o binheap.o

binoheap: binoheap.o
	$(CC) -o $@ binoheap.o

binoheap2: binoheap2.o
	$(CC) -o $@ binoheap2.o

clean:
	rm -f *.o $(PROGS)
