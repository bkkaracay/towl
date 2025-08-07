# TOWL - A TOML Parser (Not Functional)
> ⚠️ This parser is **currently not functional**. I started this project to learn how to write a parser. I have learned a lot, but I feel continuing this project won't add much value to me.

> ✅ I have used AI for code reviewing - not for writing it from scratch. All code in this repository is hand-written. I believe using AI to write code is valuable for **production** purposes, but not ideal not for learning.
## Installation
Simply copy `towl.c` and `towl.h` from the `src/` directory into your project.

## How to use
```C
const char *source_string = "program = \"Web Server\"\n"
    "[dbms]\n"
    "server = \"127.0.0.1\"\n"
    "port = 5432\n"
    "tables = [ \"users\", \"pages\"]";

int main() {
    towl_table *global;
    towl_parse_string(source_string, &global);

    const char *program;
    towl_get_string(global, "program", &program);

    towl_table *dbms;
    towl_get_table(global, "dbms", &dbms);

    const char *server;
    int port;
    towl_get_string(dbms, "server", &server);
    towl_get_int(dbms, "port", &port);

    towl_array *tables;
    towl_get_array(dbms, "tables", &tables);

    const char *first;
    towl_array_get_string(tables, 0, &first);

    towl_clean(global);
}
```

## Complated Parts
- [x] Core Data Model
- [ ] Parser
    - [x] Key
    - [ ] Value
        - [x] String
        - [x] Int
        - [x] Float
        - [x] Bool
        - [x] Array
        - [x] Datetime
        - [ ] Inline Tables
    - [ ] Key-Value Pair Handling
    - [ ] Tables
        - [x] With Dotted Key
        - [ ] Standard
    - [ ] Array of Tables

## What I Learned

1. Implementing a lexer can reduce complexity more than I expected. I had avoided implementing it because of CPU and memory costs, but I later found out that costs are actually very low.
2. Writing all code in a single file increases complexity much more than I expected. The project should be organized into multiple files with proper headers. I avoided that to make the library easier to include, but that was wrong.
3. I realize that TOML was not a good fit for me. It might sound strange, but I really haven't liked TOML. Its syntax is messy and -unlike its name- it's not a minimal language. I find datetime support, multiple definition syntaxes for defining tables, and array of tables unnecessary. These features go against its minimalism. Additionally, the inline and dotted table formats make it difficult to generate TOML code programmatically.
