#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "file_vec.h"

DIR* open_dir(char* dir_path);
int get_files_from_dir(DIR* dir, FILE_VEC* file_vec);
int get_files_with_ext(DIR* dir, const char* ext, FILE_VEC* file_vec);

char *append_str(char* dst, const char* src);
