#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct {
    uint16_t count;
    uint16_t capacity;
    FILE **vec;
} FILE_VEC;

void file_vec_init(FILE_VEC *vec);
int file_vec_push(FILE_VEC *vec, FILE *value);
int file_vec_pop(FILE_VEC *vec);
void file_vec_free(FILE_VEC *vec);
