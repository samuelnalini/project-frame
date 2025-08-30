#include "lib/component.h"
#include "lib/utils.h"
#include "lib/parser.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

static char *read_file_into_buf(FILE *fd) {
    size_t file_size = get_file_size(fd);
    char *buf = malloc(file_size + 1);

    if (!buf) {
	perror("Call to malloc() failed");
	return NULL;
    }
    
    size_t bytes_read = fread(buf, 1, file_size, fd);

    if (bytes_read < file_size) {
	fprintf(stderr, "Call to fread() failed\n");
	free(buf);
	return NULL;
    }
    
    buf[file_size] = '\0';
    return buf;
}

int load_components_from_file(File *file) {
    FILE *fd = file->ptr;
    char *buf = read_file_into_buf(fd);

    if (!buf) {
	return -1;
    }

    printf("loading file '%s'\n", file->name);

    // pass the file into the parser
    parse_buffer(buf);
    
    free(buf);
    return 0;
}

int load_components_from_vector(FILE_VEC *vector) {
    puts("loading components from vector");
    for (size_t i = 0; i < vector->count; ++i) {
	File *file = vector->vec[i];
	printf("file: '%s'\ndir: '%s'\nptr: %p\nsize: %zu\n", file->name, file->dir, file->ptr, file->file_size);

	load_components_from_file(file);
    }
    return 0;
}
