#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "tester.h"

void test_static_barekey() {
	enum towl_code code;
	
	struct towl_parser parser;
	parser.pos = malloc(PARSER_POS_BUFF_SIZE);
	strcpy(parser.pos, "hello-world.asdf");

	parser.mark = parser.pos;
	parser.line = 0;

	char *str;
	code = static_barekey(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "hello-world");
	free(str);

	strcpy(parser.pos, "world= ");
	code = static_barekey(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "world");
	free(str);

	strcpy(parser.pos, "TABLE0123456789]");
	code = static_barekey(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "TABLE0123456789");
	free(str);

	strcpy(parser.pos, "linux      =");
	code = static_barekey(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "linux");
	free(str);

	strcpy(parser.pos, "*_* =");
	code = static_barekey(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_ILLEGAL_BAREKEY_CHAR);

	strcpy(parser.pos, " =");
	code = static_barekey(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_EMPTY_BAREKEY);
}

static int check_nested_tables(towl_table *parent, towl_table *deepest,
                               char **keys, size_t len) {
	enum towl_code code;
	towl_table *child;

	for(size_t i = 0; i < len; i++) {
		code = static_singlekey_get(parent, keys[i], &child, TOWL_TABLE);
		if(code != TOWLC_SUCCESS)
			return code;

		parent = child;
	}

	if(parent != deepest)
		return -1;
	
	return 0;
}

void test_static_key() {
	enum towl_code code;
	
	struct towl_parser parser;
	parser.src = malloc(PARSER_POS_BUFF_SIZE);
	strcpy(parser.src, "    san_ma  . sahim05 . herke-si =");
	
	parser.pos = parser.mark = parser.src;
	parser.line = 0;

	towl_table *table = towl_new_table();
	char *keys[] = {"san_ma", "sahim05"};

	towl_table *deepest;
	const char *leftmost_key, *rightmost_key;
	
	code = static_parse_key(&parser, table, &deepest, &leftmost_key,
	                        &rightmost_key);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(
		check_nested_tables(table, deepest, keys, sizeof(keys)/sizeof(char*)),
		0);
	ASSERT_EQUAL_STRING(leftmost_key, keys[0]);
	ASSERT_EQUAL_STRING(rightmost_key, "herke-si");

	towl_clean_table(table);



	strcpy(parser.src, "sen.\"sadıka\\\"ne . yâr\".olur =");
	parser.pos = parser.mark = parser.src;
	table = towl_new_table();
	keys[0] = "sen";
	keys[1] = "sadıka\"ne . yâr";

	code = static_parse_key(&parser, table, &deepest, &leftmost_key,
	                        &rightmost_key);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(
		check_nested_tables(table, deepest, keys, sizeof(keys)/sizeof(char*)),
		0);
	ASSERT_EQUAL_STRING(leftmost_key, keys[0]);
	ASSERT_EQUAL_STRING(rightmost_key, "olur");

	towl_clean_table(table);



	strcpy(parser.src, "sen.   \'sadıka\\\"ne . yâr\'.  olur    ]");
	parser.pos = parser.mark = parser.src;
	table = towl_new_table();
	keys[0] = "sen";
	keys[1] = "sadıka\\\"ne . yâr";
	
	code = static_parse_key(&parser, table, &deepest, &leftmost_key,
	                        &rightmost_key);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(
		check_nested_tables(table, deepest, keys, sizeof(keys)/sizeof(char*)),
		0);
	ASSERT_EQUAL_STRING(leftmost_key, keys[0]);
	ASSERT_EQUAL_STRING(rightmost_key, "olur");

	towl_clean_table(table);	


	
	strcpy(parser.src, "\"\".\'\'.\" \" = ");
	parser.pos = parser.mark = parser.src;
	table = towl_new_table();
	keys[0] = "";
	keys[1] = "";
	
	code = static_parse_key(&parser, table, &deepest, &leftmost_key,
	                        &rightmost_key);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(
		check_nested_tables(table, deepest, keys, sizeof(keys)/sizeof(char*)),
		0);
	ASSERT_EQUAL_STRING(leftmost_key, keys[0]);		
	ASSERT_EQUAL_STRING(rightmost_key, " ");

	towl_clean_table(table);



	strcpy(parser.src, "hello. world . no = ");
	parser.pos = parser.mark = parser.src;

	table = towl_new_table();
	towl_table *hello_table = towl_new_table();
	static_singlekey_add(table, "hello", hello_table, TOWL_TABLE);
	
	towl_table *world_table = towl_new_table();
	static_singlekey_add(hello_table, "world", world_table, TOWL_TABLE);

	keys[0] = "hello";
	keys[1] = "world";

	code = static_parse_key(&parser, table, &deepest, &leftmost_key,
	                        &rightmost_key);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_INT(
		check_nested_tables(table, deepest, keys, sizeof(keys)/sizeof(char*)),
		0);
	ASSERT_EQUAL_STRING(leftmost_key, keys[0]);
	ASSERT_EQUAL_STRING(rightmost_key, "no");
	ASSERT_EQUAL_POINTER(world_table, deepest);
	
	towl_clean_table(table);

}
