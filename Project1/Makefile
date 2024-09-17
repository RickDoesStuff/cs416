CC = gcc
CFLAGS1 = -m32
CFLAGS2 = -g -Wall -std=c99 -fsanitize=address -O2
RM = rm -f

STACK = stack
THREADS = threads
BITOPS = bitops

OBJ = stack.o
OBJ3 = threads.o
OBJ5 = bitops.o

$(STACK): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(STACK)
	
$(THREADS): $(OBJ3)
	$(CC) $(CFLAGS2) $(OBJ3) -o $(THREADS)
	
$(BITOPS): $(OBJ5)
	$(CC) $(CFLAGS) $(OBJ5) -o $(BITOPS)

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

threads.o: threads.c
	$(CC) $(CFLAGS2) -c threads.c

bitops.o: bitops.c
	$(CC) $(CFLAGS) -c bitops.c

all: $(STACK) $(THREADS) $(BITOPS)

clean:
	$(RM) $(STACK) $(THREADS) $(BITOPS) $(OBJ) $(OBJ2) $(OBJ3)
