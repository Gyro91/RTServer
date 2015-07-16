CC=gcc
CFLAGS=-Wall 

all: generator

generator: generator.o

generator.o: generator.c
		 $(CC) -Wall -c generator.c 
		 
clean: 
	rm -f core *.o 