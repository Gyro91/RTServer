CC=gcc
CFLAGS=-Wall 

all: server

server : server.o dispatcher server.c
	$(CC) $(CFLAGS) -o server server.c
	
dispatcher : dispatcher.o
	$(CC) -o dispatcher dispatcher.c			

dispatcher.o : messages.h dispatcher.c
	$(CC) $(CFLAGS) -c dispatcher.c				

clean: 
	rm -f core *.o 