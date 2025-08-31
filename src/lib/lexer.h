#pragma once

#include <stdio.h>
#include <stddef.h>
#include <string.h>

typedef enum {
    TOKEN_KEYWORD, TOKEN_IDENT,
    TOKEN_TYPE,
    
    TOKEN_INT_LIT, TOKEN_STR_LIT,
    TOKEN_FLOAT_LIT, TOKEN_DOUBLE_LIT,
    TOKEN_CHR_LIT,

    TOKEN_LSQRLY, TOKEN_RSQRLY,
    TOKEN_LPAREN, TOKEN_RPAREN,

    TOKEN_EQ, TOKEN_EQEQ,
    TOKEN_NE, TOKEN_GE,
    TOKEN_PLEQ, TOKEN_MNEQ,

    TOKEN_LE, TOKEN_LT,
    TOKEN_GT, TOKEN_PLUS,
    TOKEN_MINUS, TOKEN_INC,
    TOKEN_DEC, TOKEN_AND,
    TOKEN_OR,

    TOKEN_BITOR,
    TOKEN_BITAND,

    TOKEN_NEGATE, TOKEN_STAR,
    TOKEN_SLASH, TOKEN_MOD,
    TOKEN_EXP,

    TOKEN_SEMICOLON,
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
