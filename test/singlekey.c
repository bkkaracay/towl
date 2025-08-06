#include <stdlib.h>
#include <string.h>

#include "tester.h"
#include "test.h"

void test_static_singlekey_table() {
	enum towl_code err;
	const char *TABLE_STRING_VAL = "Wow!";
	
	//Setup
	towl_table *table = towl_new_table();

	err = static_singlekey_add(table, "table_string", (void*) TABLE_STRING_VAL,
	                           TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	err = static_singlekey_add(root, "table", table, TOWL_TABLE);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	//GET
	towl_table *gettable;
	err = static_singlekey_get(root, "table", &gettable, TOWL_TABLE);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	char *tblstr;
	err = static_singlekey_get(gettable, "table_string", &tblstr, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(tblstr, TABLE_STRING_VAL);
	
	//New table Setup
	const bool NEWTABLE_BOOL_VAL = 1;
	const double NEWTABLE_FLOAT_VAL = 1456.5;

	towl_table *newtable = towl_new_table();
	err = static_singlekey_add(newtable, "table_bool", 
	                           (void*) &NEWTABLE_BOOL_VAL, TOWL_BOOL);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	//SET
	err = static_singlekey_set(root, "table", newtable, TOWL_TABLE);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	err = static_singlekey_get(root, "table", &gettable, TOWL_TABLE);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	bool ntblbool;
	err = static_singlekey_get(gettable, "table_bool", &ntblbool, TOWL_BOOL);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(ntblbool, NEWTABLE_BOOL_VAL);
}

void test_static_singlekey_string() {
	enum towl_code err;
	char *str;
	err = static_singlekey_add(root, "string", "This is a string", 
	                           TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	err = static_singlekey_get(root, "string", &str, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "This is a string");
	
	err = static_singlekey_set(root, "string", "Hello, World!",
	                           TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	err = static_singlekey_get(root, "string", &str, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "Hello, World!");
}

void test_static_singlekey_int() {
	enum towl_code err;
	int64_t i = 12;
	err = static_singlekey_add(root, "integer", &i, TOWL_INT);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	i = 0;
	err = static_singlekey_get(root, "integer", &i, TOWL_INT);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(i, 12);	
	
	i = 5;
	err = static_singlekey_set(root, "integer", &i, TOWL_INT);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	i = 0;
	err = static_singlekey_get(root, "integer", &i, TOWL_INT);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(i, 5);
}

void test_static_singlekey_float() {
	enum towl_code err;
	double f = 1.2;

	err = static_singlekey_add(root, "float", &f, TOWL_FLOAT);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	f = 0.0;
	err = static_singlekey_get(root, "float", &f, TOWL_FLOAT);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_FLOAT(f, 1.2, FLOAT_PRECISION);	
	
	f = 0.5;
	err = static_singlekey_set(root, "float", &f, TOWL_FLOAT);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	f = 0;
	err = static_singlekey_get(root, "float", &f, TOWL_FLOAT);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_FLOAT(f, 0.5, FLOAT_PRECISION);
}

void test_static_singlekey_bool() {
	enum towl_code err;
	bool b = 1;

	err = static_singlekey_add(root, "bool", &b, TOWL_BOOL);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	b = 0;
	err = static_singlekey_get(root, "bool", &b, TOWL_BOOL);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(b, 1);	

	b = 0;
	err = static_singlekey_set(root, "bool", &b, TOWL_BOOL);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	b = 1;
	err = static_singlekey_get(root, "bool", &b, TOWL_BOOL);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(b, 0);
}

void test_static_singlekey_array() {
	enum towl_code err;
	towl_array *array = towl_new_array();
	towl_array_add_int(array, 567);

	err = static_singlekey_add(root, "array", array, TOWL_ARRAY);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	towl_array *getarray;
	err = static_singlekey_get(root, "array", &getarray, TOWL_ARRAY);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	int64_t geti;
	err = towl_array_get_int(getarray, 0, &geti);	
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(geti, 567);

	towl_array *newarray = towl_new_array();
	err = towl_array_add_int(newarray, 123);

	err = static_singlekey_set(root, "array", newarray, TOWL_ARRAY);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	err = static_singlekey_get(root, "array", &getarray, TOWL_ARRAY);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	err = towl_array_get_int(getarray, 0, &geti);	
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(geti, 123);
}

void test_static_singlekey_datetime() {
	enum towl_code err;
	struct towl_datetime dt;

	dt.year = 12;
	dt.hour = 56;
	err = static_singlekey_add(root, "datetime", &dt, TOWL_DATETIME);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	
	dt.year = 0;
	dt.hour = 0;
	err = static_singlekey_get(root, "datetime", &dt, TOWL_DATETIME);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.year, 12);
	ASSERT_EQUAL_INT(dt.hour, 56);
	
	dt.year = 5;
	dt.hour = 45;
	err = static_singlekey_set(root, "datetime", &dt, TOWL_DATETIME);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);

	dt.year = 5;
	dt.hour = 45;
	err = static_singlekey_get(root, "datetime", &dt, TOWL_DATETIME);
	ASSERT_EQUAL_INT(err, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.year, 5);
	ASSERT_EQUAL_INT(dt.hour, 45);
}

void test_static_singlekey_errors() {
	enum towl_code err;
	char *str = "This is a string";
	int64_t i = 0;
	
	err = static_singlekey_add(root, NULL, str, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_NULL_KEY);

	err = static_singlekey_add(root, "string", str, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_KEY_ALREADY_EXISTS);


	err = static_singlekey_get(root, NULL, &str, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_NULL_KEY);

	err = static_singlekey_get(root, "sadfsadfasdfsaf", str, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_KEY_NOT_FOUND);
	
	err = static_singlekey_get(root, "string", &i, TOWL_INT);
	ASSERT_EQUAL_INT(err, TOWLC_TYPE_MISMATCH);


	err = static_singlekey_set(root, NULL, str, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_NULL_KEY);

	err = static_singlekey_set(root, "sadfsadaf", str, TOWL_STRING);
	ASSERT_EQUAL_INT(err, TOWLC_KEY_NOT_FOUND);
	
	err = static_singlekey_set(root, "string", &i, TOWL_INT);
	ASSERT_EQUAL_INT(err, TOWLC_TYPE_MISMATCH);
	
}
