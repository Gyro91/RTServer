#include <stdio.h>
#include <unistd.h>
#include "messages.h"
#define LOOP 1
/*
 * TEST INIT
 * int test_init(int core){
 *  int x = get_affinity();
 *
 * 	if ( x == core )
 * 		return 1;
 * 	else return 0;
 * }
 */
/**
  * assigns a core to the process
  */
void init()
{

}

/*  TEST DRAW_MESSAGE
  it has to return  only 2 values: TYPE 1 OR TYPE 2

  int my_type, my_type1, my_type2;
  my_type1 = my_type2 = 0;
  while( LOOP ){
  	   my_type = draw_message;
  	   if( my_type != TYPE1 && my_type != TYPE2 ){
  	   	   perror("error draw_message");
  	   	   exit(1);
  	   }
	   else if( my_type == TYPE1 )
	   			my_type1++;
	   else if( my_type == TYPE2 )
	   	   		my_type2++;
  	   printf("status draw_message: ok\n
  	   	   	   	    my_type1:%d my_type2:%d,",my_type1,my_type2);
  }
*/

/** @brief decides randomly what kind of message to forward
 *  @return type of message
 * */
enum msg_type draw_message()
{


}

int max (int n1, int n2 )
{
   if ( n2 > n1 )  return n2;
   return n1;
}


