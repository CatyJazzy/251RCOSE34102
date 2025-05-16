CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

SRCS = main.c process.c scheduler.c utils.c
OBJS = $(SRCS:.c=.o)
TARGET = cpu_scheduler

TEST_SRCS = test_scheduler.c process.c scheduler.c utils.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_TARGET = test_scheduler

.PHONY: all clean test

all: $(TARGET)

test: $(TEST_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET) 