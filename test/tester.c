#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "tester.h"

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RESET "\x1b[0m"

static char *current_test;
static int success;
static int used_func_count;

static double fabs(double f) {
	if(f < 0)
		return -f;
	
	return f;
}

void TEST(void (*f)(), const char *test_name) {
	if(test_name == NULL) {
		fprintf(stderr, "TESTER: Test name can't be NULL.");
		return;
	}

	current_test = strdup(test_name);
	if(current_test == NULL) {
		fprintf(stderr, "TESTER:%s: Cannot able to allocate memory.",
		        test_name);
		return;
	}
	
	success = 1;
	used_func_count = 0;

	clock_t begin = clock();
	(*f)();
	double exectime = (double) (clock() - begin) / CLOCKS_PER_SEC;
	
	if(success) {
		printf("%s: " ANSI_GREEN "SUCCESS (%f sec)\n" ANSI_RESET, current_test,
		       exectime);
	} else {
		printf("%s: " ANSI_RED "FAIL (%f sec)\n" ANSI_RESET, current_test,
		       exectime);
	}

	printf("-----------------------------------------------------------\n");
	
	free(current_test);
}

//EQUAL
void ASSERT_EQUAL_STRING(const char *a, const char *b) {
	used_func_count++;
	int cmp;
	
	if(a == NULL || b == NULL || (cmp = strcmp(a, b))) {
		success = 0;	
		printf("FUNCTION %d FAILED:\n", used_func_count);
	}

	if(a == NULL && b == NULL)
		printf("    ASSERT_EQUAL_STRING: <NULL> | <NULL>\n\n");
	else if(a == NULL)
		printf("    ASSERT_EQUAL_STRING: <NULL> | %s\n\n", b);
	else if(b == NULL)
		printf("    ASSERT_EQUAL_STRING: %s | <NULL>\n\n", a);
	else if(cmp)
		printf("    ASSERT_EQUAL_STRING: %s | %s\n\n", a, b);
}

void ASSERT_EQUAL_INT(int a, int b) {
	used_func_count++;
	
	if(a != b) {
		success = 0;	
		printf("FUNCTION %d FAILED:\n", used_func_count);
		printf("    ASSERT_EQUAL_INT: %d | %d\n\n", a, b);
	}
}

void ASSERT_EQUAL_INT64(int64_t a, int64_t b) {
	used_func_count++;
	
	if(a != b) {
		success = 0;	
		printf("FUNCTION %d FAILED:\n", used_func_count);
		printf("    ASSERT_EQUAL_INT64: %" PRId64 " | %" PRId64 "\n\n", a, b);
	}
}

void ASSERT_EQUAL_FLOAT(double a, double b, double precision) {
	used_func_count++;
		
	if(fabs(a - b) >= precision) {
		success = 0;	
		printf("FUNCTION %d FAILED:\n", used_func_count);
		printf("    ASSERT_EQUAL_FLOAT: %f | %f\n\n", a, b);
	}
}

void ASSERT_EQUAL_POINTER(const void* a, const void* b) {
	used_func_count++;
	int cmp;
	
	if(a == NULL || b == NULL || (cmp = (a != b))) {
		success = 0;	
		printf("FUNCTION %d FAILED:\n", used_func_count);
	}

	if(a == NULL && b == NULL)
		printf("    ASSERT_EQUAL_POINTER: <NULL> | <NULL>\n\n");
	else if(a == NULL)
		printf("    ASSERT_EQUAL_POINTER: <NULL> | %s\n\n", b);
	else if(b == NULL)
		printf("    ASSERT_EQUAL_POINTER: %s | <NULL>\n\n", a);
	else if(cmp)
		printf("    ASSERT_EQUAL_POINTER: %s | %s\n\n", a, b);
}

//NOT_EQUAL
void ASSERT_NOT_EQUAL_INT(int a, int b) {
	used_func_count++;

	if(a == b) {
		success = 0;	
		printf("FUNCTION %d FAILED:\n", used_func_count);
		printf("    ASSERT_NOT_EQUAL_INT: %d | %d\n\n", a, b);
	}
}

