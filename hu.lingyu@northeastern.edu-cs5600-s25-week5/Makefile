# Makefile to build the queuetest executable

CC = gcc
CFLAGS = -Wall
OBJ = queue.o queuetest.o

all: queuetest

queuetest: $(OBJ)
	$(CC) $(CFLAGS) -o queuetest $(OBJ)

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

queuetest.o: queuetest.c queue.h
	$(CC) $(CFLAGS) -c queuetest.c

clean:
	rm -f *.o queuetest