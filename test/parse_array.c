#include "test.h"
#include "tester.h"

void test_static_parse_array() {
	enum towl_code code;
	enum towl_type type;

	struct towl_parser *parser = static_new_parser("[ 14, 2.3e+3, \"hell\" ]");

	towl_array *array;
	code = static_parse_value(parser, &array, &type);	
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	
	int64_t geti;
	towl_array_get_int(array, 0, &geti);
	ASSERT_EQUAL_INT(geti, 14);

	double getf;
	towl_array_get_float(array, 1, &getf);
	ASSERT_EQUAL_FLOAT(getf, 2300, FLOAT_PRECISION);

	const char *gets;
	towl_array_get_string(array, 2, &gets);
	ASSERT_EQUAL_STRING(gets, "hell");
	//TODO continue
}


