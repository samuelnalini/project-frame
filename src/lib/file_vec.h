#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct {
    char *dir;
    char *name;
    FILE *ptr;
} File;

typedef struct {
    uint16_t count;
    uint16_t capacity;
    File **vec;
} FILE_VEC;

void file_vec_init(FILE_VEC *vec);
void file_vec_free(FILE_VEC *vec);

int file_vec_append(FILE_VEC *vec, File *value);
int file_vec_pop(FILE_VEC *vec);
int file_vec_erase(FILE_VEC *vec, int index);

void file_init(File *file);
void file_free(File *file);

