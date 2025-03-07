# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Targets
all: multithread pqueuepmain

# multithread executable
multithread: multithread.c
	$(CC) $(CFLAGS) -o multithread multithread.c

# pqueuepmain executable
pqueuepmain: pqueuepmain.c
	$(CC) $(CFLAGS) -o pqueuepmain pqueuepmain.c

# Clean up
clean:
	rm -f multithread pqueuepmain