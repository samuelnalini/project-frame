#include "lib/file_vec.h"

void file_vec_init(FILE_VEC* vec) {
    vec->count = 0;
    vec->capacity = 0;
    vec->vec = NULL;
}

int file_vec_push(FILE_VEC* vec, FILE* value) {
    if (vec->count + 1 > vec->capacity) {
        uint16_t new_capacity = vec->capacity > 4 ? vec->capacity * 2 : 4;
        FILE** new_vec = realloc(vec->vec, sizeof(FILE*) * new_capacity);

        if (!new_vec) {
            perror("Could not allocate vector space");
            return -1;
        }

        vec->vec = new_vec;
        vec->capacity = new_capacity;
    }

    vec->vec[vec->count++] = value;
    return 0;
}

int file_vec_pop(FILE_VEC* vec) {
    if (vec->count > 0) {
        fclose(vec->vec[vec->count - 1]);
        vec->count--;
    }
    else
        return -1;

/*    if (vec->count < (vec->capacity / 4)) {
        uint16_t new_capacity = vec->capacity / 2;

        if (new_capacity < 4)
            new_capacity = 4;

        DIR** new_vec = realloc(vec->vec, sizeof(DIR*) * new_capacity);

        if (!new_vec) {
            perror("Could not allocate vector space");
            return;
        }

        vec->vec = new_vec;
        vec->capacity = new_capacity;
    }
*/
}

void file_vec_free(FILE_VEC* vec) {
    for (size_t i = 0; i < vec->count; ++i) {
        if (vec->vec[i])
            fclose(vec->vec[i]);
    }

    free(vec->vec);
    file_vec_init(vec);
}
