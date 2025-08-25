#include "lib/utils.h"

DIR *open_dir(char* dir_path) {
    // opendir(3)
    DIR *dir = opendir(dir_path);

    if (dir == NULL) {
        fprintf(stderr, "Could not open dir '%s'\n", dir_path);
        return NULL;
    }

    return dir;
}

int get_files_from_dir(char *dir_path, FILE_VEC* vec) {
    // populate file_vec with FILE*
    
    DIR *dir = open_dir(dir_path);

    if (dir == NULL)
        return -1;
    
    printf("%s\n", dir_path);
    struct dirent *folder;
    while ((folder = readdir(dir)) != NULL) {
        printf("%s\n", folder->d_name);
    }

    return 0;
}

int get_files_with_ext(char *dir_path, const char* ext, FILE_VEC* vec) {
    // return a dynamic array of files

    return 0;
}

char *append_str(char* dst, const char* src) {
    int length_dst = strlen(dst);
    int length_src = strlen(src);
    int final_len = length_dst + length_src + 1;

    char *final_string = malloc(final_len);

    if (!final_string) {
        perror("malloc()");
        return NULL;
    }

    memcpy(final_string, dst, length_dst); // copy the dst into the new str
    memcpy(final_string + length_dst, src, length_src); // append the src into the str
    final_string[final_len] = '\0';

    return final_string;
}
