#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>

#include "file_vec.h"

size_t get_file_size(FILE *fd);
DIR *open_dir(char *dir_path);
File *open_file(char *file_path, char *dir_path);
int get_files_from_dir(char *dir_path, FILE_VEC *file_vec);

char *append_str(char *dst, const char *src);
