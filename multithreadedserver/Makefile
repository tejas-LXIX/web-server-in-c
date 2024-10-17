CC=gcc
CFLAGS=-g -pthread
BINS=server

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf *.dSYM $(BINS)