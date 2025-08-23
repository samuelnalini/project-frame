#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

#include "lib/utils.h"
#include "lib/processor.h"
#include "lib/component.h"
#include "lib/file_vec.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: process [project_dir]\n");
        return 1;
    }

    // open the project directory for processing
    DIR* project_dir = open_dir(argv[1]);

    if (!project_dir) {
        return 1; // fatal
    }

    // find the components dir and read it
    char* component_path;
    append_str(argv[1], "/components", component_path);

    DIR* component_dir = open_dir(component_path);

    if (!component_dir) {
        fprintf(stderr, "Could not find 'components'\n");
        return 1; // fatal
    }

    FILE_VEC components;
    FILE_VEC files_to_process;

    file_vec_init(&components);
    file_vec_init(&files_to_process);

    get_files_from_dir(component_dir, &components);
    // load components, perhaps into an arena?
    load_components_from_vector(&components);

    // look for .process files in the project dir
    get_files_with_ext(project_dir, ".process", &files_to_process);
    // store file pointers somewhere

    process_file_vector(&files_to_process);
    
    closedir(project_dir);
    closedir(component_dir);
    file_vec_free(&components);
    file_vec_free(&files_to_process);
    return 0;
}
