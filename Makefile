CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

SRCS = main.c process.c scheduler.c utils.c
OBJS = $(SRCS:.c=.o)
TARGET = cpu_scheduler

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) 