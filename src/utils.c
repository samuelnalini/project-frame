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

FILE *open_file(char *file_path) {
    if (!file_path)
        return NULL;

    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file '%s'\n", file_path);
        return NULL;
    }

    return fp;
}

int get_files_from_dir(char *dir_path, FILE_VEC* vec) {
    // populate file_vec with FILE*

    DIR *dir = open_dir(dir_path);

    if (dir == NULL)
        return -1;

    if (dir_path[strlen(dir_path)] != '/') {
        dir_path = append_str(dir_path, "/");
    }

    //    printf("%s\n", dir_path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        printf("opening %s\n", entry->d_name);
        char *file_path = append_str(dir_path, entry->d_name);

        FILE *file = open_file(file_path);
        file_vec_append(vec, file);
    }

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
