CC=gcc
CFLAGS=-Wall 

all: generator

generator: generator.o
	$(CC)  generator.o -o generator
	
generator.o: generator.c messages.h
	$(CC) $(CFLAGS) -c generator.c 
		 
clean: 
	rm -f core *.o 