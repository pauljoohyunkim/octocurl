CC=gcc
CFLAGS=-g -Wall
LDFLAGS=-lpthread -lcurl
SRCDIR=src
OBJDIR=obj
BINDIR=bin
TESTDIR=test
OBJS = $(OBJDIR)/octoget.o $(OBJDIR)/conn.o
BIN = $(BINDIR)/octoget
TESTBIN = $(TEST)/conntest

all: $(BIN)

release: CFLAGS=-Wall
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTBIN): $(TESTDIR)/conntest.o $(OBJDIR)/conn.o
	$(CC) $(CFLAGS) $^ -o $@ -lcurl

$(TESTDIR)/conntest.o: $(TESTDIR)/conntest.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/* $(TEST)/*.o $(TESTBIN)

debug: $(BIN)
	gdb $(BIN)
