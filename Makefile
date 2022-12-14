CC=gcc
CFLAGS=-g -Wall
LDFLAGS=-lpthread -lcurl -lncurses -lm
SRCDIR=src
OBJDIR=obj
BINDIR=bin
TESTDIR=test
OBJS = $(OBJDIR)/octocurl.o $(OBJDIR)/conn.o $(OBJDIR)/queue.o $(OBJDIR)/filestring.o
BIN = $(BINDIR)/octocurl
TESTBIN = $(TESTDIR)/structtest

all: $(BIN)

release: CFLAGS=-Wall
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test from http://xcal1.vodafone.co.uk/
test: $(BIN)
	$(BIN) http://212.183.159.230/5MB.zip http://212.183.159.230/10MB.zip http://212.183.159.230/20MB.zip http://212.183.159.230/50MB.zip

test1: $(BIN)
	$(BIN) -c 1 http://212.183.159.230/5MB.zip http://212.183.159.230/10MB.zip http://212.183.159.230/20MB.zip http://212.183.159.230/50MB.zip

test2: $(BIN)
	$(BIN) -c 2 http://212.183.159.230/5MB.zip http://212.183.159.230/10MB.zip http://212.183.159.230/20MB.zip http://212.183.159.230/50MB.zip

test3: $(BIN)
	$(BIN) -c 3 http://212.183.159.230/5MB.zip http://212.183.159.230/10MB.zip http://212.183.159.230/20MB.zip http://212.183.159.230/50MB.zip

test4: $(BIN)
	$(BIN) -c 4 http://212.183.159.230/5MB.zip http://212.183.159.230/10MB.zip http://212.183.159.230/20MB.zip http://212.183.159.230/50MB.zip

test5: $(BIN)
	$(BIN) -c 5 http://212.183.159.230/5MB.zip http://212.183.159.230/10MB.zip http://212.183.159.230/20MB.zip http://212.183.159.230/50MB.zip
	
testclean:
	$(RM) *.zip*

$(TESTDIR)/structtest: $(TESTDIR)/structtest.c $(SRCDIR)/queue.c
	$(CC) $(CFLAGS) $(TESTDIR)/structtest.c $(SRCDIR)/queue.c -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/* $(TEST)/*.o $(TESTBIN) ./*.zip ./*.tar ./*.com

debug: $(BIN)
	gdb $(BIN)

tar:
	tar -cf octocurl.tar *
