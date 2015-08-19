#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "fun_gen.h"
#define LOOP 1

struct sockaddr_in srv_addr; /** server address */
int sk; /* socket for communication between generator and server */


int main(int argc, char *argv[])
{
	struct timespec t;
	int period = 1250; /** generator is a process periodic */
	char *payload;
	message_t mess; /** message to be delivered */

	// init: taking cpu
	init_generator();

	// connection TCP with server
	setup_TCP_client();

	srand(time(NULL));
	clock_gettime(CLOCK_MONOTONIC, &t);
	time_add_ms(&t, period);
	while( LOOP ){

		// creating message
		generate_message(&mess);

		// generating payload
		payload = (char *) malloc(mess.size);
		generate_payload(payload, mess.size); // random payload

		// forwarding
		send_pkt(&mess, payload);

		// free payload for next packet
		free(payload);

		// sleep to next period
		clock_nanosleep(CLOCK_MONOTONIC,
				TIMER_ABSTIME, &t, NULL);
		time_add_ms(&t, period);


	}

	return 0;

}
