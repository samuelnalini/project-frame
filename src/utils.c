#include "lib/utils.h"
#include "lib/file_vec.h"
#include "lib/lexer.h"

#include <limits.h>
#include <stdlib.h>

DIR *open_dir(char* dir_path) {
    // opendir(3)
    DIR *dir = opendir(dir_path);

    if (dir == NULL) {
        fprintf(stderr, "Could not open dir '%s'\n", dir_path);
        return NULL;
    }

    return dir;
}

File *open_file(char *file_path, char *dir_path) {
    if (!file_path || !dir_path) {
	fprintf(stderr, "Error opening file '%s' at dir '%s'\n", file_path, dir_path);
        return NULL;
    }

    FILE *fp = fopen(file_path, "r");
    
    if (!fp) {
        fprintf(stderr, "Could not open file '%s'\n", file_path);
        return NULL;
    }

    File *file = malloc(sizeof(File));\

    if (!file) {
	perror("Call to malloc() failed");
	fclose(fp);
	return NULL;
    }

    size_t file_size = get_file_size(fp);

    if (file_size < 0) {
	file_free(file);
	return NULL;
    }

    file->dir = realpath(dir_path, NULL);
    file->name = realpath(file_path, NULL);
    file->ptr = fp;
    file->file_size = file_size;
    
    return file;
}

size_t get_file_size(FILE *fd) {
    if (!fd) return -1;
    
    fseek(fd, 0L, SEEK_END);
    long size = ftell(fd);

    if (size < 0) {
	perror("Call to ftell failed");
	return -1;
    }
    
    rewind(fd);
    
    return (size_t)size;
}

int get_files_from_dir(char *dir_path, FILE_VEC* vec) {
    // populate file_vec with FILE*

    DIR *dir = open_dir(dir_path);

    if (dir == NULL) {
	fprintf(stderr, "Error opening dir '%s'\n", dir_path);
        return -1;
    }

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

        File *file = open_file(file_path, dir_path);
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

char *get_token_type_str(TokenType type) {
    switch(type) {
    case TOKEN_EOF: return "TOKEN_EOF";
    case TOKEN_ERR: return "TOKEN_ERR";
    case TOKEN_UNK: return "TOKEN_UNK";
    case TOKEN_KEYWORD: return "TOKEN_KEYWORD";
    case TOKEN_INT_LIT: return "TOKEN_INT_LIT";
    case TOKEN_STR_LIT: return "TOKEN_STR_LIT";
    case TOKEN_FLOAT_LIT: return "TOKEN_FLOAT_LIT";
    case TOKEN_DOUBLE_LIT: return "TOKEN_DOUBLE_LIT";
    case TOKEN_CHR_LIT: return "TOKEN_CHR_LIT";
    case TOKEN_IDENT: return "TOKEN_IDENT";
    case TOKEN_LSQRLY: return "TOKEN_LSQRLY";
    case TOKEN_RSQRLY: return "TOKEN_RSQRLY";
    case TOKEN_LPAREN: return "TOKEN_LPAREN";
    case TOKEN_RPAREN: return "TOKEN_RPAREN";
    default: return "Unknown Token Type";
    }
}
