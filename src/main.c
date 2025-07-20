#include "main.h"
#include "parser.h"
#include "algorithms.h"

int parse_memory_arg(const char *arg) {
    int size;
    char suffix;
    sscanf(arg, "%d%c", &size, &suffix);
    if (suffix == 'M' || suffix == 'm') return (size * 1024 * 1024) / PAGE_SIZE;
    if (suffix == 'K' || suffix == 'k') return (size * 1024) / PAGE_SIZE;
    return size / PAGE_SIZE;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <access-file> <memory-size>\n", argv[0]);
        return 1;
    }

    Access *accesses = NULL;
    int count = 0;
    if (load_accesses(argv[1], &accesses, &count) <= 0) {
        printf("Failed to load accesses.\n");
        return 1;
    }

    int frame_count = parse_memory_arg(argv[2]);

    printf("Optimal page faults: %d\n", optimal(accesses, count, frame_count));
    printf("Working Set page faults: %d\n", working_set(accesses, frame_count, 10));

    free(accesses);
    return 0;
}
