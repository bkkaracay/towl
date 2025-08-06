#ifndef TESTER_H
#define TESTER_H

#include <stddef.h>
#include <inttypes.h>

void TEST(void (*f)(), const char *test_name);

void ASSERT_EQUAL_STRING(const char *a, const char *b);
void ASSERT_EQUAL_INT(int a, int b);
void ASSERT_EQUAL_INT64(int64_t a, int64_t b);
void ASSERT_EQUAL_FLOAT(double a, double b, double precision);
void ASSERT_EQUAL_POINTER(const void* a, const void* b);

void ASSERT_NOT_EQUAL_INT(int a, int b);

#endif
