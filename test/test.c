#include "tester.h"
#include "test.h"
#include "malloc.h"

towl_table *root;

int main() {
	root = towl_new_table();

	TEST(test_array_ops_table, "ARRAY OPERATIONS TABLE");
	TEST(test_array_ops_string, "ARRAY OPERATIONS STRING");
	TEST(test_array_ops_int, "ARRAY OPERATIONS INT");
	TEST(test_array_ops_float, "ARRAY OPERATIONS FLOAT");
	TEST(test_array_ops_bool, "ARRAY OPERATIONS BOOL");
	TEST(test_array_ops_array, "ARRAY OPERATIONS ARRAY");
	TEST(test_array_ops_datetime, "ARRAY OPERATIONS DATETIME");
	TEST(test_array_ops_errors, "ARRAY OPERATIONS ERROR CASES");
	
	TEST(test_static_singlekey_table,
	     "(INTERNAL) SINGLEKEY FUNCTIONS TABLE");
	TEST(test_static_singlekey_string,
	     "(INTERNAL) SINGLEKEY FUNCTIONS STRING");
	TEST(test_static_singlekey_int,
	     "(INTERNAL) SINGLEKEY FUNCTIONS INT");
	TEST(test_static_singlekey_float,
	     "(INTERNAL) SINGLEKEY FUNCTIONS FLOAT");
	TEST(test_static_singlekey_bool,
	     "(INTERNAL) SINGLEKEY FUNCTIONS BOOL");
	TEST(test_static_singlekey_array,
	     "(INTERNAL) SINGLEKEY FUNCTIONS ARRAY");
	TEST(test_static_singlekey_datetime,
	     "(INTERNAL) SINGLEKEY FUNCTIONS DATETIME");
	TEST(test_static_singlekey_errors,
	     "(INTERNAL) SINGLEKEY FUNCTIONS ERROR CASES");
	
	TEST(test_static_string, "(INTERNAL) PARSE STRING");
	TEST(test_static_barekey, "(INTERNAL) PARSE BAREKEY");
	TEST(test_static_key, "(INTERNAL) PARSE KEY");

	TEST(test_static_parse_numeric_int_dec,
	     "(INTERNAL) PARSE NUMERIC INT DECIMAL");
	TEST(test_static_parse_numeric_int_hex,
	     "(INTERNAL) PARSE NUMERIC INT HEXADECIMAL");
	TEST(test_static_parse_numeric_int_oct,
	     "(INTERNAL) PARSE NUMERIC INT OCTAL");
	TEST(test_static_parse_numeric_int_bin,
	     "(INTERNAL) PARSE NUMERIC INT BINARY");
	
	TEST(test_static_parse_numeric_float, "(INTERNAL) PARSE NUMERIC FLOAT");
	TEST(test_static_parse_numeric_date, "(INTERNAL) PARSE NUMERIC LOCAL DATE");
	TEST(test_static_parse_numeric_time, "(INTERNAL) PARSE NUMERIC LOCAL TIME");
	TEST(test_static_parse_numeric_datetime,
	     "(INTERNAL) PARSE NUMERIC DATETIME");

	TEST(test_static_parse_array, "(INTERNAL) PARSE ARRAY");
	
	towl_clean_table(root);
}
