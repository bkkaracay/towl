#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "towl.h"

//TODO Find better place for internal function definitions
//FUNC DEFS
static struct towl_parser *new_parser(char*);
static enum towl_code parse_key(struct towl_parser*, towl_table*, towl_table**,
                                const char **, const char **);

//DATA STRUCTURES AND HELPERS
struct towl_node {
	enum towl_type type;
	char* key;
	struct towl_node *next;
};

struct towl_table_node {
	struct towl_node element;
	towl_table *table;
};

struct towl_string_node {
	struct towl_node element;
	char *value;
};

struct towl_int_node {
	struct towl_node element;
	int64_t value;
};

struct towl_float_node {
	struct towl_node element;
	double value;
};

struct towl_bool_node {
	struct towl_node element;
	bool value;
};

struct towl_array_node {
	struct towl_node element;
	towl_array *array;
};

struct towl_datetime_node {
	struct towl_node element;
	struct towl_datetime value;
};

typedef struct {
	char *arr;
	size_t len;
	size_t cap;
} char_array;
/*
static size_t sizeof_type(enum towl_type type) {
	switch(type) {
		case TOWL_TABLE:
			return sizeof(towl_table);
		case TOWL_STRING:
			return sizeof(char*);
		case TOWL_INT:
			return sizeof(int64_t);
		case TOWL_FLOAT:
			return sizeof(double);
		case TOWL_BOOL:
			return sizeof(bool);
		case TOWL_ARRAY:
			return sizeof(towl_array);
		case TOWL_DATETIME:
			return sizeof(struct towl_datetime);
		case TOWL_CHAR:
			return sizeof(char);
	}

	return 0;
}*/

static char_array *new_char_array() {
	char_array *array = malloc(sizeof(char_array));
	if(array == NULL)
		return NULL;

	array->arr = NULL;
	array->len = 0;
	array->cap = 0;
}

static void char_array_add(char_array *array, char c) {
	if(array->cap == array->len) {	
		if(array->cap == 0)
			array->cap = 4;
		else
			array->cap *= 2;

		array->arr = realloc(array->arr, array->cap * sizeof(char));
	}

	array->arr[array->len] = c;

	array->len++;
}

static void clean_char_array(char_array *array) {
	free(array->arr);
	free(array);
}

static struct towl_node *append_node(towl_table *parent, const char *key, 
                                     enum towl_type type, size_t size) {
	struct towl_node *node = malloc(size);
	node->key = strdup(key);
	node->type = type;
	
	node->next = parent->child;
	parent->child = node;

	return node;
} 

static struct towl_node *find_node(towl_table *parent, const char *key) {
	struct towl_node *iter = parent->child;
		
	while(iter != NULL) {
		if(strcmp(iter->key, key) == 0)
			return iter;
		
		iter = iter->next;
	}

	return NULL;
}

static void clear_table(towl_table*);
static void clear_array(towl_array*);

static void free_node(struct towl_node *node) {
	free(node->key);

	switch(node->type) {
		case TOWL_TABLE:
			clear_table(((struct towl_table_node*) node)->table);
			break;
		case TOWL_STRING:
			free(((struct towl_string_node*) node)->value);
			break;
		case TOWL_ARRAY:
			clear_array(((struct towl_array_node*) node)->array);
			break;
	}

	free(node);
}

static void clear_table(towl_table *table) {
	struct towl_node *iter = table->child;
	while(iter != NULL) {
		struct towl_node *tmp = iter->next;
		free_node(iter);
		iter = tmp;				
	}
}

static void clear_array(towl_array *tarr) {
	for(size_t i = 0; i < towl_array_len(tarr); i++) {
		switch(tarr->type[i]) {
			case TOWL_TABLE:
				clear_table((tarr->arr)[i].tval);
				break;
			case TOWL_STRING:	
				free((tarr->arr)[i].sval);
				break;
			case TOWL_ARRAY:
				clear_array((tarr->arr)[i].aval);
				break;
		}
	}

	free(tarr->arr);
	tarr->type = NULL;
	tarr->arr = NULL;
	tarr->len = 0;
	tarr->cap = 0;
}


void towl_clean_table(towl_table *table) {
	clear_table(table);
	free(table);
}

void towl_clean_array(towl_array *array) {
	clear_array(array);
	free(array);
}

towl_table *towl_new_table() {
	towl_table *tbl = malloc(sizeof(towl_table));
	if(tbl == NULL)
		return NULL;
	tbl->child = NULL;

	return tbl;
}

towl_array *towl_new_array() {
	towl_array *tarr = malloc(sizeof(towl_array));
	if(tarr == NULL)
		return NULL;
	
	tarr->type = NULL;
	tarr->arr = NULL;
	tarr->len = 0;
	tarr->cap = 0;

	return tarr;
}

static enum towl_code singlekey_add(towl_table *parent, const char *key,
                                    void *value, enum towl_type type) {
	if(key == NULL)
		return TOWLC_NULL_KEY;

	//TODO Check for null value (Do for other functions too)

	if(find_node(parent, key))
		return TOWLC_KEY_ALREADY_EXISTS;

	struct towl_node *node;

	switch(type) {
		case TOWL_TABLE:
			node = append_node(parent, key, TOWL_TABLE,
		                       sizeof(struct towl_table_node));
			((struct towl_table_node*) node)->table = (towl_table*) value;
			break;
		case TOWL_STRING:
			node = append_node(parent, key, TOWL_STRING, 
			                   sizeof(struct towl_string_node));
			((struct towl_string_node*) node)->value = strdup((char*) value);
			break;
		case TOWL_INT:
			node = append_node(parent, key, TOWL_INT,
			                   sizeof(struct towl_int_node));
			((struct towl_int_node*) node)->value = *((int64_t*) value);
			break;
		case TOWL_FLOAT:
			node = append_node(parent, key, TOWL_FLOAT,
			                   sizeof(struct towl_float_node));
			((struct towl_float_node*) node)->value = *((double*) value);
			break;
		case TOWL_BOOL:
			node = append_node(parent, key, TOWL_BOOL,
			                   sizeof(struct towl_bool_node));
			((struct towl_bool_node*) node)->value = *((bool*) value);
			break;
		case TOWL_ARRAY:
			node = append_node(parent, key, TOWL_ARRAY,
			                   sizeof(struct towl_array_node));
			((struct towl_array_node*) node)->array = (towl_array*) value;
			break;
		case TOWL_DATETIME:
			node = append_node(parent, key, TOWL_DATETIME,
			                   sizeof(struct towl_datetime_node));
			((struct towl_datetime_node*) node)->value =
				*((struct towl_datetime*) value);
			break;
	}
	
	return TOWLC_SUCCESS;
}

static enum towl_code singlekey_get(towl_table *parent, const char *key,
                                    void *p, enum towl_type type) {
	if(key == NULL)
			return TOWLC_NULL_KEY;
	
	struct towl_node *node = find_node(parent, key);
	if(node == NULL)
		return TOWLC_KEY_NOT_FOUND;

	if(node->type != type)
		return TOWLC_TYPE_MISMATCH;

	switch(type) {
		case TOWL_TABLE:
			*((towl_table**) p) = ((struct towl_table_node*) node)->table;
			break;
		case TOWL_STRING:
			*((char**) p) = ((struct towl_string_node*) node)->value;
			break;
		case TOWL_INT:
			*((int64_t*) p) = ((struct towl_int_node*) node)->value;
			break;
		case TOWL_FLOAT:
			*((double*) p) = ((struct towl_float_node*) node)->value;
			break;
		case TOWL_BOOL:
			*((bool*) p) = ((struct towl_bool_node*) node)->value;
			break;
		case TOWL_ARRAY:
			*((towl_array**) p) = ((struct towl_array_node*) node)->array;
			break;
		case TOWL_DATETIME:
			*((struct towl_datetime*) p) =
				((struct towl_datetime_node*) node)->value;
			break;
	}

	return TOWLC_SUCCESS;
}

static enum towl_code singlekey_set(towl_table *parent, const char *key,
                                    void *value, enum towl_type type) {
	if(key == NULL)
		return TOWLC_NULL_KEY;

	struct towl_node *node = find_node(parent, key);
	if(node == NULL)
		return TOWLC_KEY_NOT_FOUND;

	if(node->type != type)
		return TOWLC_TYPE_MISMATCH;

	switch(type) {
		case TOWL_TABLE:
			clear_table(((struct towl_table_node*) node)->table);
			((struct towl_table_node*) node)->table = (towl_table*) value;
			break;
		case TOWL_STRING:
			free(((struct towl_string_node*) node)->value);
			((struct towl_string_node*) node)->value = strdup((char*) value);
			break;
		case TOWL_INT:
			((struct towl_int_node*) node)->value = *((int64_t*) value);
			break;
		case TOWL_FLOAT:
			((struct towl_float_node*) node)->value = *((double*) value);
			break;
		case TOWL_BOOL:
			((struct towl_bool_node*) node)->value = *((bool*) value);
			break;
		case TOWL_ARRAY:
			clear_array(((struct towl_array_node*) node)->array);
			((struct towl_array_node*) node)->array = (towl_array*) value;
			break;
		case TOWL_DATETIME:
			((struct towl_datetime_node*) node)->value = 
				*((struct towl_datetime*) value);
			break;
	}

	return TOWLC_SUCCESS;	
}

static enum towl_code singlekey_remove(towl_table *parent, const char *key) {
	if(key == NULL)
		return TOWLC_NULL_KEY;

	//Get the pointer of the node
	struct towl_node **current = &parent->child;
	
	while((*current) != NULL) {
		if(strcmp((**current).key, key))
			break;

		current = &((**current).next);
	}

	if((*current) == NULL)
		return TOWLC_KEY_NOT_FOUND;

	//Remove the node from chain and free the node.
	struct towl_node *next_of_current = (**current).next;
	free_node(*current);
	
	(*current) = next_of_current;
	return TOWLC_SUCCESS;
}

enum functype {
	ADD,
	GET,
	SET
};

//Supports dotted keys
static enum towl_code add_get_set(towl_table *parent, char *key, void *value,
                                  enum towl_type type, enum functype ftype) {
	enum towl_code code;
	
	struct towl_parser *parser = new_parser(key);
	towl_table *deepest;
	const char *leftmost_key = NULL, *rightmost_key = NULL;
	
	code = parse_key(parser, parent,  &deepest, &leftmost_key, &rightmost_key);
	if(code != TOWLC_SUCCESS) {
		free(parser);
		return code;
	}

	switch(ftype) {
		case ADD:
			code = singlekey_add(deepest, rightmost_key, value, type);
			break;
		case GET:
			code = singlekey_get(deepest, rightmost_key, value, type);
			break;
		default:
			code = singlekey_set(deepest, rightmost_key, value, type);
	}
	
	if(code != TOWLC_SUCCESS) {
		free(parser);
		singlekey_remove(parent, leftmost_key);
		return code;
	}
	
	free(parser);
	return TOWLC_SUCCESS;
}

static enum towl_code add(towl_table *parent, char *key, void *value,
                          enum towl_type type) {
	return add_get_set(parent, key, value, type, ADD);
}

enum towl_code towl_add_table(towl_table *parent, char *key, towl_table *new) {
	return add(parent, key, new, TOWL_TABLE);
}

enum towl_code towl_add_string(towl_table *parent, char *key, char *new) {
	return add(parent, key, new, TOWL_STRING);
}

enum towl_code towl_add_int(towl_table *parent, char *key, int64_t new) {
	return add(parent, key, &new, TOWL_INT);
}

enum towl_code towl_add_float(towl_table *parent, char *key, double new) {
	return add(parent, key, &new, TOWL_FLOAT);
}

enum towl_code towl_add_bool(towl_table *parent, char *key, bool new) {
	return add(parent, key, &new, TOWL_BOOL);
}

enum towl_code towl_add_array(towl_table *parent, char *key, towl_array *new) {
	return add(parent, key, new, TOWL_ARRAY);
}

enum towl_code towl_add_datetime(towl_table *parent, char *key, 
                                 struct towl_datetime new) {
	return add(parent, key, &new, TOWL_DATETIME);
}


static enum towl_code get(towl_table *parent, char *key, void *value,
                          enum towl_type type) {
	return add_get_set(parent, key, value, type, GET);
}

enum towl_code towl_get_table(towl_table *parent, char *key, towl_table **varp) {
	return get(parent, key, varp, TOWL_TABLE);
}

enum towl_code towl_get_string(towl_table *parent, char *key, char **varp) {
	return get(parent, key, varp, TOWL_STRING);
}

enum towl_code towl_get_int(towl_table *parent, char *key, int64_t *varp) {
	return get(parent, key, varp, TOWL_INT);
}

enum towl_code towl_get_float(towl_table *parent, char *key, double *varp) {
	return get(parent, key, varp, TOWL_FLOAT);
}

enum towl_code towl_get_bool(towl_table *parent, char *key, bool *varp) {
	return get(parent, key, varp, TOWL_BOOL);
}

enum towl_code towl_get_array(towl_table *parent, char *key, towl_array **varp) {
	return get(parent, key, varp, TOWL_ARRAY);
}

enum towl_code towl_get_datetime(towl_table *parent, char *key,
                                 struct towl_datetime *varp) {
	return get(parent, key, varp, TOWL_DATETIME);
}


static enum towl_code set(towl_table *parent, char *key, void *value,
                          enum towl_type type) {
	return add_get_set(parent, key, value, type, SET);
}

enum towl_code towl_set_table(towl_table *parent, char *key, towl_table *new) {
	return set(parent, key, new, TOWL_TABLE);
}

enum towl_code towl_set_string(towl_table *parent, char *key, char *new) {
	return set(parent, key, new, TOWL_STRING);
}

enum towl_code towl_set_int(towl_table *parent, char *key, int64_t new) {
	return set(parent, key, &new, TOWL_INT);
}

enum towl_code towl_set_float(towl_table *parent, char *key, double new) {
	return set(parent, key, &new, TOWL_FLOAT);
}

enum towl_code towl_set_bool(towl_table *parent, char *key, bool new) {
	return set(parent, key, &new, TOWL_BOOL);
}

enum towl_code towl_set_array(towl_table *parent, char *key, towl_array *new) {
	return set(parent, key, new, TOWL_ARRAY);
}

enum towl_code towl_set_datetime(towl_table *parent, char *key,
                                 struct towl_datetime new) {
	return set(parent, key, &new, TOWL_DATETIME);
}


enum towl_code towl_remove(towl_table *parent, char *key) {
	enum towl_code code;
		
	struct towl_parser *parser = new_parser(key);
	towl_table *deepest;
	const char *leftmost_key = NULL, *rightmost_key = NULL;
	
	code = parse_key(parser, parent,  &deepest, &leftmost_key, &rightmost_key);
	if(code != TOWLC_SUCCESS) {
		free(parser);
		return code;
	}

	code = singlekey_remove(deepest, rightmost_key);
	
	if(code != TOWLC_SUCCESS) {
		free(parser);
		code = singlekey_remove(parent, leftmost_key);
		if(code != TOWLC_SUCCESS)
			return code;
		return code;
	}
	
	free(parser);
	return TOWLC_SUCCESS;
}

//PARSER
struct towl_parser {
	char *src;
	char *pos;
	char *mark;
	int line;
	towl_table *global;
};

static struct towl_parser *new_parser(char *src) {
	if(src == NULL)
		return NULL;

	struct towl_parser *parser = malloc(sizeof(struct towl_parser));
	if(parser == NULL)
		return NULL;
	
	parser->src = parser->pos = parser->mark = src;
	parser->line = 0;

	return parser;
}

static void change_parser_src(struct towl_parser *parser, char *src) {
	parser->src = parser->pos = parser->mark = src;
}

//Only use if the key is heap allocated
static void clean_parser(struct towl_parser *parser) {
	if(parser == NULL)
		return;

	towl_clean_table(parser->global);
	free(parser->src);
	free(parser);
}

static char advance(struct towl_parser *parser) {
	return *(parser->pos++);
}

static void advance_by(struct towl_parser *parser, size_t count) {
	parser->pos += count;
}

static char peek(struct towl_parser *parser, int n) {
	return *(parser->pos + n);
}

static bool match(struct towl_parser *parser, char expected) {
	if(*parser->pos != expected)
		return false;

	parser->pos++;
	return true;
}

static bool match_string(struct towl_parser *parser, char *expected) {
	int i = 0;
	for(; expected[i] != '\0'; i++) {
		if(parser->pos[i] == '\0')
			return false;

		if(parser->pos[i] != expected[i])
			return false;
	}
	
	parser->pos += i;
	return true;
}

static void skip_whitespace(struct towl_parser *parser) {
	while(*parser->pos == ' ')
		parser->pos++;
}

static char *pos(struct towl_parser *parser) {
	return parser->pos;
}

static char *mark(struct towl_parser *parser) {
	return (parser->mark = parser->pos);
}

static char *reset_to_mark(struct towl_parser *parser) {
	return (parser->pos = parser->mark);
}

static size_t token_len(struct towl_parser *parser) {
	return parser->pos - parser->mark;
}

static int8_t hexchar_to_dec(char c) {
	if(48 <= c && c <= 57) //0,1,...,9
		return c - 48;

	if(65 <= c && c <= 70) //A,B,C,D,E,F
		return c - 55;

	if(97 <= c && c <= 102) //a,b,c,d,e,f
		return -2;

	return -1;
}

static enum towl_code hexstr_to_uint32(const char *str, int len,
                                       uint32_t *out) {
	*out = 0;
	for(int i = 0; i < len; i++) {
		int8_t num = hexchar_to_dec(str[i]);
		if(num < 0)
			return TOWLC_ILLEGAL_HEX_CHAR;
		
		*out += num * (1 << 4*(len - i - 1));
	}

	return TOWLC_SUCCESS;
}

static int uint32_to_utf8(uint32_t n, char utf8[5]) {
	if(n < 0x80) { //1 Byte (7 usable bits)
		utf8[0] = n;
		utf8[1] = '\0';
		return 1;
	} 

	if(n < 0x800) { //2 Byte (11 usable bits)
		utf8[0] = 0xC0 | (n >> 6);
		utf8[1] = 0x80 | (n & 0x3F);
		utf8[2] = '\0';
		return 2;
	} 

	if(n >= 0xD800 && n <= 0xDFFF) //UTF16 surrogates
		return 0;
	
	if(n < 0x10000) { //3 Byte (16 usable bits)
		utf8[0] = 0xE0 | (n >> 12);
		utf8[1] = 0x80 | ((n >> 6) & 0x3F);
		utf8[2] = 0x80 | (n & 0x3F);
		utf8[3] = '\0';
		return 3;
	} 

	if(n < 0x110000) { //4 Byte (21 usable bits but standard have limits)
		utf8[0] = 0xF0 | (n >> 18);
		utf8[1] = 0x80 | ((n >> 12) & 0x3F);
		utf8[2] = 0x80 | ((n >> 6) & 0x3F);
		utf8[3] = 0x80 | (n & 0x3F);
		utf8[4] = '\0';
		return 4;
	} 
	
	return 0;
}

static enum towl_code hexstr_to_utf8(const char *str, int len, char utf8[5]) {
	enum towl_code code;

	uint32_t decimal;
	code = hexstr_to_uint32(str, len, &decimal);
	if(code != TOWLC_SUCCESS)
		return code;

	int result = uint32_to_utf8(decimal, utf8);
	if(result == 0)
		return TOWLC_ILLEGAL_UNICODE;

	return TOWLC_SUCCESS;		
}

enum towl_code towl_array_add_char(towl_array *array, char val);

static enum towl_code string_handle_escape(struct towl_parser *parser,
                                           char_array *array) {
	enum towl_code code = TOWLC_SUCCESS;
	if(match(parser, 'b'))
		char_array_add(array, '\b');
		
	else if(match(parser, 't'))
		char_array_add(array, '\t');
		
	else if(match(parser, 'n'))
		char_array_add(array, '\n');
		
	else if(match(parser, 'f'))
		char_array_add(array, '\f');
		
	else if(match(parser, 'r'))
		char_array_add(array, '\r');
		
	else if(match(parser, '\"'))
		char_array_add(array, '\"');
		
	else if(match(parser, '\\'))
		char_array_add(array, '\\');
				
	else if(match(parser, 'u') || match(parser, 'U')) {
		size_t hexlen = (peek(parser, -1) == 'u') ? 4 : 8;
		
		char utf8[5];
		code = hexstr_to_utf8(parser->pos, hexlen, utf8);
		if(code != TOWLC_SUCCESS)
			return code;

		for(int i = 0; utf8[i] != '\0'; i++)
			char_array_add(array, utf8[i]);
		
		advance_by(parser, hexlen);
	} else if(match(parser, '\0')) {
		return TOWLC_ESCAPE_UNEXPECTED_EOF;
	} else {
		return TOWLC_UNKNOWN_ESCAPE_SEQ;
	}

	return code;
}

static enum towl_code string_dquote(struct towl_parser *parser, char **str) {
	enum towl_code code;
	char_array *array = new_char_array();
	*str = NULL;

	while(true) {
		if(match(parser, '\\')) {			
			code = string_handle_escape(parser, array);
			if(code != TOWLC_SUCCESS) {
				clean_char_array(array);
				return code;
			}
		} else {
			if(peek(parser, 0) == '\0') {
				clean_char_array(array);		
				return TOWLC_UNTERMINATED_STRING;
			}
		
			char current = advance(parser);
			
			if(current == '"')
				break;

			if(current == '\n') {	
				clean_char_array(array);
				return TOWLC_UNEXPECTED_NEWLINE;
			}
			
			char_array_add(array, current);
		}
	}

	char_array_add(array, '\0');
	*str = array->arr;
	free(array);
	
	return TOWLC_SUCCESS;
}

//double quote, multiline
static enum towl_code string_dquote_multi(struct towl_parser *parser,
                                          char **str) {
	enum towl_code code;
	char_array *array = new_char_array();
	*str = NULL;

	if(peek(parser, 0) == '\n') //Handle first newline after three quotes
		advance(parser);

	while(true) {
		if(match(parser, '\\')) {	
			if(match(parser, '\n'))
				skip_whitespace(parser);
			else {
				code = string_handle_escape(parser, array);
				if(code != TOWLC_SUCCESS) {
					clean_char_array(array);
					return code;
				}
			}
		} else {
			if(peek(parser, 0) == '\0') {
				clean_char_array(array);		
				return TOWLC_UNTERMINATED_STRING;
			}
		
			char current = advance(parser);
		
			if(current == '"') {
				if(match_string(parser, "\"\"")) {
					//Standard accepts strings closed with 5 or 4 quotes
					//""""hello"""" parsed as "hello"
					if(match(parser, '"')) {
						char_array_add(array, '"');

						if(match(parser, '"')) 
							char_array_add(array, '"');
					}
				}
				
				break;
			}
			
			if(current == '\n')
				parser->line++;
			
			char_array_add(array, current);
		}
	}

	char_array_add(array, '\0');
	*str = array->arr;
	free(array);
	
	return TOWLC_SUCCESS;
}

static enum towl_code string_squote(struct towl_parser *parser, char **str) {
	char *start = mark(parser);
	enum towl_code code;
	*str = NULL;
	
	while(true) {
		if(peek(parser, 0) == '\0')	
			return TOWLC_UNTERMINATED_STRING;
	
		char current = advance(parser);
		
		if(current == '\'')
			break;

		if(current == '\n')
			return TOWLC_UNEXPECTED_NEWLINE;
	}

	size_t len = token_len(parser) - 1;
	*str = malloc(sizeof(char) * (len + 1));
	
	memcpy(*str, start, sizeof(char) * len);
	(*str)[len] = '\0';
	
	return TOWLC_SUCCESS;
}

//single quote, multiline
static enum towl_code string_squote_multi(struct towl_parser *parser,
                                          char **str) {
	enum towl_code code;
	*str = NULL;
	
	if(peek(parser, 0) == '\n') //Handle first newline after three quotes
		advance(parser);

	char *start = mark(parser);
	
	while(true) {
		if(peek(parser, 0) == '\0')
			return TOWLC_UNTERMINATED_STRING;
	
		char current = advance(parser);
		if(current == '\'') {
			if(match_string(parser, "''")) {
				//Standard accepts strings closed with 5 or 4 quotes
				//''''hello'''' parsed as 'hello'
				if(match(parser, '\'')) 
					match(parser, '\'');
				
				break;
			}
		}

		if(current == '\n')
			parser->line++;
	}

	size_t len = token_len(parser) - 3;
	*str = malloc(sizeof(char) * (len + 1));

	memcpy(*str, start, sizeof(char) * len);
	(*str)[len] = '\0'; 
	
	return TOWLC_SUCCESS;
}

static bool is_barekey_char(char c) {
	return (65 <= c && c <= 90) //A-Z
		|| (97 <= c && c <= 122) //a-z
		|| (48 <= c && c <= 57)	 //0-9
		|| (c == 45) || (c == 95); //- _
}

static enum towl_code barekey(struct towl_parser *parser, char **str) {
	char *start = mark(parser);
	*str = NULL;

	while(true) {
		char current = peek(parser, 0);

		if(current == '.' || current == '=' ||
		   current == ']' || current == ' ')
		{
			break;
		}

		if(current == '\0')
			return TOWLC_EOL;

		if(!is_barekey_char(current))
			return TOWLC_ILLEGAL_BAREKEY_CHAR;	

		advance(parser);		
	}

	size_t len = token_len(parser);
	
	if(len == 0)
		return TOWLC_EMPTY_BAREKEY;
	
	*str = malloc(sizeof(char) * (len + 1));
	
	memcpy(*str, start, sizeof(char) * len);
	(*str)[len] = '\0';

	return TOWLC_SUCCESS;
}

static enum towl_code parse_key(struct towl_parser *parser, towl_table *parent,
                                towl_table **deepest_table, 
                                const char **leftmost_key,
                                const char **rightmost_key) {
	enum towl_code code;

	char *key_slice;
	towl_table *before = parent;
	*leftmost_key = NULL;
	while(true) {
		skip_whitespace(parser);
		switch(peek(parser, 0)) {
			case '"': //TODO !!! Check for errors when using three quotes
				advance(parser);
				code = string_dquote(parser, &key_slice);
				break;
			case '\'':
				advance(parser);
				code = string_squote(parser, &key_slice);
				break;
			default:
				code = barekey(parser, &key_slice);
		}
		
		if(code != TOWLC_SUCCESS)
			return code;

		if(*leftmost_key == NULL)
			*leftmost_key = key_slice;

		skip_whitespace(parser);
		
		if(peek(parser, 0) != '.')
			break;

		advance(parser);
		
		struct towl_node *node = find_node(before, key_slice);		
		if(node == NULL) {
			towl_table *current = towl_new_table();
			singlekey_add(before, key_slice, current, TOWL_TABLE);
			before = current;
		} else {
			if(node->type != TOWL_TABLE) {
				free(key_slice);
				return TOWLC_REDEFINITION_AS_TABLE;
			}

			before = ((struct towl_table_node*) node)->table;
		}
	}

	*deepest_table = before ;
	*rightmost_key = key_slice;
	
	return TOWLC_SUCCESS;
}

static enum towl_code parse_bool(struct towl_parser *parser, bool *val) {
	if(match_string(parser, "true")) {
		*val = 1;
		return TOWLC_SUCCESS;
	}

	if(match_string(parser, "false")) {
		*val = 0;
		return TOWLC_SUCCESS;
	}

	return TOWLC_INVALID_VALUE;	
}

enum int_base {
	INT_BASE_DEC,
	INT_BASE_HEX,
	INT_BASE_OCT,
	INT_BASE_BIN	
};

static double power(double base, int exp) {
	double out = 1;

	if(exp > 0) {
		for(int i = 0; i < exp; i++)
			out *= base;
	} else {
		for(int i = 0; i > exp; i--)
			out /= base;
	}
	
	return out;
}

static bool is_digit(char c) {
	return '0' <= c && c <= '9';
}

static bool is_hex_digit(char c) {
	return ('0' <= c && c <= '9') || 
	       ('A' <= c && c <= 'F') ||
	       ('a' <= c && c <= 'f');
}

static bool is_oct_digit(char c) {
	return '0' <= c && c <= '7';
}

static bool is_bin_digit(char c) {
	return c == '0' || c == '1';
}

static inline bool is_valid_digit(char c, enum int_base base) {
	switch(base) {
		case INT_BASE_HEX:
			return is_hex_digit(c);
		case INT_BASE_OCT:
			return is_oct_digit(c);
		case INT_BASE_BIN:
			return is_bin_digit(c);
	}

	return is_digit(c);
}

static int base_to_int(enum int_base base) {
	switch(base) {
		case INT_BASE_HEX:
			return 16;
		case INT_BASE_OCT:
			return 8;
		case INT_BASE_BIN:
			return 2;
	}

	return 10;
}

static enum towl_code uint64_to_int64(uint64_t val, int sign, int64_t *varp) {
	if(sign >= 0) {
		if(val > INT64_MAX)
			return TOWLC_NUMBER_OVERFLOW;

		*varp = val;
		return TOWLC_SUCCESS;
	}
	
	if(val > (uint64_t) INT64_MIN)
		return TOWLC_NUMBER_UNDERFLOW;
	
	*varp = val * -1;
	return TOWLC_SUCCESS;
}

static int parse_sign(struct towl_parser *parser) {
	if(match(parser, '-')) 
		return -1;
	if(match(parser, '+'))
		return 1;
	
	return 0;
}

static enum int_base parse_base(struct towl_parser *parser) { 
	if(match_string(parser, "0x"))
		return INT_BASE_HEX;
	
	if(match_string(parser, "0o"))
		return INT_BASE_OCT;

	if(match_string(parser, "0b"))
		return INT_BASE_BIN;
	
	return INT_BASE_DEC; 
}

static enum towl_code digit(struct towl_parser *parser,
                            enum int_base base,
                            uint64_t *varp,
							int *digit_count,
							bool allow_underscore) {
	uint64_t out = 0;
	
	int digit_found = 0;

	char next = peek(parser, 0);
	while(is_valid_digit(next, base) || (allow_underscore && next == '_')) {
		advance(parser);

		if(allow_underscore && next == '_') {
			next = peek(parser, 0);
			continue;
		}
		
		digit_found++;
		out = out * base_to_int(base) + hexchar_to_dec(next);
		next = peek(parser, 0);
	}

	if(digit_found == 0)
		return TOWLC_DIGIT_NOT_FOUND;
	
	*varp = out;
	
	if(digit_count != NULL)
		*digit_count = digit_found;

	return TOWLC_SUCCESS;	
}

static enum towl_code parse_digit_nounderscore(struct towl_parser *parser,
                                               enum int_base base,
								               uint64_t *varp,
											   int *digit_count) {
	return digit(parser, base, varp, digit_count, false);
}

static enum towl_code parse_digit(struct towl_parser *parser,
                                  enum int_base base,
					              uint64_t *varp,
					              int *digit_count) {
	enum towl_code code;
	if(peek(parser, 0) == '_')
		return TOWLC_UNDERSCORE_PLACEMENT;
	
	code = digit(parser, base, varp, digit_count, true);
	if(code != TOWLC_SUCCESS)
		return code;

	if(peek(parser, -1) == '_')
		return TOWLC_UNDERSCORE_PLACEMENT;
	
	return TOWLC_SUCCESS;
}

static enum towl_code parse_n_digits(struct towl_parser *parser, int n,
                                     unsigned int *varp) {
	int out = 0;

	char next = peek(parser, 0);
	for(int i = 0; i < n; i++) {
		if(!is_digit(peek(parser, 0)))
			return TOWLC_NON_DIGIT_CHAR;

		out = out * 10 + hexchar_to_dec(next);
		advance(parser);
		next = peek(parser, 0);
	}

	*varp = out;

	return TOWLC_SUCCESS;
}

static bool has_leading_trailing_zero(int digit_count, char first_last_char) {
	if(digit_count > 1 && first_last_char == '0')
		return 1;

	return 0;
}

static enum towl_code parse_float_rest(struct towl_parser *parser,
                                       int sign,
                                       uint64_t first_part,
									   double *varp) {
	enum towl_code code;
	double out = first_part;
	int exp = 0;

	if(match(parser, '.')) {
		uint64_t fract_part;
		int digit_count;
		code = parse_digit(parser, INT_BASE_DEC, &fract_part, &digit_count);
		if(code != TOWLC_SUCCESS) {
			if(code == TOWLC_DIGIT_NOT_FOUND)
				return TOWLC_FLOAT_EMPTY_FRACT_PART;

			return code;
		}

		if(has_leading_trailing_zero(digit_count, peek(parser, -1)))
			return TOWLC_TRAILING_ZERO_ERR;

		out += fract_part * power(10, -digit_count);
	}

	if(match(parser, 'e') || match(parser, 'E')) {
		int exp_sign = parse_sign(parser);

		uint64_t exp_part;
		code = parse_digit_nounderscore(parser, INT_BASE_DEC, &exp_part, NULL);
		if(code != TOWLC_SUCCESS) {
			if(code == TOWLC_DIGIT_NOT_FOUND)
				return TOWLC_FLOAT_EMPTY_EXP_PART;

			return code;
		}
		
		int64_t signed_exp_part;
		code = uint64_to_int64(exp_part, exp_sign, &signed_exp_part);
		if(code != TOWLC_SUCCESS)
			return code;

		out *= power(10, signed_exp_part);
	}

	if(sign == 0)
		sign = 1;

	if(out == HUGE_VAL) {
		if(sign > 0)
			return TOWLC_FLOAT_OVERFLOW;
		else
			return TOWLC_FLOAT_UNDERFLOW;
	}

	*varp = out * sign;

	return TOWLC_SUCCESS;
}

static struct towl_datetime new_datetime() {
	struct towl_datetime dt = { -1, -1, -1, -1, -1, -1, -1, -1 };
	return dt;
}

static enum towl_code parse_time_rest(struct towl_parser *parser,
                                      uint64_t first_part,
                                      struct towl_datetime *varp) {
	enum towl_code code;
	struct towl_datetime dt = new_datetime();

	if(!match(parser, ':'))
		return TOWLC_TIME_SYNTAX_ERR;

	dt.hour = first_part;
	if(dt.hour > 23)
		return TOWLC_HOUR_OUT_OF_RANGE;
	
	unsigned int min;
	code = parse_n_digits(parser, 2, &min);
	if(code != TOWLC_SUCCESS)
		return code;

	if(min > 59)
		return TOWLC_MIN_OUT_OF_RANGE;


	if(!match(parser, ':'))
		return TOWLC_TIME_SYNTAX_ERR;
	

	unsigned int sec;
	code = parse_n_digits(parser, 2, &sec);
	if(code != TOWLC_SUCCESS)
		return code;
	
	if(sec > 59)
		return TOWLC_SEC_OUT_OF_RANGE;
	
	dt.min = min;
	dt.sec = sec;

	if(match(parser, '.')) {
		uint64_t fract_part;
		code = parse_digit_nounderscore(parser, INT_BASE_DEC, &fract_part,
		                                NULL);
		if(code != TOWLC_SUCCESS)
			return code;
		
		int64_t secfrac;
		code = uint64_to_int64(fract_part, 1, &secfrac);
		if(code != TOWLC_SUCCESS)
			return code;

		dt.secfrac = secfrac;
	}

	*varp = dt;
	return TOWLC_SUCCESS;
}

static enum towl_code parse_date_rest(struct towl_parser *parser,
                                      uint64_t first_part,
								      struct towl_datetime *varp) {
	enum towl_code code;
	struct towl_datetime dt = new_datetime();

	if(!match(parser, '-'))
		return TOWLC_DATE_SYNTAX_ERR;

	dt.year = first_part;
	
	unsigned int mon;
	code = parse_n_digits(parser, 2, &mon);
	if(code != TOWLC_SUCCESS)
		return code;

	if(!match(parser, '-'))
		return TOWLC_DATE_SYNTAX_ERR;

	unsigned int mday;
	code = parse_n_digits(parser, 2, &mday);
	if(code != TOWLC_SUCCESS)
		return code;
	
	dt.mon = mon;
	dt.mday = mday;
	
	if(dt.mday < 1)
		return TOWLC_DAY_OUT_OF_RANGE;

	switch(dt.mon) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if(dt.mday > 31)
				return TOWLC_DAY_OUT_OF_RANGE;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if(dt.mday > 30)
				return TOWLC_DAY_OUT_OF_RANGE;
			break;
		case 2:
			if(dt.mday % 4 == 0) {
				if(dt.mday > 29)
					return TOWLC_DAY_OUT_OF_RANGE;
			} else {
				if(dt.mday > 28)
					return TOWLC_DAY_OUT_OF_RANGE;
			}
			break;
		default:
			return TOWLC_MONTH_OUT_OF_RANGE;
	}

	*varp = dt;
	return TOWLC_SUCCESS;
}

static enum towl_code parse_gmtoff(struct towl_parser *parser, long *varp) {
	if(match(parser, 'Z')) {
		*varp = 0;
		return TOWLC_SUCCESS;
	}

	int sign = parse_sign(parser);
	if(sign == 0) {
		*varp = -1;
		return TOWLC_SUCCESS;
	}

	enum towl_code code;
	
	unsigned int hour;
	code = parse_n_digits(parser, 2, &hour);
	if(code != TOWLC_SUCCESS)
		return code;

	if(!match(parser, ':'))
		return TOWLC_GMTOFF_SYNTAX_ERR;

	unsigned int min;
	code = parse_n_digits(parser, 2, &min);
	if(code != TOWLC_SUCCESS)
		return code;
	
	*varp = (hour * 3600 + min * 60) * sign;
	return TOWLC_SUCCESS;
}

static enum towl_code parse_datetime_rest(struct towl_parser *parser,
                                          uint64_t first_part,
                                          struct towl_datetime *varp) {
	enum towl_code code;
	struct towl_datetime dt = new_datetime();

	struct towl_datetime date;
	code = parse_date_rest(parser, first_part, &date);
	if(code != TOWLC_SUCCESS)
		return code;
	
	dt.year = date.year;
	dt.mon = date.mon;
	dt.mday = date.mday;
	
	if(match(parser, 'T') || match(parser, ' ')) {
		unsigned int u64_hour;
		code = parse_n_digits(parser, 2, &u64_hour);
		if(code != TOWLC_SUCCESS)
			return code;

		struct towl_datetime time;
		code = parse_time_rest(parser, u64_hour, &time);
		if(code != TOWLC_SUCCESS)
			return code;
		
		dt.hour = time.hour;
		dt.min = time.min;
		dt.sec = time.sec;
		dt.secfrac = time.secfrac;

		code = parse_gmtoff(parser, &dt.gmtoff);
		if(code != TOWLC_SUCCESS)
			return code;
	}
	*varp = dt;
	return TOWLC_SUCCESS;
}
		

static enum towl_type estimate_type(struct towl_parser *parser,
                                    enum int_base base,
                                    int digit_count,
									int sign,
									bool has_underscore) {
	bool float_possible = 1, datetime_possible = 1;

	if(base != INT_BASE_DEC) {
		float_possible = 0;
		datetime_possible = 0;
	}

	if(digit_count != 2 && digit_count != 4)
		datetime_possible = 0;

	if(sign != 0)
		datetime_possible = 0;

	if(has_underscore)
		datetime_possible = 0;

	char next = peek(parser, 0);
	
	if(float_possible && (next == '.' || next == 'e' || next == 'E'))
		return TOWL_FLOAT;

	if(datetime_possible && (next == '-' || next == ':'))
		return TOWL_DATETIME;

	return TOWL_INT;
}
	
static enum towl_code handle_int(enum int_base base,
                                 bool leading_zero,
                                 int sign,
								 uint64_t val,
								 int64_t *varp) {
	if(base == INT_BASE_DEC) {
		if(leading_zero)
			return TOWLC_LEADING_ZERO_ERR;

		return uint64_to_int64(val, sign, varp);
	}

	if(sign != 0)
		return TOWLC_SIGN_IN_RADIX_INT;

	return uint64_to_int64(val, sign, varp);
}

static enum towl_code parse_numeric(struct towl_parser *parser, void *varp,
                                    enum towl_type *type) {
	int sign = parse_sign(parser);
	enum int_base base = parse_base(parser);

	enum towl_code code;
	
	mark(parser);

	char first_digit = peek(parser, 0);

	int digit_count;
	uint64_t first_part;
	code = parse_digit(parser, base, &first_part, &digit_count);
	if(code != TOWLC_SUCCESS)
		return code;

	bool has_leading_zero = has_leading_trailing_zero(digit_count, first_digit);
	bool has_underscore = (digit_count != token_len(parser));

	*type = estimate_type(parser, base, digit_count, sign, has_underscore);
	switch(*type) {
		case TOWL_FLOAT:
			if(has_leading_zero)
				return TOWLC_LEADING_ZERO_ERR;

			return parse_float_rest(parser, sign, first_part, varp);
		case TOWL_DATETIME:
			if(peek(parser, 0) == '-')
				return parse_datetime_rest(parser, first_part, varp);
			else
				return parse_time_rest(parser, first_part, varp);
	}
			
	return handle_int(base, has_leading_zero, sign, first_part, varp);
}

static enum towl_code parse_inf_nan(struct towl_parser *parser, double *varp) {
	int sign = parse_sign(parser);
	if(sign == 0)
		sign = 1;
	
	if(match_string(parser, "inf")) {
		*varp = INFINITY * sign;
		return TOWLC_SUCCESS;
	}

	if(match_string(parser, "nan")) {
		*varp = NAN * sign;
		return TOWLC_SUCCESS;
	}

	return TOWLC_INVALID_VALUE;
}


static enum towl_code parse_value(struct towl_parser *parser, void *varp,
                                  enum towl_type *type);

static enum towl_code array_add(towl_array *array, void *p,
		                        enum towl_type type);

union expensive {
	towl_table *tval;
	char *sval;
	int64_t ival;
	double fval;
	bool bval;
	towl_array *aval;
	struct towl_datetime dtval;
	char cval;
};


static enum towl_code parse_array(struct towl_parser *parser,
                                  towl_array **varp) {
	
	enum towl_code code;	
	enum towl_type type;
	bool trailing_comma = 1;

	towl_array *array = towl_new_array();
	while(true) {
		skip_whitespace(parser);
		
		bool match_bracket = match(parser, ']');
		if(match_bracket && !trailing_comma)
			break;
		else if(!match_bracket && !trailing_comma)
			return TOWLC_EXPECTED_BRACKET;

		union expensive val;				
		code = parse_value(parser, &val, &type);
		if(code != TOWLC_SUCCESS) {
			towl_clean_array(array);
			return code;
		}
		
		switch(type) {
			case TOWL_TABLE:	
				code = towl_array_add_table(array, val.tval);	
				break;
			case TOWL_STRING:
				code = towl_array_add_string(array, val.sval);	
				break;
			case TOWL_INT:
				code = towl_array_add_int(array, val.ival);	
				break;
			case TOWL_FLOAT:
				code = towl_array_add_float(array, val.fval);	
				break;
			case TOWL_BOOL:
				code = towl_array_add_bool(array, val.bval);	
				break;
			case TOWL_ARRAY:
				code = towl_array_add_array(array, val.aval);	
				break;
			case TOWL_DATETIME:
				code = towl_array_add_datetime(array, val.dtval);	
				break;
		}
		if(code != TOWLC_SUCCESS) {
			towl_clean_array(array);
			return code;
		}

		skip_whitespace(parser);

		if(!match(parser, ','))
			trailing_comma = 0;
	}
	
	*varp = array;
	return TOWLC_SUCCESS;
}

static enum towl_code parse_value(struct towl_parser *parser, void *varp,
                                  enum towl_type *type) {
	enum towl_code code;

	char next = peek(parser, 0);

	switch(next) {
		case '"':
			*type = TOWL_STRING;

			if(match_string(parser, "\"\"\"")) {
				code = string_dquote_multi(parser, varp);
				break;
			}
			advance(parser);
			code = string_dquote(parser, varp);
			break;
		case '\'':
			*type = TOWL_STRING;

			if(match_string(parser, "'''")) {
				code = string_squote_multi(parser, varp);
				break;
			}

			advance(parser);
			code = string_squote(parser, varp);
			break;
		case 't':
		case 'f':
			*type = TOWL_BOOL;
			parse_bool(parser, varp);
			break;
		case '[':
			advance(parser);
			*type = TOWL_ARRAY;
			code = parse_array(parser, varp);
			break;
		case 'i':
		case 'n':
			*type = TOWL_FLOAT;
			code = parse_inf_nan(parser, varp);
			break;
		default:
			if(is_digit(next) || next == '+' || next == '-')
				code = parse_numeric(parser, varp, type);	
			else //TODO Error handling may be improved
				return TOWLC_INVALID_VALUE;
	}

	return code;
}

static enum towl_code parse_key_value(struct towl_parser *parser) {
	enum towl_code code;
	
	towl_table *deepest;
	const char *left_key, *right_key;
	code = parse_key(parser, parser->global, &deepest, &left_key, &right_key);
	if(code != TOWLC_SUCCESS)
		return code;

	enum towl_type type;
	union expensive val;
	code = parse_value(parser, &val, &type);
	if(code != TOWLC_SUCCESS)
		return code;

	if(!match(parser, '\n'))
		return TOWLC_EXPECTED_NEWLINE;

	switch(type) {
		case TOWL_TABLE:	
			code = singlekey_add(deepest, right_key, val.tval, type);	
			break;
		case TOWL_STRING:
			code = singlekey_add(deepest, right_key, val.sval, type);	
			break;
		case TOWL_INT:
			code = singlekey_add(deepest, right_key, &val.ival, type);	
			break;
		case TOWL_FLOAT:
			code = singlekey_add(deepest, right_key, &val.fval, type);	
			break;
		case TOWL_BOOL:
			code = singlekey_add(deepest, right_key, &val.bval, type);	
			break;
		case TOWL_ARRAY:
			code = singlekey_add(deepest, right_key, val.aval, type);	
			break;
		case TOWL_DATETIME:
			code = singlekey_add(deepest, right_key, &val.dtval, type);	
			break;
	}
	if(code != TOWLC_SUCCESS)
		return code;

	return TOWLC_SUCCESS;
}


//ARRAY OPS
static enum towl_code array_add(towl_array *array, void *value,
                                enum towl_type type) {
	if(array->len == array->cap) {
		if(array->cap == 0)
			array->cap = 4;
		else
			array->cap *= 2;

		array->type = realloc(array->type,
				              array->cap * sizeof(enum towl_type));
		array->arr = realloc(array->arr, array->cap * sizeof(union towl_union));
	}

	array->type[array->len] = type;
	
	switch(type) {
		case TOWL_TABLE:
			(array->arr)[array->len].tval = (towl_table*) value;
			break;
		case TOWL_STRING:
			(array->arr)[array->len].sval = strdup((char*) value);
			break;
		case TOWL_INT:
			(array->arr)[array->len].ival = *((int64_t*) value);
			break;
		case TOWL_FLOAT:
			(array->arr)[array->len].fval = *((double*) value);
			break;
		case TOWL_BOOL:
			(array->arr)[array->len].bval = *((bool*) value);
			break;
		case TOWL_ARRAY:
			(array->arr)[array->len].aval = (towl_array*) value;
			break;
		case TOWL_DATETIME:
			;
			size_t size = sizeof(struct towl_datetime);
			struct towl_datetime *dt = malloc(size);
			memcpy(dt, value, size);

			(array->arr)[array->len].dtp = dt;
			break;
	}

	array->len++;

	return TOWLC_SUCCESS;
}

enum towl_code towl_array_add_table(towl_array *array, towl_table *val) {
	return array_add(array, val, TOWL_TABLE);
}

enum towl_code towl_array_add_string(towl_array *array, char *val) {
	return array_add(array, val, TOWL_STRING);
}

enum towl_code towl_array_add_int(towl_array *array, int64_t val) {
	return array_add(array, &val, TOWL_INT);
}

enum towl_code towl_array_add_float(towl_array *array, double val) {
	return array_add(array, &val, TOWL_FLOAT);
}

enum towl_code towl_array_add_bool(towl_array *array, bool val) {
	return array_add(array, &val, TOWL_BOOL);
}

enum towl_code towl_array_add_array(towl_array *array, towl_array *val) {
	return array_add(array, val, TOWL_ARRAY);
}

enum towl_code towl_array_add_datetime(towl_array *array, 
                                       struct towl_datetime val) {
	return array_add(array, &val, TOWL_DATETIME);
}


static enum towl_code array_get(towl_array *array, size_t index, void *p,
                                enum towl_type type) {
	if(array->len <= index)
		return TOWLC_INDEX_OUT_OF_RANGE;

	if(array->type[index] != type)
		return TOWLC_TYPE_MISMATCH;
	
	switch(type) {
		case TOWL_TABLE:
			*((towl_table**) p) = (array->arr)[index].tval;
			break;
		case TOWL_STRING:
			*((char**) p) = (array->arr)[index].sval;
			break;
		case TOWL_INT:
			*((int64_t*) p) = (array->arr)[index].ival;
			break;
		case TOWL_FLOAT:
			*((double*) p) = (array->arr)[index].fval;
			break;
		case TOWL_BOOL:
			*((bool*) p) = (array->arr)[index].bval;
			break;
		case TOWL_ARRAY:
			*((towl_array**) p) = (array->arr)[index].aval;
			break;
		case TOWL_DATETIME:
			memcpy((struct towl_datetime*) p,
			       array->arr[index].dtp, 
			       sizeof(struct towl_datetime));
			break;
	}

	return TOWLC_SUCCESS;
}

enum towl_code towl_array_get_table(towl_array *array, size_t index,
                                    towl_table **val) {
	return array_get(array, index, val, TOWL_TABLE);
}

enum towl_code towl_array_get_string(towl_array *array, size_t index, 
                                     const char **val) {
	return array_get(array, index, val, TOWL_STRING);
}

enum towl_code towl_array_get_int(towl_array *array, size_t index, int64_t *val) {
	return array_get(array, index, val, TOWL_INT);
}

enum towl_code towl_array_get_float(towl_array *array, size_t index, 
                                    double *val) {
	return array_get(array, index, val, TOWL_FLOAT);
}

enum towl_code towl_array_get_bool(towl_array *array, size_t index,
                                   bool *val)  {
	return array_get(array, index, val, TOWL_BOOL);
}

enum towl_code towl_array_get_array(towl_array *array, size_t index,
                                    towl_array **val) {
	return array_get(array, index, val, TOWL_ARRAY);
}

enum towl_code towl_array_get_datetime(towl_array *array, size_t index,
                                       struct towl_datetime *val) {
	return array_get(array, index, val, TOWL_DATETIME);
}

static enum towl_code array_set(towl_array *array, size_t index, void *value,
                                enum towl_type type) {
	if(array->len <= index)
		return TOWLC_INDEX_OUT_OF_RANGE;
	
	switch(type) {
		case TOWL_TABLE:
			clear_table((array->arr)[index].tval);
			(array->arr)[index].tval = (towl_table*) value;
			break;
		case TOWL_STRING:
			free((array->arr)[index].sval);
			(array->arr)[index].sval = strdup((char*) value);
			break;
		case TOWL_INT:
			(array->arr)[index].ival = *((int64_t*) value);
			break;
		case TOWL_FLOAT:
			(array->arr)[index].fval = *((double*) value);
			break;
		case TOWL_BOOL:
			(array->arr)[index].bval = *((bool*) value);
			break;
		case TOWL_ARRAY:
			clear_array((array->arr)[index].aval);
			(array->arr)[index].aval = (towl_array*) value;
			break;
		case TOWL_DATETIME:
			memcpy((array->arr)[index].dtp, value,
			       sizeof(struct towl_datetime));
			break;
	}

	(array->type)[index] = type; 

	return TOWLC_SUCCESS;
}

enum towl_code towl_array_set_table(towl_array *array, size_t index,
                                    towl_table *val) {
	return array_set(array, index, val, TOWL_TABLE);
}

enum towl_code towl_array_set_string(towl_array *array, size_t index,
                                     char *val) {
	return array_set(array, index, val, TOWL_STRING);
}

enum towl_code towl_array_set_int(towl_array *array, size_t index, int64_t val) {
	return array_set(array, index, &val, TOWL_INT);
}

enum towl_code towl_array_set_float(towl_array *array, size_t index,
                                    double val)  {
	return array_set(array, index, &val, TOWL_FLOAT);
}

enum towl_code towl_array_set_bool(towl_array *array, size_t index, bool val) {
	return array_set(array, index, &val, TOWL_BOOL);
}

enum towl_code towl_array_set_array(towl_array *array, size_t index,
                                    towl_array *val) {
	return array_set(array, index, val, TOWL_ARRAY);
}

enum towl_code towl_array_set_datetime(towl_array *array, size_t index,
                                       struct towl_datetime val) {
	return array_set(array, index, &val, TOWL_DATETIME);
}

size_t towl_array_len(towl_array *array)  {
	return array->len;
}

#ifdef TOWL_TEST
static char *type_to_string(enum towl_type type) {
	switch(type) {
		case TOWL_TABLE:
			return "Table";
		case TOWL_STRING:
			return "String";
		case TOWL_INT:
			return "Int";
		case TOWL_FLOAT:
			return "Float";
		case TOWL_BOOL:
			return "Bool";
		case TOWL_ARRAY:
			return "Array";
		default:
			return "<Unknown>";
	}
}

static void print_node(struct towl_node*, int tab_count);
static void print_table_with_tabs(towl_table *table, int tab_count);

static void print_tabs(int tab_count) {
	for(int i = 0; i < tab_count; i++)
		printf("\t");
}

static void print_array_with_tabs(towl_array *array, int tab_count) {
	print_tabs(tab_count);
	printf("--- array start ---\n");

	print_tabs(tab_count);
	printf("cap: %u\n", array->cap);

	print_tabs(tab_count);
	printf("len: %u\n", array->len);

	print_tabs(tab_count);
	printf("{ ");
	
	for(size_t i = 0; i < array->len; i++) {
		switch(array->type[i]) {
			case TOWL_TABLE:
				printf("\n");
				print_table_with_tabs(((towl_table*) array->arr) + i,
				                      tab_count);
				printf(", ");
				break;
			case TOWL_STRING:
				printf("\"%s\", ", ((char**) array->arr)[i]);
				break;
			case TOWL_INT:
				printf("%" PRId64 ", ", ((int64_t*) array->arr)[i]);
				break;
			case TOWL_FLOAT:
				printf("%f, ", ((double*) array->arr)[i]);
				break;
			case TOWL_BOOL:
				printf("%d, ", ((bool*) array->arr)[i]);
				break;
			case TOWL_ARRAY:
				printf("\n");
				print_array(array);
				break;
		}
	}

	printf("\b\b }\n");
	
	print_tabs(tab_count);
	printf("--- array end ---\n\n");
}

void print_array(towl_array *array) {
	print_array_with_tabs(array, 0);
}

static void print_table_with_tabs(towl_table *table, int tab_count) {
	struct towl_node *iter = table->child;

	for(int i = 0; iter != NULL; i++) {
		print_tabs(tab_count);
		printf("table elem %d:\n", i);

		print_node(iter, tab_count + 1);
		iter = iter->next;
	}
}

void print_table(towl_table *table) {
	print_table_with_tabs(table, 0);
}

void print_datetime(struct towl_datetime dt) {
	printf("secfrac: %ld\n", dt.secfrac);
	printf("sec: %d\n", dt.sec);
	printf("min: %d\n", dt.min);
	printf("hour: %d\n", dt.hour);
	printf("mday: %d\n", dt.mday);
	printf("mon: %d\n", dt.mon);
	printf("year: %d\n", dt.year);
	printf("gmtoff: %ld\n", dt.gmtoff);
}

static void print_node(struct towl_node *node, int tab_count) {
	if(node == NULL)
		printf("(NULL NODE)\n");

	print_tabs(tab_count);
	printf("key: %s\n", node->key);

	print_tabs(tab_count);
	printf("type: %s\n", type_to_string(node->type));

	switch(node->type) {
		case TOWL_TABLE:
			print_table_with_tabs(((struct towl_table_node*) node)->table,
			                      tab_count);
			break;
		case TOWL_STRING:
			print_tabs(tab_count);
			printf("value: %s\n", ((struct towl_string_node*) node)->value);
			break;
		case TOWL_INT:
			print_tabs(tab_count);
			printf("value: %d\n", ((struct towl_int_node*) node)->value);
			break;
		case TOWL_FLOAT:
			print_tabs(tab_count);
			printf("value: %f\n", ((struct towl_float_node*) node)->value);
			break;
		case TOWL_BOOL:
			print_tabs(tab_count);
			printf("value: %d\n", ((struct towl_bool_node*) node)->value);
			break;
		case TOWL_ARRAY:
			print_array_with_tabs(((struct towl_array_node*) node)->array,
			                      tab_count);
			break;
	}
}

struct towl_node *static_append_node(towl_table *parent, char *key, 
                              enum towl_type type, size_t size) {
	return append_node(parent, key, type, size);
}

struct towl_node *static_find_node(towl_table *parent, char *key) {
	return find_node(parent, key);
}

enum towl_code static_singlekey_add(towl_table *parent, char *key, void *value, 
                                    enum towl_type type) {
	return singlekey_add(parent, key, value, type);
}

enum towl_code static_singlekey_get(towl_table *parent, char *key, void *p, 
                                    enum towl_type type) {
	return singlekey_get(parent, key, p, type);
}

enum towl_code static_singlekey_set(towl_table *parent, char *key, void *value,
                                    enum towl_type type) {
	return singlekey_set(parent, key, value, type);
}

enum towl_code static_singlekey_remove(towl_table *parent, char *key) {
	return singlekey_remove(parent, key);
}

struct towl_parser *static_new_parser(char *src) {
	return new_parser(src);
}

void static_change_parser_src(struct towl_parser *parser, char *src) {
	change_parser_src(parser, src);
}

enum towl_code static_string_dquote(struct towl_parser *parser, char **str) {
	return string_dquote(parser, str);
}

enum towl_code static_string_dquote_multi(struct towl_parser *parser,
                                          char **str) {
	return string_dquote_multi(parser, str);
}

enum towl_code static_string_squote(struct towl_parser *parser, char **str) {
	return string_squote(parser, str);
}

enum towl_code static_string_squote_multi(struct towl_parser *parser,
                                          char **str) {
	return string_squote_multi(parser, str);
}


enum towl_code static_barekey(struct towl_parser *parser, char **str) {
	return barekey(parser, str);
}

enum towl_code static_parse_key(struct towl_parser *parser, towl_table *parent,
                                towl_table **deepest_table,
                                const char **leftmost_key, 
                                const char **rightmost_key) {
	return parse_key(parser, parent, deepest_table, leftmost_key,
	                 rightmost_key);
}


enum towl_code static_parse_numeric(struct towl_parser *parser, void *varp,
                                    enum towl_type *type) {
	return parse_numeric(parser, varp, type);
}

enum towl_code static_parse_value(struct towl_parser *parser, void *varp,
                                  enum towl_type *type) {
	return parse_value(parser, varp, type);
}

#endif
