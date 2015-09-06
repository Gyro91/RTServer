#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "fun_gen.h"
#include <ctype.h>
#define LOOP 1

struct sockaddr_in srv_addr; /** server address */
int sk; /* socket for communication between generator and server */


int main(int argc, char *argv[])
{
	struct timespec t;
	int c, number;
	unsigned int period;
	char *payload;
	message_t mess; /** message to be delivered */

	/* Get the values for period e number of packets to send */

	if (argv[optind] == NULL || argv[optind + 1] == NULL) {
		    printf("Mandatory argument(s) missing\n");
		    exit(1);
	}

	while ((c = getopt (argc, argv, "p:m:")) != -1) {
	    switch (c)
	    {
	      case 'p':
	        period = atoi(optarg);
	        break;
	      case 'm':
	        number = atoi(optarg);
	        break;
	      case '?':
	        if (optopt == 't')
	          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	        else if (isprint (optopt))
	          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	        else
	          fprintf (stderr,
	                   "Unknown option character `\\x%x'.\n",
	                   optopt);
	        return 1;
	      default:
	        abort ();
	    }
	 }


	// init: taking cpu
	init_generator();

	// connection TCP with server
	setup_TCP_client();

	srand(time(NULL));
	clock_gettime(CLOCK_MONOTONIC, &t);
	time_add_micros(&t, period);
	while( LOOP ){

		if( !number )
			break;

		// creating message
		generate_message(&mess);

		// generating payload
		payload = (char *) malloc(mess.size);
		generate_payload(payload, mess.size); // random payload

		// forwarding
		send_pkt(&mess, payload);

		// free payload for next packet
		free(payload);
		number--;

		// sleep to next period
		clock_nanosleep(CLOCK_MONOTONIC,
				TIMER_ABSTIME, &t, NULL);
		time_add_micros(&t, period);


	}

	close(sk);

    printf("#Generator finished\n");

	return 0;

}
