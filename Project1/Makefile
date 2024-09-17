CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address -O2 -m32
RM = rm -f

STACK = stack
DEBUGSTACK = dstack
THREADS = threads
DEBUGTHREADS = dthreads
BITOPS = bitops
DEBUGBITOPS = dbitops

OBJ = stack.o
OBJ2 = dstack.o
OBJ3 = threads.o
OBJ4 = dthreads.o
OBJ5 = bitops.o
OBJ6 = dbitops.o

$(STACK): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(STACK)

$(DEBUGSTACK): $(OBJ2)
	$(CC) $(CFLAGS) -DDEBUG=1 $(OBJ2) -o $(DEBUGSTACK)
	
$(THREADS): $(OBJ3)
	$(CC) $(CFLAGS) $(OBJ3) -o $(THREADS)

$(DEBUGTHREADS): $(OBJ4)
	$(CC) $(CFLAGS) -DDEBUG=1 $(OBJ4) -o $(DEBUGTHREADS)
	
$(BITOPS): $(OBJ5)
	$(CC) $(CFLAGS) $(OBJ5) -o $(BITOPS)

$(DEBUGBITOPS): $(OBJ6)
	$(CC) $(CFLAGS) -DDEBUG=1 $(OBJ6) -o $(DEBUGBITOPS)

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

dstack.o: stack.c
	$(CC) $(CFLAGS) -c -DDEBUG=1 stack.c -o dstack.o

threads.o: threads.c
	$(CC) $(CFLAGS) -c threads.c

dthreads.o: threads.c
	$(CC) $(CFLAGS) -c -DDEBUG=1 threads.c -o dthreads.o

bitops.o: bitops.c
	$(CC) $(CFLAGS) -c bitops.c

dbitops.o: bitops.c
	$(CC) $(CFLAGS) -c -DDEBUG=1 bitops.c -o dbitops.o

all: $(STACK) $(DEBUGSTACK) $(THREADS) $(DEBUGTHREADS) $(BITOPS) $(DEBUGBITOPS)

clean:
	$(RM) $(STACK) $(DEBUGSTACK) $(THREADS) $(DEBUGTHREADS) $(BITOPS) $(DEBUGBITOPS) $(OBJ) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6)
