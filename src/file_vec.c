#include "lib/file_vec.h"

void file_vec_init(FILE_VEC *vec) {
    vec->count = 0;
    vec->capacity = 0;
    vec->vec = NULL;
    puts("vector zeroed");
}

int file_vec_append(FILE_VEC *vec, FILE *value) {
    if (vec->count + 1 > vec->capacity) {
        uint16_t new_capacity = vec->capacity > 4 ? vec->capacity * 2 : 4;
        FILE **new_vec = realloc(vec->vec, sizeof(FILE*) * new_capacity);

        if (!new_vec) {
            perror("Could not allocate vector space");
            return -1;
        }

        vec->vec = new_vec;
        vec->capacity = new_capacity;
    }

    vec->vec[vec->count++] = value;
    printf("appended to vector: %d\n", vec->count);
    return 0;
}

int file_vec_pop(FILE_VEC *vec) {
    if (vec->count <= 0)
        return -1;

    fclose(vec->vec[vec->count - 1]);
    vec->count--;
    printf("popped from vector: %d\n", vec->count);
}

void file_vec_free(FILE_VEC *vec) {
    for (size_t i = vec->count; i > 0; --i) {
        file_vec_pop(vec);
    }

    free(vec->vec);
    file_vec_init(vec);
    puts("cleaned up vector");
}
