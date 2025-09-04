#pragma once

#include "sema.h"
#include "lexer.h"

typedef enum NodeType {
    NODE_ASSIGN,
    NODE_BINARY,
    NODE_UNARY,
    NODE_LITERAL,
    NODE_VARIABLE,
    NODE_EQUAL,
    NODE_EXPR_STMT,
    NODE_VAR_DECL,
    NODE_RETURN,
} NodeType;

typedef struct AST_Node {
    NodeType kind;
    DataType type;
    Token token;
    struct AST_Node *left;
    struct AST_Node *right;
/*    
    union {
	// PEMDAS
	struct {struct AST_Node *left; struct AST_Node *right;} assign;
    };
*/
} AST_Node;


AST_Node *literal_node(Token token);
AST_Node *variable_node(Token token);
AST_Node *unary_node(Token token, AST_Node *operator);
AST_Node *binary_node(Token token, AST_Node *left, AST_Node *right);
AST_Node *assign_node(AST_Node *target, AST_Node *value);
AST_Node *return_node(Token token, AST_Node *value);
AST_Node *expr_stmt_node(AST_Node *expression);
AST_Node *var_decl_node(DataType data_type, Token var_name, AST_Node *init);
void free_node(AST_Node *node);
