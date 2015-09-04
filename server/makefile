CC=gcc
CFLAGS=-Wall 

all: server

server : server.o dispatcher server.c consumers.c consumers
	$(CC) $(CFLAGS) -o server server.c
	
dispatcher : dispatcher.o 
	$(CC) -o dispatcher dispatcher.c			

dispatcher.o : messages.h dispatcher.c
	$(CC) $(CFLAGS) -c dispatcher.c				

consumers : consumers.o consumers.c
	$(CC) $(CFLAGS) -o consumer1 consumers.c util.c -lpthread  
	$(CC) $(CFLAGS) -o consumer2 consumers.c util.c -lpthread  
	$(CC) $(CFLAGS) -o output output.c
	
clean: 
	rm -f core *.o 