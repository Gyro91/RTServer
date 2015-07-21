#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"

//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"

#include "../fun_gen.h"
#include "../messages.h"
#include <stdio.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* Test case functions ****************/



void test_case_sample(void)
{
	CU_ASSERT(CU_TRUE);
	CU_ASSERT_NOT_EQUAL(2, -1);
	CU_ASSERT_STRING_EQUAL("string #1", "string #1");
	CU_ASSERT_STRING_NOT_EQUAL("string #1", "string #2");

	CU_ASSERT(CU_FALSE);
	CU_ASSERT_EQUAL(2, 3);
	CU_ASSERT_STRING_NOT_EQUAL("string #1", "string #1");
	CU_ASSERT_STRING_EQUAL("string #1", "string #2");
}



/* 	TEST INIT_GENERATOR
 *  it checks if the cpu is the same like it was set,then
 *  it sets another cpu and checks again
 *
 */

void init_generator_test()
{
	cpu_set_t bitmap;

	init_generator(); // setting current cpu to 0
	sched_getaffinity(0, sizeof(bitmap), &bitmap);

	CU_ASSERT_EQUAL(CPU_COUNT(&bitmap), 1); // checking if number of cpu > 0
	CU_ASSERT_NOT_EQUAL(CPU_ISSET(0, &bitmap),0); // checking if it is set cpu number 0

	CPU_ZERO(&bitmap);
	CPU_SET(1,&bitmap);
	sched_setaffinity(0, sizeof(bitmap), &bitmap);
	init_generator();
	sched_getaffinity(0, sizeof(bitmap), &bitmap);

	CU_ASSERT_EQUAL(CPU_COUNT(&bitmap),1);
	CU_ASSERT_NOT_EQUAL(CPU_ISSET(0, &bitmap), 0);

}

/*  TEST DRAW_MESSAGE
 * it has to return  only 2 values: TYPE 1 or TYPE2
 */

void draw_message_test()
{
	int i;
	enum msg_type my_type;

	for (i = 0; i < 5000; i++) {
		my_type = draw_message();
		CU_ASSERT( my_type == TYPE1 || my_type == TYPE2 );
	}

}


/************* Test Runner Code goes here **************/

int main ( void )
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if ( CUE_SUCCESS != CU_initialize_registry() )
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite( "test_suite", init_suite, clean_suite );
	if ( NULL == pSuite ) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	if (	(NULL == CU_add_test(pSuite, "draw_message_test", draw_message_test)) ||
			(NULL == CU_add_test(pSuite, "init_generator_test", init_generator_test))
	)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Run all tests using the basic interface
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	printf("\n");
	CU_basic_show_failures(CU_get_failure_list());
	printf("\n\n");
	/*
   // Run all tests using the automated interface
   CU_automated_run_tests();
   CU_list_tests_to_file();

   // Run all tests using the console interface
   CU_console_run_tests();
	 */
	/* Clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}

