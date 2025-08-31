#include "lib/lexer.h"
#include "lib/parser.h"
#include "lib/sema.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

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
    if (lexer->buf) free(lexer->buf);
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

	if (isblank(ch)) {
	    break; // exit at whitespace
	}
	    
	if (ch == '\n' || ch == ';') {
	    break;
	}
	
	if (isdigit(ch)) {
	    if (seen_ch) {
		advance(lexer);
		return error(lexer, "Invalid number (digit after number type)", line);
	    }

	    advance(lexer);
	    continue;
	}

	if (ch == 'f') {
	    advance(lexer);
	    
	    if (seen_ch) {
		return error(lexer, "Invalid number type", line);
	    }
		
	    seen_ch = true;
	    type = TOKEN_FLOAT_LIT;	    
	    continue;
	}

	if (ch == '.') {
	    advance(lexer);
	    
	    if (seen_dot) {
		return error(lexer, "Invalid number (there are too many decimals)", line);
	    }
		
	    seen_dot = true;
	    type = TOKEN_DOUBLE_LIT;
	    continue;
	}

	if (isalpha(ch)) {
	    advance(lexer);
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
    advance(lexer); // consume the "
    int start = lexer->index;
    int line = lexer->line;
    
    while (true) {
	if (at_eof(lexer)) {
	    rollback(lexer, start, line);
	    return error(lexer, "Unterminated string, skipping line", line);
	}

	char ch = lexer->buf[lexer->index];
	if (ch == '\n') {
	    advance(lexer);
	    lexer->line++;
	    continue;
	}

	if (ch == '"') {
	    advance(lexer);
	    break;
	}

	advance(lexer);
    }

    return make_token(TOKEN_STR_LIT, &lexer->buf[start], (lexer->index - start - 1), line);
}

static Token character(Lexer *lexer) {
    advance(lexer); // consume the '
    int start = lexer->index;
    int line = lexer->index;

    int charLen = 1;
    if (lexer->buf[start] == '\\') {
	// this is an escaped character like \n
	charLen = 2; // increment the char's length and consume the backslash
	advance(lexer);
    }

    if (!isalnum(lexer->buf[lexer->index])) {
	return error(lexer, "Invalid character literal", line);
    }

    advance(lexer); // consume the actual character
    
    if (lexer->buf[lexer->index] != '\'') { // check the closing '
	return error(lexer, "Unterminated character literal", line);
    }

    advance(lexer); // consume the final '
    return make_token(TOKEN_CHR_LIT, &lexer->buf[start], charLen, line);
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

	if (ch == '\n' || ch == ';') {
	    break;
	}

	advance(lexer);
    }
    
    int length = (lexer->index - start);
    char string[length + 1];
    //puts("made it to the seg faulter 5000");
    snprintf(string, (length + 1), "%.*s", length, &lexer->buf[start]); // does this work
    
    if (parser_keyword_match(string)) {
	return make_token(TOKEN_KEYWORD, &lexer->buf[start], length, line);
    }
    
    if (sema_keyword_match(string)) {
	return make_token(TOKEN_TYPE, &lexer->buf[start], length, line);
    }
    
    return make_token(TOKEN_IDENT, &lexer->buf[start], length, line);
}

static char peek(Lexer *lexer) {
    if (at_eof(lexer)) return '\0';
    if (lexer->index + 1 >= strlen(lexer->buf)) return '\0';
    return lexer->buf[lexer->index + 1];
}

Token get_token(Lexer *lexer) {
    skip_whitespace(lexer);

    if (at_eof(lexer))
	return make_token(TOKEN_EOF, &lexer->buf[lexer->index - 1], 0, lexer->line);

    printf("\n------------------------------------------------------\n");
    
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
    case ';':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_SEMICOLON);
    case '&':
	if (peek(lexer) == '&') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_AND);
	}

	return single_ch_token(lexer, TOKEN_BITAND);
    case '|':
	if (peek(lexer) == '|') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_OR);
	}

	return single_ch_token(lexer, TOKEN_BITOR);
    case '+':
	advance(lexer);

	if (lexer->buf[lexer->index] == '+') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_INC);
	}

	if (lexer->buf[lexer->index] == '=') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_PLEQ);
	}
	
	return single_ch_token(lexer, TOKEN_PLUS);
    case '-':
	advance(lexer);

	if (lexer->buf[lexer->index] == '-') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_DEC);
	}

	if (lexer->buf[lexer->index] == '=') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_MNEQ);
	}	
	
	return single_ch_token(lexer, TOKEN_MINUS);
    case '!':
	advance(lexer);

	if (lexer->buf[lexer->index] == '=') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_NE);
	}	
	
	return single_ch_token(lexer, TOKEN_NEGATE);
    case '=':
	advance(lexer);

	if (lexer->buf[lexer->index] == '=') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_EQEQ);
	}
	
	return single_ch_token(lexer, TOKEN_EQ);
    case '>':
	advance(lexer);

	if (lexer->buf[lexer->index] == '=') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_GE);
	}
	
	return single_ch_token(lexer, TOKEN_GT);
    case '<':
	advance(lexer);

	if (lexer->buf[lexer->index] == '=') {
	    advance(lexer);
	    return double_ch_token(lexer, TOKEN_LE);
	}
	
	return single_ch_token(lexer, TOKEN_LT);
    case '/':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_SLASH);
    case '*':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_STAR);
    case '%':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_MOD);
    case '^':
	advance(lexer);
	return single_ch_token(lexer, TOKEN_EXP);
    case '"':
	return string(lexer);
    case '\'':
	return character(lexer);
    default:
	if (isdigit(ch))
	    return number(lexer);

	if (isprint(ch))
	    return identifier(lexer);

	advance(lexer);
	return make_token(TOKEN_UNK, &lexer->buf[lexer->index - 1], 1, lexer->line);
    }
}
