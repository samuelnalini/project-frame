#include "lib/parser.h"
#include "lib/lexer.h"
#include "lib/utils.h"

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

void parse_stmt(Parser *parser) {
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

    return;
}

void parse_file(Parser *parser) {
    printf("parsing buffer\n");

    while (parser->token.type != TOKEN_EOF) {
	parse_stmt(parser);
    }
}
