#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

#include "lib/utils.h"
#include "lib/processor.h"
#include "lib/component.h"
#include "lib/file_vec.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: process [project_dir]\n");
        return 1;
    }

    char *project_path = argv[1];

    // open the project directory for processing
    DIR *project_dir = open_dir(project_path);

    if (!project_dir) {
        return 1; // fatal
    }
    
    if (project_path[strlen(project_path) - 1] != '/')
        project_path[strlen(project_path)] = '/';

    // find the components dir and read it
    char *component_path = append_str(argv[1], "components");
    DIR *component_dir = open_dir(component_path);

    if (!component_dir) {
        fprintf(stderr, "Could not find %s\n", component_path);
        return 1; // fatal
    }

    FILE_VEC component_files;
    FILE_VEC files_to_process;

    file_vec_init(&component_files);
    file_vec_init(&files_to_process);

    get_files_from_dir(component_path, &component_files);
    // load components, perhaps into an arena?
    load_components_from_vector(&component_files);

    // look for .process files in the project dir
    //TODO: get_files_with_ext(argv[1], ".process", &files_to_process);
    // store file pointers somewhere

    process_file_vector(&files_to_process);
    
    closedir(project_dir);
    closedir(component_dir);
    file_vec_free(&component_files);
    file_vec_free(&files_to_process);
    return 0;
}
