CC=gcc
CFLAGS=-Wall 

all: main

main: main.o generator.o
	$(CC)  generator.c main.o -o generator
	
main.o: generator.c messages.h main.c fun_gen.h
	$(CC) $(CFLAGS) -c main.c 
	
		 
clean: 
	rm -f core *.o 