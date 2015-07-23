#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "messages.h"
#define LOOP 1

struct sockaddr_in my_addr, cl_addr; /** struct with address for server and client */
int sk, cn_sk; /** socket and connected socket */


/** @brief waits a signal from both consumers
 *
 *  when consumers have set up, it allows dispatcher to do its task
 *
 * */
void wait_for_signal()
{


}

/** @brief receive packet from generator
 *
 *  it receives size,then the packet and generates a message_t
 *
 * 	@return original message,composed from type and payload
 */
/*
message_t receive_pkt()
{
	// buffering
}
*/
/**
 * @brief setup for listening
 *
 * it does prelimanry operations for a connection TCP
 */

void setup_TCP_server()
{
	int ret;
	socklen_t addrlen;

	if( ( sk = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("Server-socket() error!");
		exit(1);
	}

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(1234);

	if( (ret = bind(sk,(struct sockaddr*)&my_addr, sizeof(my_addr))) == -1 ){
		perror("Server-bind() error!");
		exit(1);
	}

	if( (ret = listen(sk,1)) == -1 ){
		perror("Server-listen() error!");
		exit(1);
	}

	addrlen = sizeof(cl_addr);

	if( (cn_sk = accept(sk, (struct sockaddr*)&cl_addr, &addrlen)) == -1 ){
		perror("Server-accept() error!");
		exit(1);
	}
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
	printf("#Dispatcher created\n");

	// waiting for a signal from consumers(condition variable)

	// setup for listening

	// accept

	while( LOOP ){

		setup_TCP_server();
		getchar();
		break;
		// receive

		// decide those who to forward

		// forwarding

		// sleep to next period
	}

	return 0;

}
