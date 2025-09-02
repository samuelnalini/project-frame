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

static AST_Node *parse_expr(Parser *parser);

static AST_Node *parse_primary(Parser *parser) {
    Token token = parser->token;

    if (token.type == TOKEN_TYPE) {
	parser->token = get_token(parser->lexer);
	return literal_node(token); // temp
    }

    if (token.type == TOKEN_IDENT) {
	parser->token = get_token(parser->lexer);
	return variable_node(token);
    }

    
    if (match(parser, TOKEN_LPAREN)) {
	AST_Node *expr = parse_expr(parser);
	expect(parser, TOKEN_RPAREN);
	return expr;
    }

    error(parser, "Unexpected token as part of expression", token.line);
}

static AST_Node *parse_unary(Parser *parser) {
    switch(parser->token.type) {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
	Token operator = parser->token;
	parser->token = get_token(parser->lexer);
	AST_Node *operand = parse_unary(parser);
	return unary_node(operator, operand);
    }
    
    return parse_primary(parser);
}

static AST_Node *parse_factor(Parser *parser) {
    AST_Node *left = parse_unary(parser);
    AST_Node *node = NULL;

    switch (parser->token.type) {
    case TOKEN_STAR:
    case TOKEN_SLASH:
	Token operator = parser->token;
	parser->token = get_token(parser->lexer);
	AST_Node *right = parse_unary(parser);
	node = binary_node(operator, left, right);
    }
    
    return (node != NULL ? node : left);
}

// term := (factor ('+' | '-') factor)
static AST_Node *parse_term(Parser *parser) {
    AST_Node *left = parse_factor(parser);
    AST_Node *node = NULL;
    
    switch (parser->token.type) {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
	Token operator = parser->token;
	parser->token = get_token(parser->lexer);
	AST_Node *right = parse_factor(parser);
	node = binary_node(operator, left, right);
    }
    
    return (node != NULL ? node : left);
}

// comparison := (term ('>' | '<' | '<=' | '>=') term)
static AST_Node *parse_compare(Parser *parser) {
    AST_Node *left = parse_term(parser);
    AST_Node *node = NULL;

    switch (parser->token.type) {
    case TOKEN_GT:
    case TOKEN_LT:
    case TOKEN_GE:
    case TOKEN_LE:
	Token operator = parser->token;
	parser->token = get_token(parser->lexer);
	AST_Node *right = parse_term(parser);
	node = binary_node(operator, left, right);
    default:
	return left;
    }
    
    return node;
}

// equality := comparison ( '==', '!=' )
static AST_Node *parse_equal(Parser *parser) {
    AST_Node *left = parse_compare(parser);
    AST_Node *node = NULL;

    if (parser->token.type == TOKEN_EQEQ) {
	Token operator = parser->token;

	parser->token = get_token(parser->lexer);
    
	AST_Node *right = parse_compare(parser);
	node = binary_node(operator, left, right);
    }
    
    return (node != NULL ? node : left);
}

// assignment := equality ( '=' )
static AST_Node *parse_assign(Parser *parser) {
    AST_Node *left = parse_equal(parser);
    
    if (!match(parser, TOKEN_EQ)) {
	return left;
    }

    AST_Node *right = parse_assign(parser);

    if (left == NULL || left->kind != NODE_VARIABLE) {
	error(parser, "Invalid assignment target (invalid after the =)", parser->token.line);
    }

    return assign_node(left, right);
}

// Lowest to highest priority, starting with assignment (=)
static AST_Node *parse_expr(Parser *parser) {
    return parse_assign(parser);
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
	Token type_token = parser->token;
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

	printf("Declaring %.*s (%.*s) as %s\n",
	       var_name_token.length, var_name_token.start,
	       type_token.length, type_token.start,
	    "?");
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
