CC=gcc
CFLAGS=-Wall 

all: server

server : server.o dispatcher server.c consumers.c consumers
	$(CC) $(CFLAGS) -o server server.c
	
dispatcher : dispatcher.c messages.h dispatcher.o
	$(CC) -o dispatcher dispatcher.c -lrt			

consumers : consumers.o consumers.c
	$(CC) $(CFLAGS) -o consumer1 consumers.c util.c -lpthread  -lrt
	$(CC) $(CFLAGS) -o consumer2 consumers.c util.c -lpthread  -lrt
	$(CC) $(CFLAGS) -o output output.c
	
clean: 
	rm -f core *.o 