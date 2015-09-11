#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>
#include "messages.h"
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
#define LOOP 1
#define __sched_priority sched_priority


struct sockaddr_in my_addr, cl_addr; /** struct with address for server and client */
int sk, cn_sk; /** socket and connected socket */
int status_c = 0; /** status of connection */

mqd_t mq1, mq2; /** Descriptor for each queue */
char *wc = "/tmp/wait_c";



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

void write_queue(mqd_t mq, char *buffer, message_t *mess)
{
	CHECK(0 <= mq_send(mq, (char *)mess, DIM_MAX_PAYLOAD, 0));

	CHECK(0 <= mq_send(mq, buffer, DIM_MAX_PAYLOAD, 0));
}

/** @brief receive packet from generator
 *
 *  it receives size,then the packet and generates a message_t
 *
 * 	@return original message,composed from type and payload
 */

int receive_dispatch_pkt()
{
	int ret;
	message_t mess;
	char *buffer;

	ret = recv(cn_sk, (void *)&mess, sizeof(mess), MSG_WAITALL); /* receiving struct*/
	handle_error_recv(ret);

	/* taking arrival time */
	clock_gettime(CLOCK_MONOTONIC, &mess.arrival_time);

	if( status_c ){
		buffer = (char *)malloc(mess.size);
		ret = recv(cn_sk, buffer, mess.size, MSG_WAITALL); /* receive message */
		handle_error_recv(ret);

		/* if type1 sending to consumer 1,
		 * otherwise sending to consumer2 */

		if( !mess.type )
			write_queue(mq1, buffer, &mess);
		else
			write_queue(mq2, buffer, &mess);

		free(buffer);
	}

	return mess.size;

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
	my_addr.sin_port = htons(1296);

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

void set_scheduler()
{
	int ret;
	struct sched_attr attr;

	attr.size = sizeof(attr);
	attr.sched_flags = 0;
	attr.sched_nice = 0;
	attr.sched_priority = 0;

	attr.sched_policy = SCHED_DEADLINE;
	attr.sched_runtime = 1 * 1000 * 1000;
	attr.sched_period = attr.sched_deadline = attr.sched_runtime / 0.95;

	ret = sched_setattr(0, &attr, 0);
	if (ret < 0) {
		perror("Error set_attr Dispatcher");
		exit(1);
	}



}

/** Opens queues for writing to consumers */

void open_queues()
{
	/* Open the mail queue1 */

	while( (mq1 = mq_open(QUEUE_NAME1, O_WRONLY)) == -1 );
	CHECK((mqd_t)-1 != mq1);

	/* Open the mail queue2 */

	while( (mq2 = mq_open(QUEUE_NAME2, O_WRONLY)) == -1);
	CHECK((mqd_t)-1 != mq2);

	printf("#Dispatcher ready\n");
}

int main(int argc, char *argv[])
{	

	printf("#Dispatcher created\n");

	/* TEST
   cpu_set_t bitmap;

	sched_getaffinity(0,sizeof(bitmap), &bitmap);
	if( CPU_COUNT(&bitmap) == 1 )
		printf("test1 dispatcher ok\n");
	if( CPU_ISSET(1, &bitmap) != 0  )
		printf("test2 dispatcher ok\n");
	else
		printf("test failed\n");
	 */

	open_queues();
	set_scheduler();

	// setup for a connection TCP
	setup_TCP_server();
	accept_connection();

	while( LOOP ){
		// receive
		receive_dispatch_pkt();

		if( !status_c )
			accept_connection();

	}

	return 0;

}
