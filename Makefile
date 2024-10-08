CC      := gcc
BIN     ?= chip8

LDLIBS  := -lSDL2
CFLAGS  := -std=c17 -Wall -Wpedantic -Wextra -g3 -pedantic

SRCDIR  ?= src
SRCS    := $(wildcard $(SRCDIR)/*.c)

OBJDIR  ?= build
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(LDLIBS) $(INCLUDE)

.phony: clean

clean:
	rm -rf $(OBJDIR) chip8
