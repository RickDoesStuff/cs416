C = gcc
CFLAGS1 = -g -m32
CFLAGS2 = -g -Wall -std=c99 -fsanitize=address -O2
CFLAGS3 = -g -lpthread
RM = rm -f

STACK = stack
THREADS = threads
BITOPS = bitops

$(STACK): stack.c
	$(CC) $(CFLAGS1) stack.c -o $(STACK)

$(BITOPS): bitops.c
	$(CC) $(CFLAGS) bitops.c -o $(BITOPS)

$(THREADS): threads.c
	$(CC) $(CFLAGS3) threads.c -o $(THREADS)

all: $(STACK) $(THREADS) $(BITOPS)

clean:
	$(RM) $(STACK) $(THREADS) $(BITOPS)