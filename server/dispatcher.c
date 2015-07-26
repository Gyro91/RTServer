#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "messages.h"
#define LOOP 1

struct sockaddr_in my_addr, cl_addr; /** struct with address for server and client */
int sk, cn_sk; /** socket and connected socket */
int status_c = 0; /** status of connection */

char *myfifo1 = "/tmp/myfifo1";

/* add ms to the specific destination */
void time_add_ms(struct timespec *dst, long int ms)
{
	dst->tv_sec += ms/1000;
	dst->tv_nsec += (ms % 1000) * 1e6;

	if (dst->tv_nsec > 1e9) {
		dst->tv_nsec -= 1e9;
		dst->tv_sec++;
	}
}

/** @brief waits a signal from both consumers
 *
 *  when consumers have set up, it allows dispatcher to do its task
 *
 * */
void wait_for_signal()
{

}

/** handling return of recv: -1 error on socket, 0 if connection lost */
void handle_error_recv(int ret)
{
	if( ret == -1)
	{
		perror("Error recv size\n");
		exit(1);
	}

	if ( ret == 0 ){
		status_c = 0;
		printf("#Connection with generator:lost\n");
	}
}
/** @brief receive packet from generator
 *
 *  it receives size,then the packet and generates a message_t
 *
 * 	@return original message,composed from type and payload
 */

int receive_dispatch_pkt()
{
	int ret, fd;
	unsigned char size;
	char *buffer;
	char size_f[4];

	ret = recv(cn_sk, (void *)&size, 1, 0);
	handle_error_recv(ret);

	if( status_c ){
		buffer = (char *)malloc(size);
		ret = recv(cn_sk, (void *)buffer, size, MSG_WAITALL);
		handle_error_recv(ret);

		fd = open(myfifo1, O_WRONLY);
		sprintf(size_f, "%d", size);
		write(fd, size_f, 4);
		write(fd, buffer, size);
		close(fd);
/*
		printf("%s\n", buffer);
		printf("%d\n", size); */
		free(buffer);
	}

	return size;

}


/**
 * @brief setup for listening
 *
 * it does preliminary operations for a connection TCP
 */

void setup_TCP_server()
{
	int ret;
	int yes = 1;

	if( ( sk = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("Server-socket() error!");
		exit(1);
	}

	if(setsockopt( sk,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
		perror("Server-setsockopt() error!");
		exit(1);
	}

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(1235);

	if( (ret = bind(sk,(struct sockaddr*)&my_addr, sizeof(my_addr))) == -1 ){
		perror("Server-bind() error!");
		exit(1);
	}

	if( (ret = listen(sk,1)) == -1 ){
		perror("Server-listen() error!");
		exit(1);
	}

}
/** accepts a connection tcp and changes status_c*/

void accept_connection()
{
	socklen_t addrlen = sizeof(cl_addr);

	if( (cn_sk = accept(sk, (struct sockaddr*)&cl_addr, &addrlen)) == -1 ){
		perror("Server-accept() error!");
		exit(1);
	}
	status_c = 1; // there's a connection
	printf("#Connection with generator: accepted \n");

}

/**
 *  @brief forwards packet to a consumer
 *
 *  it forwards the message via IPC to the consumer
 *
 */
void forward_message(){


}

int main(int argc, char *argv[])
{	
	int period = 250; /** dispatcher is a process periodic */
	struct timespec t;

	printf("#Dispatcher created\n");

	// waiting for a signal from consumers(condition variable)

	// setup for a connection TCP
	setup_TCP_server();
	accept_connection();

	clock_gettime(CLOCK_MONOTONIC, &t);
	time_add_ms(&t, period);
	while( LOOP ){
		// receive
		receive_dispatch_pkt();

		if( !status_c )
			accept_connection();

		// sleep to next period
		clock_nanosleep(CLOCK_MONOTONIC,
				TIMER_ABSTIME, &t, NULL);
		time_add_ms(&t, period);

	}

	return 0;

}
