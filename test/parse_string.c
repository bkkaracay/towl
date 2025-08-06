#include <stdlib.h>
#include <string.h>

#include "tester.h"
#include "test.h"

//"hello"

void test_static_string() {
	enum towl_code code;

	struct towl_parser parser;
	parser.pos = malloc(PARSER_POS_BUFF_SIZE);
	strcpy(parser.pos,
		"\\\"\\U0000FF33anma \\u015Fahım herk\\u0065si sen "
		"s\\u15E9dıkane yar olur\\\""
		"\\b\\t\\n\\f\\r\\\\\""
	);

	parser.mark = parser.pos;
	parser.line = 0;

	char *str;
	code  = static_string_dquote(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str,
		"\"Ｓanma şahım herkesi sen sᗩdıkane yar olur\"\b\t\n\f\r\\");
	free(str);

	strcpy(parser.pos, "Herkesi sen\\");
	parser.mark = parser.pos;
	code  = static_string_dquote(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_ESCAPE_UNEXPECTED_EOF);
	
	strcpy(parser.pos, "dost mu sandın\\w\"");
	parser.mark = parser.pos;
	code  = static_string_dquote(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_UNKNOWN_ESCAPE_SEQ);
	
	strcpy(parser.pos, "belki ol");
	parser.mark = parser.pos;
	code  = static_string_dquote(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_UNTERMINATED_STRING);


	strcpy(parser.pos, 
		"\\\n      a\\U0000011Fyar\\b\\tol\\u0075r\\n\\f\\r\\\\ \\\n"
		"    \\U0000FF33ad\\u0131k\\u15E9ne\"\"\"\""
	);
	parser.mark = parser.pos;
	code  = static_string_dquote_multi(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "ağyar\b\tolur\n\f\r\\ Ｓadıkᗩne\"");
	free(str);
	
	strcpy(parser.pos, "belki ol\"\"\"\"\"");
	parser.mark = parser.pos;
	code  = static_string_dquote_multi(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "belki ol\"\"");
	
	strcpy(parser.pos, "alemde\\");
	parser.mark = parser.pos;
	code  = static_string_dquote_multi(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_ESCAPE_UNEXPECTED_EOF);
	
	strcpy(parser.pos, "serdar olur\\w\"\"\"");
	parser.mark = parser.pos;
	code  = static_string_dquote_multi(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_UNKNOWN_ESCAPE_SEQ);
	
	strcpy(parser.pos, "Yar olur");
	parser.mark = parser.pos;
	code  = static_string_dquote_multi(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_UNTERMINATED_STRING);

	
	strcpy(parser.pos, "a\\ğyar\\tolur'");
	parser.mark = parser.pos;
	code  = static_string_squote(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "a\\ğyar\\tolur");
	free(str);
	
	strcpy(parser.pos, "Sanma şahım\\");
	parser.mark = parser.pos;
	code  = static_string_squote(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_UNTERMINATED_STRING);	

	strcpy(parser.pos, "\nh\\erkesi \\\n\\tsen'''");
	parser.mark = parser.pos;
	code  = static_string_squote_multi(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_SUCCESS);
	ASSERT_EQUAL_STRING(str, "h\\erkesi \\\n\\tsen");
	free(str);
	
	strcpy(parser.pos, "sadıkane\\");
	parser.mark = parser.pos;
	code  = static_string_squote_multi(&parser, &str);
	ASSERT_EQUAL_INT(code, TOWLC_UNTERMINATED_STRING);
}
