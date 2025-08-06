#ifndef TEST_H
#define TEST_H

#include "towl_test.h"

#define FLOAT_PRECISION 0.0001
#define PARSER_POS_BUFF_SIZE 512

extern towl_table *root;

void test_array_ops_table();
void test_array_ops_string();
void test_array_ops_int();
void test_array_ops_float();
void test_array_ops_bool();
void test_array_ops_array();
void test_array_ops_datetime();
void test_array_ops_errors();

void test_static_singlekey_table();
void test_static_singlekey_string();
void test_static_singlekey_int();
void test_static_singlekey_float();
void test_static_singlekey_bool();
void test_static_singlekey_array();
void test_static_singlekey_datetime();
void test_static_singlekey_errors();

void test_static_string();

void test_static_barekey();
void test_static_key();

void test_static_parse_numeric_int_dec();
void test_static_parse_numeric_int_hex();
void test_static_parse_numeric_int_oct();
void test_static_parse_numeric_int_bin();

void test_static_parse_numeric_float();
void test_static_parse_numeric_date();
void test_static_parse_numeric_time();
void test_static_parse_numeric_datetime();

void test_static_parse_array();

#endif
