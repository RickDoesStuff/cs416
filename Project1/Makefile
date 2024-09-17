CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address -O2
RM = rm -f

SPELLCHECK = spchk
DEBUGSPELLCHECK = dspchk

OBJ = spchk.o
OBJ2 = dspchk.o

$(SPELLCHECK): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(SPELLCHECK)

$(DEBUGSPELLCHECK): $(OBJ2)
	$(CC) $(CFLAGS) -DDEBUG=1 $(OBJ2) -o $(DEBUGSPELLCHECK)

spchk.o: spchk.c
	$(CC) -c spchk.c

dspchk.o: spchk.c
	$(CC) -c -DDEBUG=1 spchk.c -o dspchk.o

all: $(SPELLCHECK) $(DEBUGSPELLCHECK)

clean:
	$(RM) $(SPELLCHECK) $(DEBUGSPELLCHECK) $(OBJ) $(OBJ2)
