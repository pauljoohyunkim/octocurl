CC=gcc
CFLAGS=-g -Wall
LDFLAGS=-lpthread -lcurl
SRCDIR=src
OBJDIR=obj
BINDIR=bin
OBJS = $(OBJDIR)/octoget.o
BIN = $(BINDIR)/octoget

all: $(BIN)

release: CFLAGS=-Wall
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/*

debug: $(BIN)
	gdb $(BIN)
