CC=gcc
CFLAGS=-Wall 

all: main

main: main.o generator.o generator.c main.c 
	$(CC) $(CFLAGS) main.c generator.c -o generator  
		 
clean: 
	rm -f core *.o 