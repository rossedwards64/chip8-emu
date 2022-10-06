CC 	    := gcc
BIN     := chip8

INCLUDE := -Isdl -Isdl/include
LINK    := -Lsdl/lib
LIBS    := -lSDL2
CFLAGS  := -Wall -Wpedantic -Wextra -g3 -pedantic

SRCDIR  := src
SRCS    := $(wildcard $(SRCDIR)/*.c)

OBJDIR  := build
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(LINK) $(LIBS) $(INCLUDE) -o $@ $^ $(CFLAGS)

.phony: clean

clean:
	rm -rf $(OBJDIR) chip8
