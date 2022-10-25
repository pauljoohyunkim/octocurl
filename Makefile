CC=gcc
CFLAGS=-g -Wall
LDFLAGS=-lpthread -lcurl
SRCDIR=src
OBJDIR=obj
BINDIR=bin
TESTDIR=test
OBJS = $(OBJDIR)/octoget.o $(OBJDIR)/conn.o
BIN = $(BINDIR)/octoget

all: $(BIN)

release: CFLAGS=-Wall
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDFLAGS)

CURLOPT_VERBOSECURLOPT_VERBOSE$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTDIR)/conntest: $(TESTDIR)/conntest.o $(OBJDIR)/conn.o
	$(CC) $(CFLAGS) $^ -o $@ -lcurl

$(TESTDIR)/conntest.o: $(TESTDIR)/conntest.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/*

debug: $(BIN)
	gdb $(BIN)
