#pragma once

#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Lexer *lexer;
    Token token;
} Parser;

void init_parser(Parser *parser);
Parser *config_parser(Parser *parser, Lexer *lexer);
void free_parser(Parser *parser);

void parse_file(Parser *parser);
bool parser_keyword_match(char *string);
