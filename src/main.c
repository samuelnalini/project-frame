#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc < 1) {
        fprintf(stderr, "Usage: process [project_dir]\n");
        return 1;
    }
}
