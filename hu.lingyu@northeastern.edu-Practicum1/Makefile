CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Source files
SRCS = message.c cache.c test.c 
OBJS = $(SRCS:.c=.o)
TARGET = test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c message.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c cache.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)