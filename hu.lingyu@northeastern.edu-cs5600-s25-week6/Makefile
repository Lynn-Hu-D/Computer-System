# Makefile to build the read_and_cipher executable

CC = gcc
CFLAGS = -Wall
OBJ = queue.o read_and_cipher.o

# Targets
all: read_and_cipher cipher genrandstrings

# genrand executable
genrand: genrandstrings.c
	$(CC) $(CFLAGS) -o genrandstrings genrandstrings.c

# cipher executable
cipher: cipher.c polybius.c
	$(CC) $(CFLAGS) -o cipher cipher.c polybius.c

read_and_cipher: $(OBJ)
	$(CC) $(CFLAGS) -o read_and_cipher $(OBJ)

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

read_and_cipher.o: read_and_cipher.c queue.h
	$(CC) $(CFLAGS) -c read_and_cipher.c

clean:
	rm -f *.o read_and_cipher cipher genrandstrings *.txt