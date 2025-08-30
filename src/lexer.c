#include "lib/lexer.h"

#include <ctype.h>

void init_lexer(Lexer *lexer) {
    if (!lexer) return;
    lexer->buf = NULL;
    lexer->line = 0;
    lexer->index = 0;
}

Lexer *config_lexer(Lexer *lexer, char *buf) {
    if (!lexer || !buf) return NULL;
    init_lexer(lexer);
    lexer->line = 1;
    lexer->buf = buf;
    return lexer;
}

void free_lexer(Lexer *lexer) {
    if (!lexer) return;
    //if (lexer->buf) free(lexer->buf); // holding off for now because it's a ptr to the actual thing
    init_lexer(lexer);
}

static void advance(Lexer *lexer) {
    lexer->index++;
}

static bool at_eof(Lexer *lexer) {
    if (lexer->index >= strlen(lexer->buf)) // buffer is \0 terminated
	return true;

    return false;
}

static void skip_whitespace(Lexer *lexer) {
    while (true) {
	if (at_eof(lexer))
	    break;
	
	char ch = lexer->buf[lexer->index];
	if (isblank(ch)) { // space, tab
	    advance(lexer);
	    continue;
	}

	if (ch == '\n') { // new line
	    lexer->line++;
	    advance(lexer);
	    continue;
	}

	break; // anything else
    }
}

static Token make_token(TokenType type, char *start, size_t length, int line) {
    return (Token) {
	.type = type,
	.start = start,
	.length = length,
	.line = line
    };
}

static Token single_ch_token(Lexer *lexer, TokenType type) {
    return make_token(type, &lexer->buf[lexer->index - 1], 1, lexer->line);
}

static Token double_ch_token(Lexer *lexer, TokenType type) {
    return make_token(type, &lexer->buf[lexer->index - 2], 2, lexer->line);
}

static Token error(Lexer *lexer, const char *message, int line) {
    fprintf(stderr, "Error [line %d]: %s\n", lexer->line, message);

    while (lexer->buf[lexer->index] != '\n') {
	if (at_eof(lexer))
	    break;
	
	advance(lexer);
    }

    {
	advance(lexer); // consume the newline
	lexer->line++;
    }
    
    return make_token(TOKEN_ERR, &lexer->buf[lexer->index], 0, line);
}

static Token number(Lexer *lexer) {
    int start = lexer->index;
    int line = lexer->line;
    
    TokenType type = TOKEN_INT_LIT;
    bool seen_dot = false;
    bool seen_ch = false;
    
    while (true) {
	if (at_eof(lexer)) {
	    break;
	}

	char ch = lexer->buf[lexer->index];

	if (isblank(ch))
	    break; // exit at whitespace

	if (ch == '\n') {
	    lexer->line++;
	    break; // increment line and exit at whitespace
	}
	
	if (isdigit(ch)) {
	    if (seen_ch)
		return error(lexer, "Invalid number (digit after number type)", line);
	    advance(lexer);
	    continue;
	}

	if (ch == 'f') {
	    if (seen_ch)
		return error(lexer, "Invalid number type", line);
	    seen_ch = true;
	    type = TOKEN_FLOAT_LIT;
	    continue;
	}

	if (ch == '.') {
	    if (seen_dot)
		return error(lexer, "Invalid number (there are too many decimals)", line);
	    seen_dot = true;
	    type = TOKEN_DOUBLE_LIT;
	    continue;
	}

	if (isalpha(ch)) {
	    return error(lexer, "Invalid char in number", line);
	}
    }

    return make_token(type, &lexer->buf[start], (lexer->index - start), line);
}

static void rollback(Lexer *lexer, int index, int line) {
    fprintf(stderr, "Lexer rollback due to error\nOriginal index: %d\nOriginal line: %d\nRolled back index: %d\nRolled back line: %d\n", lexer->index, lexer->line, index, line);
    lexer->index = index;
    lexer->line = line;
}

static Token string(Lexer *lexer) {
    int start = lexer->index;
    int line = lexer->line;

    while (true) {
	if (at_eof(lexer)) {
	    rollback(lexer, start, line);
	    return error(lexer, "Unterminated string, skipping line", line);
	}

	char ch = lexer->buf[lexer->index];
	if (ch == '\n') {
	    lexer->line++;
	    advance(lexer);
	    continue;
	}

	if (ch == '"') {
	    advance(lexer);
	    break;
	}

	advance(lexer);
    }

    return make_token(TOKEN_STR_LIT, &lexer->buf[start + 1], (lexer->index - 1) - (start + 1), line);
}

static Token identifier(Lexer *lexer) {
    int start = lexer->index;
    int line = lexer->line;

    while (true) {
	if (at_eof(lexer))
	    break;

	char ch = lexer->buf[lexer->index];
	if (isblank(ch))
	    break;

	if (ch == '\n') {
	    lexer->line++;
	    break;
	}

	advance(lexer);
    }

    return make_token(TOKEN_IDENT, &lexer->buf[start], (lexer->index - start), line);
}

Token get_token(Lexer *lexer) {
    skip_whitespace(lexer);

    if (at_eof(lexer))
	return make_token(TOKEN_EOF, &lexer->buf[lexer->index - 1], 0, lexer->line);
    
    char ch = lexer->buf[lexer->index];
    switch(ch) {
    case '(':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_LPAREN);
    case ')':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_RPAREN);
    case '{':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_LSQRLY);
    case '}':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_RSQRLY);
    case '"':
	return string(lexer);
    default:
	if (isdigit(ch))
	    return number(lexer);

	if (isprint(ch))
	    return identifier(lexer);

	advance(lexer);
	return make_token(TOKEN_UNK, &lexer->buf[lexer->index - 1], 1, lexer->line);
    }
}
