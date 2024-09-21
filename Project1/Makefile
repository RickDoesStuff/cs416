C = gcc
CFLAGS1 = -g -m32
CFLAGS2 = -g -Wall -std=c99 -fsanitize=address -O2
RM = rm -f

STACK = stack
THREADS = threads
BITOPS = bitops

$(STACK): stack.c
        $(CC) $(CFLAGS1) stack.c -o $(STACK)

$(THREADS): threads.c
        $(CC) $(CFLAGS2) threads.c -o $(THREADS)

$(BITOPS): bitops.c
        $(CC) $(CFLAGS1) bitops.c -o $(BITOPS)

all: $(STACK) $(THREADS) $(BITOPS)