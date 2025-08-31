#include "lib/sema.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char *type_keywords[] = {
    "i8", "i16", "i32", "i64",
    "float", "double", "string",
    "char"
};

bool sema_keyword_match(char *string) {
    for (int i = 0; i < sizeof(type_keywords) / sizeof(type_keywords[0]); ++i) {
	if (strcmp(string, type_keywords[i]) == 0)
	    return true;
    }

    return false;
}
