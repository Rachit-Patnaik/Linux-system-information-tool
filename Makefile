CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -Isrc
TARGET = sysinfo_tool

SRCS = src/main.c src/system_info.c src/cpu_info.c src/mem_info.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean