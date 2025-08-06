#include <string.h>

#include "tester.h"
#include "test.h"

void test_static_parse_numeric_int_dec() {
	enum towl_code code;
	enum towl_type type;
	int64_t i;
	
	struct towl_parser *parser = static_new_parser("+123_12_9");
	
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 123129);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "123_12_9");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 123129);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "-123_12_9");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, -123129);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "123_12ABCD_3");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 12312);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "123_1sanmasahimherkesi2_3");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 1231);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "-00123_12_9");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_LEADING_ZERO_ERR);

	static_change_parser_src(parser, "-_123_12_9");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, "-123_12_9_");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);
	
	static_change_parser_src(parser, "9223372036854775808");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NUMBER_OVERFLOW);
		
	static_change_parser_src(parser, "-9223372036854775808");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, INT64_MIN);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "9223372036854775809");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NUMBER_OVERFLOW);

	static_change_parser_src(parser, "+");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DIGIT_NOT_FOUND);
}

void test_static_parse_numeric_int_hex() {
	enum towl_code code;
	enum towl_type type;
	int64_t i;
	
	struct towl_parser *parser = static_new_parser("0x123A_B_C");

	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 1194684);

	static_change_parser_src(parser, "0x123_1sanmasahimherkesi2_3");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 4657);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "0x_123A_B_C");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, "0x123A_B_C_");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, "0x8000000000000000");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NUMBER_OVERFLOW);
	
	static_change_parser_src(parser, "0x");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DIGIT_NOT_FOUND);
}

void test_static_parse_numeric_int_oct() {
	enum towl_code code;
	enum towl_type type;
	int64_t i;
	
	struct towl_parser *parser = static_new_parser("0o123_12_3");

	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 42579);
	ASSERT_EQUAL_INT(type, TOWL_INT);
	
	static_change_parser_src(parser, "0o123_12ABC_3");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 5322);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "0o123_1sanmasahimherkesi2_3");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 665);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, "0o_123_12_3");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, "0o123_12_3_");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, "0o893_129_3");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DIGIT_NOT_FOUND);
	
	static_change_parser_src(parser, "0o1000000000000000000000");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NUMBER_OVERFLOW);

	static_change_parser_src(parser, "0o");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DIGIT_NOT_FOUND);
}

void test_static_parse_numeric_int_bin() {
	enum towl_code code;
	enum towl_type type;
	int64_t i;
	
	struct towl_parser *parser = static_new_parser(
		"0b100010000000000___00000000__0000000001_10000000");

	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 1168231104896);

	static_change_parser_src(parser,
		"0b_100010000000000___00000000__0000000001_10000000");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, 
		"0b100010000000000___00000000__0000000001_10000000_");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);	

	static_change_parser_src(parser, "0b1112");
	code = static_parse_numeric(parser, &i, &type);	
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT64(i, 7);
	ASSERT_EQUAL_INT(type, TOWL_INT);

	static_change_parser_src(parser, 
		"0b1000000000000_000000000000000000000000000000000000000000000000000");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NUMBER_OVERFLOW);

	static_change_parser_src(parser, "0b");
	code = static_parse_numeric(parser, &i, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DIGIT_NOT_FOUND);
}


void test_static_parse_numeric_float() {
	enum towl_code code;
	enum towl_type type;
	double f;
	
	struct towl_parser *parser = static_new_parser("12.23456");
	
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_FLOAT(f, 12.23456, FLOAT_PRECISION);
	ASSERT_EQUAL_INT(type, TOWL_FLOAT);

	static_change_parser_src(parser, "-34.56");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_FLOAT(f, -34.56, FLOAT_PRECISION);
	ASSERT_EQUAL_INT(type, TOWL_FLOAT);

	static_change_parser_src(parser, "+910.0");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_FLOAT(f, 910.0, FLOAT_PRECISION);
	ASSERT_EQUAL_INT(type, TOWL_FLOAT);

	static_change_parser_src(parser, "+14.53e+4");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_FLOAT(f, 145300.0, FLOAT_PRECISION);
	ASSERT_EQUAL_INT(type, TOWL_FLOAT);

	static_change_parser_src(parser, "+14.53e-2");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_FLOAT(f, 0.145300, FLOAT_PRECISION);
	ASSERT_EQUAL_INT(type, TOWL_FLOAT);

	static_change_parser_src(parser, "+14e-2");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_FLOAT(f, 0.14, FLOAT_PRECISION);
	ASSERT_EQUAL_INT(type, TOWL_FLOAT);
	

	static_change_parser_src(parser, "+014.53e+4");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_LEADING_ZERO_ERR);

	static_change_parser_src(parser, "+14.530e+4");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_TRAILING_ZERO_ERR);

	
	static_change_parser_src(parser, "_56.74e12");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, "56_.74e12");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, "56._74e12");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);

	static_change_parser_src(parser, "56.74_e12");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_UNDERSCORE_PLACEMENT);


	static_change_parser_src(parser, "56.");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_FLOAT_EMPTY_FRACT_PART);

	static_change_parser_src(parser, "56.e12");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_FLOAT_EMPTY_FRACT_PART);

	static_change_parser_src(parser, "56.74e");
	code = static_parse_numeric(parser, &f, &type);
	ASSERT_EQUAL_INT(code, TOWLC_FLOAT_EMPTY_EXP_PART);
}

void test_static_parse_numeric_date() {
	enum towl_code code;
	enum towl_type type;
	struct towl_datetime date;

	struct towl_parser *parser = static_new_parser("1453-05-29");
	
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(date.secfrac < 0, 1);
	ASSERT_EQUAL_INT(date.sec < 0, 1);
	ASSERT_EQUAL_INT(date.min < 0, 1);
	ASSERT_EQUAL_INT(date.hour < 0, 1);
	ASSERT_EQUAL_INT(date.mday, 29);
	ASSERT_EQUAL_INT(date.mon, 5);
	ASSERT_EQUAL_INT(date.year, 1453);
	ASSERT_EQUAL_INT(date.gmtoff < 0, 1);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "1453-05_29");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_NOT_EQUAL_INT(code, TOWLC_SUCCESS);
	
	static_change_parser_src(parser, "1453-5-29");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_NOT_EQUAL_INT(code, TOWLC_SUCCESS);
	
	static_change_parser_src(parser, "1453-0A-29");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NON_DIGIT_CHAR);
	
	static_change_parser_src(parser, "1453-05-2F");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NON_DIGIT_CHAR);

	
	static_change_parser_src(parser, "1453-00-29");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_MONTH_OUT_OF_RANGE);
	
	static_change_parser_src(parser, "1453-13-29");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_MONTH_OUT_OF_RANGE);

	
	static_change_parser_src(parser, "1453-05-00");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DAY_OUT_OF_RANGE);
	
	static_change_parser_src(parser, "1453-05-32");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DAY_OUT_OF_RANGE);

	//Check for months with 30 days
	static_change_parser_src(parser, "1453-04-31");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DAY_OUT_OF_RANGE);
	
	static_change_parser_src(parser, "1453-06-31");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DAY_OUT_OF_RANGE);
	
	static_change_parser_src(parser, "1453-09-31");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DAY_OUT_OF_RANGE);
	
	static_change_parser_src(parser, "1453-11-31");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DAY_OUT_OF_RANGE);

	//Check for february
	static_change_parser_src(parser, "1453-02-29");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DAY_OUT_OF_RANGE);
	
	static_change_parser_src(parser, "1452-02-30");
	code = static_parse_numeric(parser, &date, &type);
	ASSERT_EQUAL_INT(code, TOWLC_DAY_OUT_OF_RANGE);

}

void test_static_parse_numeric_time() {
	enum towl_code code;
	enum towl_type type;
	struct towl_datetime time;

	struct towl_parser *parser = static_new_parser("07:45:56");
	
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(time.secfrac < 0, 1);
	ASSERT_EQUAL_INT(time.sec, 56);
	ASSERT_EQUAL_INT(time.min, 45);
	ASSERT_EQUAL_INT(time.hour, 7);
	ASSERT_EQUAL_INT(time.mday < 0, 1);
	ASSERT_EQUAL_INT(time.mon < 0, 1);
	ASSERT_EQUAL_INT(time.year < 0, 1);
	ASSERT_EQUAL_INT(time.gmtoff < 0, 1);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "07:45:56.98765");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(time.secfrac, 98765);
	ASSERT_EQUAL_INT(time.sec, 56);
	ASSERT_EQUAL_INT(time.min, 45);
	ASSERT_EQUAL_INT(time.hour, 7);
	ASSERT_EQUAL_INT(time.mday < 0, 1);
	ASSERT_EQUAL_INT(time.mon < 0, 1);
	ASSERT_EQUAL_INT(time.year < 0, 1);
	ASSERT_EQUAL_INT(time.gmtoff < 0, 1);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "07.45:56");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_NOT_EQUAL_INT(code, TOWLC_SUCCESS);

	static_change_parser_src(parser, "07:45.56");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_NOT_EQUAL_INT(code, TOWLC_SUCCESS);

	static_change_parser_src(parser, "07:45");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_NOT_EQUAL_INT(code, TOWLC_SUCCESS);
	
	static_change_parser_src(parser, "07:E5:56");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NON_DIGIT_CHAR);
	
	static_change_parser_src(parser, "07:45:5C");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NON_DIGIT_CHAR);

	static_change_parser_src(parser, "07:45:56.9AF65");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(time.secfrac, 9);
	ASSERT_EQUAL_INT(time.sec, 56);
	ASSERT_EQUAL_INT(time.min, 45);
	ASSERT_EQUAL_INT(time.hour, 7);
	ASSERT_EQUAL_INT(time.mday < 0, 1);
	ASSERT_EQUAL_INT(time.mon < 0, 1);
	ASSERT_EQUAL_INT(time.year < 0, 1);
	ASSERT_EQUAL_INT(time.gmtoff < 0, 1);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "24:45:56");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_EQUAL_INT(code, TOWLC_HOUR_OUT_OF_RANGE);

	static_change_parser_src(parser, "07:60:56");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_EQUAL_INT(code, TOWLC_MIN_OUT_OF_RANGE);

	static_change_parser_src(parser, "07:45:60");
	code = static_parse_numeric(parser, &time, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SEC_OUT_OF_RANGE);
}
/*
void test_static_parse_offset() {
	enum towl_code code;
	long off;
	struct towl_parser *parser = static_new_parser("Z\nasf");

	code = static_parse_num(parser, &off);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(off, 0);

	static_change_parser_src(parser, "+08:00");
	code = static_parse_offset(parser, &off);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(off, 28800);

	static_change_parser_src(parser, "-08:00");
	code = static_parse_offset(parser, &off);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(off, 28800);
}*/

void test_static_parse_numeric_datetime() {
	enum towl_code code;
	enum towl_type type;
	struct towl_datetime dt;

	struct towl_parser *parser = static_new_parser("1986-09-30T07:32:09");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.secfrac < 0, 1);
	ASSERT_EQUAL_INT(dt.sec, 9);
	ASSERT_EQUAL_INT(dt.min, 32);
	ASSERT_EQUAL_INT(dt.hour, 7);
	ASSERT_EQUAL_INT(dt.mday, 30);
	ASSERT_EQUAL_INT(dt.mon, 9);
	ASSERT_EQUAL_INT(dt.year, 1986);
	ASSERT_EQUAL_INT(dt.gmtoff < 0, 1);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "1986-09-30 07:32:09");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.secfrac < 0, 1);
	ASSERT_EQUAL_INT(dt.sec, 9);
	ASSERT_EQUAL_INT(dt.min, 32);
	ASSERT_EQUAL_INT(dt.hour, 7);
	ASSERT_EQUAL_INT(dt.mday, 30);
	ASSERT_EQUAL_INT(dt.mon, 9);
	ASSERT_EQUAL_INT(dt.year, 1986);
	ASSERT_EQUAL_INT(dt.gmtoff < 0, 1);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "1986-09-30 07:32:09.987");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.secfrac, 987);
	ASSERT_EQUAL_INT(dt.sec, 9);
	ASSERT_EQUAL_INT(dt.min, 32);
	ASSERT_EQUAL_INT(dt.hour, 7);
	ASSERT_EQUAL_INT(dt.mday, 30);
	ASSERT_EQUAL_INT(dt.mon, 9);
	ASSERT_EQUAL_INT(dt.year, 1986);
	ASSERT_EQUAL_INT(dt.gmtoff < 0, 1);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "1986-09-30 07:32:09.987Z");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.secfrac, 987);
	ASSERT_EQUAL_INT(dt.sec, 9);
	ASSERT_EQUAL_INT(dt.min, 32);
	ASSERT_EQUAL_INT(dt.hour, 7);
	ASSERT_EQUAL_INT(dt.mday, 30);
	ASSERT_EQUAL_INT(dt.mon, 9);
	ASSERT_EQUAL_INT(dt.year, 1986);
	ASSERT_EQUAL_INT(dt.gmtoff, 0);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "1986-09-30 07:32:09.987ZQWERTY");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.secfrac, 987);
	ASSERT_EQUAL_INT(dt.sec, 9);
	ASSERT_EQUAL_INT(dt.min, 32);
	ASSERT_EQUAL_INT(dt.hour, 7);
	ASSERT_EQUAL_INT(dt.mday, 30);
	ASSERT_EQUAL_INT(dt.mon, 9);
	ASSERT_EQUAL_INT(dt.year, 1986);
	ASSERT_EQUAL_INT(dt.gmtoff, 0);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "1986-09-30 07:32:09.987+09:01");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.secfrac, 987);
	ASSERT_EQUAL_INT(dt.sec, 9);
	ASSERT_EQUAL_INT(dt.min, 32);
	ASSERT_EQUAL_INT(dt.hour, 7);
	ASSERT_EQUAL_INT(dt.mday, 30);
	ASSERT_EQUAL_INT(dt.mon, 9);
	ASSERT_EQUAL_INT(dt.year, 1986);
	ASSERT_EQUAL_INT(dt.gmtoff, 32460);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "1986-09-30 07:32:09.987-06:20");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(dt.secfrac, 987);
	ASSERT_EQUAL_INT(dt.sec, 9);
	ASSERT_EQUAL_INT(dt.min, 32);
	ASSERT_EQUAL_INT(dt.hour, 7);
	ASSERT_EQUAL_INT(dt.mday, 30);
	ASSERT_EQUAL_INT(dt.mon, 9);
	ASSERT_EQUAL_INT(dt.year, 1986);
	ASSERT_EQUAL_INT(dt.gmtoff, -22800);
	ASSERT_EQUAL_INT(type, TOWL_DATETIME);

	static_change_parser_src(parser, "1986-09-30 07:32:09.987-6:20");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NON_DIGIT_CHAR);

	static_change_parser_src(parser, "1986-09-30 07:32:09.987-06:2");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_NON_DIGIT_CHAR);

	static_change_parser_src(parser, "1986-09-30 07:32:09.987-06.20");
	code = static_parse_numeric(parser, &dt, &type);
	ASSERT_EQUAL_INT(code, TOWLC_GMTOFF_SYNTAX_ERR);
}
