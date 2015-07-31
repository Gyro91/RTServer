CC=gcc
CFLAGS=-Wall 
CWHERE=-L/usr/local/lib	
CUNIT=-lcunit  
TEST=-DTEST

all: test 

test: test_server.o test_server.c
	$(CC) $(CFLAGS) $(WHERE) -o test_server test_server.c $(CUNIT)
	
clean: 
	rm -f core *.o 
 
