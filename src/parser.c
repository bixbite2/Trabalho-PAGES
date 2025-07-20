#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int load_accesses(const char *path, Access **accesses, int *count) {
    FILE *file = fopen(path, "r");
    if (!file) return -1;

    char page_buffer[MAX_PAGE_NAME];
    int capacity = 10000;
    *count = 0;
    *accesses = malloc(sizeof(Access) * capacity);

    while (fscanf(file, "%s", page_buffer) == 1) {
        if (*count >= capacity) {
            capacity *= 2;
            *accesses = realloc(*accesses, sizeof(Access) * capacity);
        }
        (*accesses)[*count].type = page_buffer[0];
        strcpy((*accesses)[*count].page, page_buffer);
        (*count)++;
    }

    fclose(file);
    return *count;
}
