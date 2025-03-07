# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Targets
all: genrand cipher

# genrand executable
genrand: genrand.c
	$(CC) $(CFLAGS) -o genrand genrand.c

# cipher executable
cipher: cipher.c polybius.c
	$(CC) $(CFLAGS) -o cipher cipher.c polybius.c

# Clean up
clean:
	rm -f genrand cipher