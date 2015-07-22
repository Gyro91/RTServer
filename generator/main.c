#include <stdio.h>
#include <unistd.h>
#include "messages.h"
#include "fun_gen.h"
#define LOOP 1


struct sockaddr_in srv_addr; /** server address */
int sk; /* socket for communication between generator and server */


int main(int argc, char *argv[])
{
	// init
	char *addr; /** destination address server */
	int dport; /** destination port server*/

	// connection TCP with dispatcher

	while( LOOP ){
		// drawing

		// creating

		// forwarding

		// sleep to next period
	}

	return 0;

}
