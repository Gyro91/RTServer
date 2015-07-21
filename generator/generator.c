#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "messages.h"
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>

/**
  * assigns a core to the process
  */

void init_generator()
{
	cpu_set_t bitmap;

	CPU_ZERO(&bitmap); // resetting bitmap
	CPU_SET(0, &bitmap); // setting bitmap to zero

	sched_setaffinity(0, sizeof(bitmap), &bitmap); // taking cpu-0
}



/** @brief decides randomly what kind of message to forward
 *  @return type of message
 * */
enum msg_type draw_message()
{
	int random = rand() % 2;

	if( random )
		return TYPE1;
	else
		return TYPE2;
}



/*
 * size = rand() % 100
 * char * msg = malloc(sizeof(char) * size + sizeof(enum)+1)
 * generate_message(msg, size)
 *
 *  int generate_message(char *message, int payload_size)*/
