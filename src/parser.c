#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PAGE_SIZE_KB 4

int parse_memory_size(const char *size_str) {
    long long value = atoll(size_str);
    const char *unit = "";

    for (int i = 0; size_str[i]; i++) {
        if (isalpha(size_str[i])) {
            unit = &size_str[i];
            break;
        }
    }

    if (strcasecmp(unit, "KB") == 0) {
        return value / PAGE_SIZE_KB;
    } else if (strcasecmp(unit, "MB") == 0) {
        return (value * 1024) / PAGE_SIZE_KB;
    } else if (strcasecmp(unit, "GB") == 0) {
        return (value * 1024 * 1024) / PAGE_SIZE_KB;
    }

    return value;
}

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
        strcpy((*accesses)[*count].page, page_buffer);
        (*count)++;
    }

    fclose(file);
    return *count;
}
