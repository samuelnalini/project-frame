#pragma once

#include "sema.h"

typedef enum NodeType {
    NODE_ASSIGN,
    
} NodeType;

typedef struct AST_Node {
    NodeType kind;
    DataType type;
    Token token;
/*
    struct AST_Node *left;
    struct AST_Node *right;
 */
    
    union {
	// PE MD AS
	struct {struct AST_Node *left; struct AST_Node *right;} assign;
    };
} AST_Node;
