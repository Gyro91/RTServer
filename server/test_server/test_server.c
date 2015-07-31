#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"

//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>

void set_cpu(int pid1, int pid2, int pid3)
{
	cpu_set_t bitmap;

	CPU_ZERO(&bitmap); // resetting bitmap
	CPU_SET(1, &bitmap); // setting bitmap to zero

	sched_setaffinity(pid1, sizeof(bitmap), &bitmap); /* pid1 takes cpu1*/

	CPU_ZERO(&bitmap); // resetting bitmap
	CPU_SET(2, &bitmap); // setting bitmap to zero

	sched_setaffinity(pid2, sizeof(bitmap), &bitmap); /* pid2 takes cpu2*/

	CPU_ZERO(&bitmap); // resetting bitmap
	CPU_SET(3, &bitmap); // setting bitmap to zero

	sched_setaffinity(pid2, sizeof(bitmap), &bitmap); /* pid3 takes cpu3*/
}

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

void test_cpu1()
{
	cpu_set_t bitmap;

	CU_ASSERT_EQUAL(CPU_COUNT(&bitmap), 1); // checking if number of cpu > 0
	CU_ASSERT_NOT_EQUAL(CPU_ISSET(1, &bitmap),1); // checking if it is set cpu number 1
}

void test_cpu2()
{
	cpu_set_t bitmap;

	CU_ASSERT_EQUAL(CPU_COUNT(&bitmap), 1); // checking if number of cpu > 0
	CU_ASSERT_NOT_EQUAL(CPU_ISSET(2, &bitmap),1); // checking if it is set cpu number 2

}

void test_cpu3()
{
	cpu_set_t bitmap;

	CU_ASSERT_EQUAL(CPU_COUNT(&bitmap), 1); // checking if number of cpu > 0
	CU_ASSERT_NOT_EQUAL(CPU_ISSET(3, &bitmap),1); // checking if it is set cpu number 3

}

void test_set_cpu()
{
	int pid1, pid2 , pid3 ,
		wait_c = 1, status;

	pid1 = fork(); // creating first child
	if( pid1 != 0 )
		pid2 = fork(); // creating second child
	if( pid1 != 0 && pid2 != 0 )
		pid3 = fork(); // creating third child
	if ( pid1 != 0 && pid2 != 0 && pid3 != 0){
		set_cpu(pid1, pid2 ,pid3);
		wait_c = 0;
		wait(&status);
	}


	if( pid1 == 0 )
		test_cpu1();

	if( pid2 == 0)
		test_cpu2();

	if( pid3 == 0 )
		test_cpu3();

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
	if (	(NULL == CU_add_test(pSuite, "test_set_cpu", test_set_cpu))	)
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

