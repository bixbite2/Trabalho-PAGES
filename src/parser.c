#include "parser.h"

int load_accesses(const char *path, Access *accesses, int max_count) {
    FILE *file = fopen(path, "r");
    if (!file) return -1;
    int i = 0;
    while (i < max_count && fscanf(file, "%*d %15s", accesses[i].page) == 1) {
        accesses[i].type = accesses[i].page[0];
        i++;
    }
    fclose(file);
    return i;
}
