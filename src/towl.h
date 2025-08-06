#ifndef TOWL_H
#define TOWL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>

//TODO Rethink the way arrays are used
//TODO Hide internal code and remove unused functions
//TODO Test on c99
//TODO Check for NULL after check
//TODO Reorganize towl_codes
//TODO Improve the comments and add new ones
enum towl_code {
	TOWLC_SUCCESS = 0,
	TOWLC_EOL,
	TOWLC_KEY_ALREADY_EXISTS,
	TOWLC_ILLEGAL_KEY,
	TOWLC_NULL_KEY,
	TOWLC_KEY_NOT_FOUND,
	TOWLC_TYPE_MISMATCH,
	TOWLC_INDEX_OUT_OF_RANGE,
	TOWLC_UNKNOWN_TYPE, //REMOVABLE
	TOWLC_TYPE_UNSUPPORTED,
	TOWLC_ILLEGAL_BAREKEY_CHAR,
	TOWLC_UNTERMINATED_STRING,
	TOWLC_ILLEGAL_HEX_CHAR,
	TOWLC_UNEXPECTED_NEWLINE,
	TOWLC_ILLEGAL_UNICODE,
	TOWLC_OUT_OF_MEMORY,
	TOWLC_UNKNOWN_ESCAPE_SEQ,
	TOWLC_ESCAPE_UNEXPECTED_EOF,
	TOWLC_EMPTY_BAREKEY,
	TOWLC_REDEFINITION_AS_TABLE,
	TOWLC_INVALID_VALUE, //20
	TOWLC_INT_SYNTAX_ERR,
	TOWLC_NUMBER_OVERFLOW,
	TOWLC_NUMBER_UNDERFLOW,
	TOWLC_LEADING_ZERO_ERR,
	TOWLC_DIGIT_NOT_FOUND,
	TOWLC_UNDERSCORE_PLACEMENT,
	TOWLC_NON_DIGIT_CHAR,
	TOWLC_TRAILING_ZERO_ERR,
	TOWLC_FLOAT_SYNTAX_ERR,
	TOWLC_FLOAT_EMPTY_FRACT_PART, //30
	TOWLC_FLOAT_EMPTY_EXP_PART,
	TOWLC_FLOAT_OVERFLOW,
	TOWLC_FLOAT_UNDERFLOW,
	TOWLC_TIME_SYNTAX_ERR,
	TOWLC_HOUR_OUT_OF_RANGE,
	TOWLC_MIN_OUT_OF_RANGE,
	TOWLC_SEC_OUT_OF_RANGE,
	TOWLC_DATE_SYNTAX_ERR,
	TOWLC_DAY_OUT_OF_RANGE,
	TOWLC_MONTH_OUT_OF_RANGE, //40
	TOWLC_SIGN_IN_RADIX_INT,
	TOWLC_GMTOFF_SYNTAX_ERR,

	TOWLC_EXPECTED_BRACKET,
	TOWLC_EXPECTED_NEWLINE,
};

//TODO Hide implementation
enum towl_type {
	TOWL_TABLE,
	TOWL_STRING,
	TOWL_INT,
	TOWL_FLOAT,
	TOWL_BOOL,
	TOWL_ARRAY,
	TOWL_DATETIME,
};

typedef struct {
	struct towl_node *child;	
} towl_table;

typedef struct {
	enum towl_type *type;
	union towl_union *arr;
	size_t len;
	size_t cap;
} towl_array;

union towl_union {
	towl_table *tval;
	char *sval;
	int64_t ival;
	double fval;
	bool bval;
	towl_array *aval;
	struct towl_datetime *dtp;
	char cval;
};

struct towl_datetime {
	long secfrac;
	int sec;
	int min;
	int hour;
	int mday;
	int mon;
	int year;
	long gmtoff; //in seconds
};

enum towl_code towl_parse_string(char *text, towl_table **table);
enum towl_code towl_parse_file(FILE *file, towl_table **table);

enum towl_code towl_write_string(char **text, towl_table *table);
enum towl_code towl_write_file(FILE *text, towl_table *table);

//NEW
towl_table *towl_new_table();

towl_array *towl_new_array();

//ADD TODO write test
enum towl_code towl_add_table(towl_table *parent, char *key, towl_table *new);
enum towl_code towl_add_string(towl_table *parent, char *key, char *new);
enum towl_code towl_add_int(towl_table *parent, char *key, int64_t new);
enum towl_code towl_add_float(towl_table *parent, char *key, double new);
enum towl_code towl_add_bool(towl_table *parent, char *key, bool new);
enum towl_code towl_add_array(towl_table *parent, char *key, towl_array *new);
enum towl_code towl_add_datetime(towl_table *parent, char *key,
                                 struct towl_datetime new);

//GET TODO write test
enum towl_code towl_get_table(towl_table *parent, char *key, 
                              towl_table **varp);
enum towl_code towl_get_string(towl_table *parent, char *key, char **varp);
enum towl_code towl_get_int(towl_table *parent, char *key, int64_t *varp);
enum towl_code towl_get_float(towl_table *parent, char *key, double *varp);
enum towl_code towl_get_bool(towl_table *parent, char *key, bool *varp);
enum towl_code towl_get_array(towl_table *parent, char *key, 
                              towl_array **varp);
enum towl_code towl_get_datetime(towl_table *parent, char *key,
                                 struct towl_datetime *new);

//SET TODO write test
enum towl_code towl_set_table(towl_table *parent, char *key, towl_table *new);
enum towl_code towl_set_string(towl_table *parent, char *key, char *new);
enum towl_code towl_set_int(towl_table *parent, char *key, int64_t new);
enum towl_code towl_set_float(towl_table *parent, char *key, double new);
enum towl_code towl_set_bool(towl_table *parent, char *key, bool new);
enum towl_code towl_set_array(towl_table *parent, char *key, towl_array *new);
enum towl_code towl_set_datetime(towl_table *parent, char *key,
                                 struct towl_datetime new);

//REMOVE TODO write test
enum towl_code towl_remove(towl_table *parent, char *key);

//ARRAY_ADD
enum towl_code towl_array_add_table(towl_array *array, towl_table *val);
enum towl_code towl_array_add_string(towl_array *array, char *val);
enum towl_code towl_array_add_int(towl_array *array, int64_t val);
enum towl_code towl_array_add_float(towl_array *array, double val);
enum towl_code towl_array_add_bool(towl_array *array, bool val);
enum towl_code towl_array_add_array(towl_array *array, towl_array *val);
enum towl_code towl_array_add_datetime(towl_array *array, 
                                       struct towl_datetime val);

//ARRAY_GET
enum towl_code towl_array_get_table(towl_array *array, size_t index,
                                    towl_table **val);
enum towl_code towl_array_get_string(towl_array *array, size_t index, 
                                     const char **val);
enum towl_code towl_array_get_int(towl_array *array, size_t index, int64_t *val);
enum towl_code towl_array_get_float(towl_array *array, size_t index, 
                                    double *val);
enum towl_code towl_array_get_bool(towl_array *array, size_t index, bool *val);
enum towl_code towl_array_get_array(towl_array *array, size_t index,
                                    towl_array **val);
enum towl_code towl_array_get_datetime(towl_array *array, size_t index,
                                       struct towl_datetime *val);

//ARRAY_SET
enum towl_code towl_array_set_table(towl_array *array, size_t index,
                                    towl_table *val);
enum towl_code towl_array_set_string(towl_array *array, size_t index,
                                     char *val);
enum towl_code towl_array_set_int(towl_array *array, size_t index,
                                  int64_t val);
enum towl_code towl_array_set_float(towl_array *array, size_t index,
                                    double val);
enum towl_code towl_array_set_bool(towl_array *array, size_t index, bool val);
enum towl_code towl_array_set_array(towl_array *array, size_t index,
                                    towl_array *val);
enum towl_code towl_array_set_datetime(towl_array *array, size_t index,
                                       struct towl_datetime val);
//ARRAY_LEN
size_t towl_array_len(towl_array* array);

void towl_clean_table(towl_table *table);
void towl_clean_array(towl_array *array);

#ifdef TOWL_TEST
void print_table(towl_table *table);
void print_array(towl_array *array);
void print_datetime(struct towl_datetime);
#endif

#endif
