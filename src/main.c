#include "main.h"
#include "parser.h"
#include "algorithms.h"



int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <access-file> <memory-size> <tau>\n", argv[0]);
        return 1;
    }

    Access *accesses = NULL;
    int count = 0;
    if (load_accesses(argv[1], &accesses, &count) <= 0) {
        printf("Failed to load accesses.\n");
        return 1;
    }

    int frame_count = atoi(argv[2]);
    int tau = atoi(argv[3]);

    printf("Optimal page faults: %d\n", optimal(accesses, count, frame_count));
    printf("Working Set page faults: %d\n", working_set(accesses, count, frame_count, tau));

    free(accesses);
    return 0;
}
