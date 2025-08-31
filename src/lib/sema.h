#pragma once

#include <stdbool.h>

typedef enum DataType {
    TYPE_STRING,
    TYPE_CHAR,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_FLOAT,
    TYPE_DOUBLE,
} DataType;

bool sema_keyword_match(char *string);
