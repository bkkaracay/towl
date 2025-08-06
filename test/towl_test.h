#ifndef TOWL_TEST_H
#define TOWL_TEST_H

#include "../src/towl.h"

struct towl_parser {
	char *src;
	char *pos;
	char *mark;
	int line;
};

enum towl_code static_singlekey_add(towl_table *parent, char *key, void *value, 
                                    enum towl_type type);
enum towl_code static_singlekey_get(towl_table *parent, char *key, void *p, 
                                    enum towl_type type);
enum towl_code static_singlekey_set(towl_table *parent, char *key, void *value,
                                    enum towl_type type);
enum towl_code static_singlekey_remove(towl_table *parent, char *key);

struct towl_parser *static_new_parser(char *src);
void static_change_parser_src(struct towl_parser *parser, char *src);

enum towl_code static_string_dquote(struct towl_parser *parser, char **str);
enum towl_code static_string_dquote_multi(struct towl_parser *parser,
                                          char **str);
enum towl_code static_string_squote(struct towl_parser *parser, char **str);
enum towl_code static_string_squote_multi(struct towl_parser *parser,
                                          char **str);


enum towl_code static_barekey(struct towl_parser *parser, char **str);
enum towl_code static_parse_key(struct towl_parser *parser, 
                                towl_table *parent,
                                towl_table **deepest_table, 
                                const char **leftmost_key,
                                const char **rightmost_key);

enum towl_code static_parse_numeric(struct towl_parser *parser, void *varp,
                                    enum towl_type *type);

enum towl_code static_parse_value(struct towl_parser *parser, void *varp,
                                  enum towl_type *type);
#endif
