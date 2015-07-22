CC=gcc
CFLAGS=-Wall 
CWHERE=-L/usr/local/lib	
CUNIT=-lcunit  

all: test 

generator.o:../generator.c
	$(CC) $(CFLAGS) -c ../generator.c 

test: generator.o ../generator.c
	$(CC) $(CFLAGS) $(WHERE) -o test_generator test_generator.c  generator.o $(CUNIT)
	
clean: 
	rm -f core *.o 
 
