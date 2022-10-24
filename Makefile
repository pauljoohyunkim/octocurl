CC=gcc
CFLAGS=-g -Wall
SRCDIR=src
OBJDIR=obj
BINDIR=bin
OBJS = $(OBJDIR)/*.o
BIN = $(BINDIR)/octoget

all: $(BIN)

release: CFLAGS=-Wall
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/*
