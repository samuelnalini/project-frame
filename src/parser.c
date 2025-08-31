#include "lib/parser.h"
#include "lib/lexer.h"
#include "lib/ast.h"
#include "lib/utils.h"
#include "lib/sema.h"

#include <stdbool.h>
#include <stdlib.h>

void init_parser(Parser *parser) {
    parser->lexer = NULL;
    parser->token = (Token) {
	.type = TOKEN_UNK,
	.start = '\0',
	.length = 0,
	.line = 0
    };
}

Parser *config_parser(Parser *parser, Lexer *lexer) {
    if (!parser || !lexer) return NULL;
    init_parser(parser);
    parser->lexer = lexer;
    return parser;
}

void free_parser(Parser *parser) {
    if (!parser) return;
    if (parser->lexer)
	free_lexer(parser->lexer);
    init_parser(parser);
}

static char *keywords[] = {
    "return", "null"
};

bool parser_keyword_match(char *string) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
	if (strcmp(string, keywords[i]) == 0)
	    return true;
    }

    return false;
}

static bool match(Parser *parser, TokenType type) {
    if (parser->token.type == type) {
	parser->token = get_token(parser->lexer);
	return true;
    }

    return false;
}

static void error(Parser *parser, const char *message, int line) {
    fprintf(stderr, "Parse Error [line %d]: %s\n", line, message);
}

static bool expect(Parser *parser, TokenType type) {
    if (!match(parser, type)) {
	if (parser->token.type == TOKEN_ERR)
	    return false; // don't print error msg
	
	fprintf(stderr, "Parse Error [line %d]: expected %s but got %s\n",
		parser->token.line, get_token_type_str(type),
		get_token_type_str(parser->token.type)
	);
	return false;
    }

    return true;
}

static AST_Node *parse_expr(Parser *parser) {
    puts("Expressions have not been implemented yet.");
    parser->token = get_token(parser->lexer); // consume the equal!
    return NULL;
}

static void parse_stmt(Parser *parser) {
    parser->token = get_token(parser->lexer);
    Token token = parser->token;

    printf("Type: %s\nVal: %.*s\nLine: %d\nStart: %c\nLen: %d\n",
	   get_token_type_str(token.type),
	   token.length, token.start,
	   token.line,
	   *token.start, token.length
	);
    
    if (token.type == TOKEN_EOF) {
	return;
    }

    // check if the statement starts off with a type
    
    if (token.type == TOKEN_TYPE) {
	// var declaration
	parser->token = get_token(parser->lexer); // consume the type
	
	Token var_name_token = parser->token;

	if (!expect(parser, TOKEN_IDENT)) { // check if the name is an identifier
	    return;
	}

	// on to the equal
	AST_Node *init = NULL;
	if (match(parser, TOKEN_EQ)) {
	    init = parse_expr(parser);
	}

	// should be a semi colon
	if (!expect(parser, TOKEN_SEMICOLON)) {
	    error(parser, "Expected a ';' at the end of expression", parser->token.line);
	    printf("-> got %c instead\n", parser->token.start);
	    return;
	}

	printf("Declaring %.*s as %s\n",
	       var_name_token.length, var_name_token.start,
	    "idk yet");
	// declare in sym table
    }

    // check token type and send to correct function
    return;
}

void parse_file(Parser *parser) {
    printf("parsing buffer\n");

    while (parser->token.type != TOKEN_EOF) {
	parse_stmt(parser);
    }
}
