#pragma once

#include <stdio.h>
#include <stddef.h>
#include <string.h>

typedef enum {
    TOKEN_KEYWORD,
    
    TOKEN_INT_LIT,
    TOKEN_STR_LIT,
    TOKEN_FLOAT_LIT,
    TOKEN_DOUBLE_LIT,
    TOKEN_CHR_LIT,

    TOKEN_IDENT,

    TOKEN_LSQRLY,
    TOKEN_RSQRLY,
    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_EOF,
    TOKEN_ERR,
    TOKEN_UNK,
} TokenType;

typedef struct {
    TokenType type;
    char *start;
    size_t length;
    int line;
} Token;

typedef struct {
    char *buf;
    int line;
    int index;
} Lexer;

void init_lexer(Lexer *lexer);
void free_lexer(Lexer *lexer);

Lexer *config_lexer(Lexer *lexer, char *buf);

Token get_token(Lexer *lexer);
