#include "lib/component.h"

int load_components_from_file(char *file_path) {
    printf("loading components from file '%s'\n", file_path);
    return 0;
}

int load_components_from_vector(FILE_VEC *vector) {
    puts("loading components from vector");
    for (size_t i = 0; i < vector->count; ++i) {
	File *file = vector->vec[i];
	printf("file: '%s'\ndir: '%s'\nptr: %p\n", file->name, file->dir, file->ptr);
    }
    return 0;
}
