CC 	   := gcc
BIN    := chip8
LIB    :=
CFLAGS := -Wall -Wpedantic -Wextra -g3 -pedantic

SRCDIR := src
SRCS   := $(wildcard $(SRCDIR)/*.c)

OBJDIR := build
OBJS   := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LIB) $(CFLAGS)

.phony: clean

clean:
	rm -rf $(OBJDIR) chip8
