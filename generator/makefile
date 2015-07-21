CC=gcc
CFLAGS=-Wall 

all: main

main: main.o
	$(CC)  main.o -o generator
	
main.o: generator.c messages.h main.c
	$(CC) $(CFLAGS) -c main.c 
		 
clean: 
	rm -f core *.o 