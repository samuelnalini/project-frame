#include "lib/utils.h"

DIR* open_dir(char* dir_path) {
    // opendir(3)
    DIR* dir = opendir(dir_path);

    if (dir == NULL) {
        fprintf(stderr, "Could not open dir '%s'\n", dir_path);
        return NULL;
    }

    return dir;
}

int get_files_from_dir(DIR* dir, FILE_VEC* vec) {
    // populate file_vec with FILE*


    return 0;
}

int get_files_with_ext(DIR* dir, const char* ext, FILE_VEC* vec) {
    // return a dynamic array of files

    return 0;
}

void append_str(char* dst, const char* src, char* final_str) {
    snprintf(final_str, strlen(dst) + strlen(src), "%s%s", dst, src); // segfault :(
}
