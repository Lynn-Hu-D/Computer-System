CC = gcc
CFLAGS = -Wall -Wextra -g
TARGETS = loadmem allocmem

all: $(TARGETS)

loadmem: loadmem.o
	$(CC) $(CFLAGS) -o loadmem loadmem.o

allocmem: allocmem.o
	$(CC) $(CFLAGS) -o allocmem allocmem.o

loadmem.o: loadmem.c
	$(CC) $(CFLAGS) -c loadmem.c

allocmem.o: allocmem.c
	$(CC) $(CFLAGS) -c allocmem.c

clean:
	rm -f *.o $(TARGETS)