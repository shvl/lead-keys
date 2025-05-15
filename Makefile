CC = cc
CFLAGS = -Wall -Wextra
LDFLAGS = -framework ApplicationServices

SRCS = seq-lead-keys.c command_tree.c parse_args.c
OBJS = $(SRCS:%.c=bin/%.o)
TARGET = bin/seq-lead-keys

all: bin $(TARGET)

bin:
	mkdir -p bin

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

bin/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean bin 