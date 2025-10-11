# Directories
INCLUDES 	:= ./include
TESTS 		:= ./test
SRCS 		:= ./src
BINS 		:= ./bin

CC 			:= gcc
CFLAGS 		:= -std=gnu11 -Wall -Wextra -O2 -Iinclude
LDFLAGS		:= -lm
SRCFILES	:= $(wildcard SRCS/*.c)
OBJFILES	:= $(SRCFILES:.c=.o)
TARGET		:= bthread_demo


all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET)
