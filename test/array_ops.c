#include <stdlib.h>

#include "tester.h"
#include "test.h"

const size_t EXAMPLE_SIZE = 30;

static size_t digit_count(size_t n) {
	int i = 0;
	while(n != 0) {
		n /= 10;
		i++;
	}

	return i;
}

void test_array_ops_table() {
	enum towl_code err;
	towl_array *array = towl_new_array();

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		towl_table *table = towl_new_table();

		err = static_singlekey_add(table, "hello", &i, TOWL_INT);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		err = towl_array_add_table(array, table);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		
		towl_table *gettable;
		err = towl_array_get_table(array, i, &gettable);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		int64_t geti;
		err = static_singlekey_get(gettable, "hello", &geti, TOWL_INT);	
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		ASSERT_EQUAL_INT(geti, i);
	}

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		towl_table *newtable = towl_new_table();
		int64_t newi = EXAMPLE_SIZE - i;
		err = static_singlekey_add(newtable, "world", &newi, TOWL_INT);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		
		err = towl_array_set_table(array, i, newtable);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		
		towl_table *gettable;
		err = towl_array_get_table(array, i, &gettable);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		int64_t geti;
		err = static_singlekey_get(gettable, "world", &geti, TOWL_INT);	
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		ASSERT_EQUAL_INT(geti, newi);
	}

	towl_clean_array(array);
}

void test_array_ops_string() {
	enum towl_code err;
	towl_array *array = towl_new_array();
	int strmaxlen = digit_count(EXAMPLE_SIZE - 1);
	
	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		char str[strmaxlen + 1];
		sprintf(str, "%d", i);

		err = towl_array_add_string(array, str);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		const char *getstr;
		err = towl_array_get_string(array, i, &getstr);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);		
		ASSERT_EQUAL_STRING(getstr, str);	
	}
	
	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		char str[strmaxlen + 1];
		sprintf(str, "%d", EXAMPLE_SIZE - i);

		err = towl_array_set_string(array, i, str);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		const char *getstr;
		err = towl_array_get_string(array, i, &getstr);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);		
		ASSERT_EQUAL_STRING(getstr, str);
	}

	towl_clean_array(array);
}

void test_array_ops_int() {
	enum towl_code err;
	towl_array *array = towl_new_array();

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		err = towl_array_add_int(array, i);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		int64_t geti;
		err = towl_array_get_int(array, i, &geti);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);		
		ASSERT_EQUAL_INT(geti, i);	
	}

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		err = towl_array_set_int(array, i, EXAMPLE_SIZE - i);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		int64_t geti;
		err = towl_array_get_int(array, i, &geti);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		ASSERT_EQUAL_INT(geti, EXAMPLE_SIZE - i);
	}

	towl_clean_array(array);
}

void test_array_ops_float() {
	enum towl_code err;
	towl_array *array = towl_new_array();

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		err = towl_array_add_float(array, (double) i/100);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		double getf;
		err = towl_array_get_float(array, i, &getf);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);		
		ASSERT_EQUAL_FLOAT(getf, (double) i/100, FLOAT_PRECISION);	
	}

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		err = towl_array_set_float(array, i, (double) (EXAMPLE_SIZE - i)/100);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		double getf;
		err = towl_array_get_float(array, i, &getf);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		ASSERT_EQUAL_FLOAT(getf, (double) (EXAMPLE_SIZE - i)/100,
		                   FLOAT_PRECISION);
	}

	towl_clean_array(array);
}

void test_array_ops_bool() {
	enum towl_code err;
	towl_array *array = towl_new_array();

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		err = towl_array_add_bool(array, i % 2);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		bool getb;
		err = towl_array_get_bool(array, i, &getb);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);		
		ASSERT_EQUAL_INT(getb, i % 2);	
	}

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		err = towl_array_set_bool(array, i, (i % 3) != 0);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		bool getb;
		err = towl_array_get_bool(array, i, &getb);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		ASSERT_EQUAL_INT(getb, (i % 3) != 0);
	}

	towl_clean_array(array);
}

void test_array_ops_array() {
	enum towl_code err;
	towl_array *array = towl_new_array();

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		towl_array *inner_array = towl_new_array();
		err = towl_array_add_int(inner_array, i);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		err = towl_array_add_array(array, inner_array);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		towl_array *getinner;
		err = towl_array_get_array(array, i, &getinner);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		int64_t geti;
		err = towl_array_get_int(getinner, 0, &geti);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		ASSERT_EQUAL_INT(geti, i);
	}

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		towl_array *inner_array = towl_new_array();
		err = towl_array_add_int(inner_array, EXAMPLE_SIZE - i);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		err = towl_array_set_array(array, i, inner_array);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		towl_array *getinner;
		err = towl_array_get_array(array, i, &getinner);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		int64_t geti;
		err = towl_array_get_int(getinner, 0, &geti);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		ASSERT_EQUAL_INT(geti, EXAMPLE_SIZE - i);
	}

	towl_clean_array(array);
}

void test_array_ops_datetime() {
	enum towl_code err;
	towl_array *array = towl_new_array();

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		struct towl_datetime dt;
		dt.gmtoff = i;
		
		err = towl_array_add_datetime(array, dt);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		struct towl_datetime getdt;
		err = towl_array_get_datetime(array, i, &getdt);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);		
		ASSERT_EQUAL_INT(getdt.gmtoff, i);	
	}

	for(int i = 0; i < EXAMPLE_SIZE; i++) {
		struct towl_datetime dt;
		dt.gmtoff = EXAMPLE_SIZE - i;
	
		err = towl_array_set_datetime(array, i, dt);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

		struct towl_datetime getdt;
		err = towl_array_get_datetime(array, i, &getdt);
		ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
		ASSERT_EQUAL_INT(getdt.gmtoff, EXAMPLE_SIZE - i);
	}

	towl_clean_array(array);
}

void test_array_ops_errors() {
	enum towl_code err;
	
	towl_array *array = towl_new_array();
	towl_array_add_int(array, 34);
	towl_array_add_int(array, 35);
	towl_array_add_int(array, 36);

	double getf;
	err = towl_array_get_float(array, 2, &getf);
	ASSERT_EQUAL_INT(err, TOWLC_TYPE_MISMATCH);

	int64_t geti;
	err = towl_array_get_int(array, 1000, &geti);
	ASSERT_EQUAL_INT(err, TOWLC_INDEX_OUT_OF_RANGE);


	err = towl_array_set_int(array, 1000, 60);
	ASSERT_EQUAL_INT(err, TOWLC_INDEX_OUT_OF_RANGE);
	
	towl_clean_array(array);
}
