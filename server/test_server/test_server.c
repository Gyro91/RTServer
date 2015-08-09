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
#include "../consumers.h"


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

struct response_task *rt;

/** inserting a new element at the head in the list of response time */
void insert_rt()
{
	struct response_task *qt = NULL; /* temp pointer */

	qt = (struct response_task *)malloc(sizeof(struct response_task));

	if( rt == NULL ) /* no elements in the list */
		rt = qt;
	else{
		qt->next = rt;
		rt = qt;
	}

}


/** deleting the head element of the list */
void delete_rt()
{
	struct response_task *qt = NULL;

	if( rt != NULL ){
		qt = rt;
		rt = rt->next;
		free(qt);
	}

}

/** counting number of elements in the list*/
int count_rt()
{
	int size = 0;
	struct response_task *qt = rt;

	for(qt = rt; qt != 0; qt = qt->next)
		size++;

	return size;

}

void test_f_list()
{
	int i;

	CU_ASSERT_EQUAL(0, count_rt());

	insert_rt();

	CU_ASSERT_EQUAL(1, count_rt());

	for(i = 0; i < 5; i++)
		insert_rt();
	CU_ASSERT_EQUAL(6, count_rt());

	delete_rt();
	CU_ASSERT_EQUAL(5, count_rt());

	for(i = 0; i < 6; i++)
		delete_rt(rt);
	CU_ASSERT_EQUAL(0, count_rt());
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
	if (	(NULL == CU_add_test(pSuite, "test_f_list", test_f_list))	)
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

