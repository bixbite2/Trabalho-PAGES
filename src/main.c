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

    int frame_count = parse_memory_size(argv[2]);
    int tau = atoi(argv[3]);

    int optimal_faults = optimal(accesses, count, frame_count);
    int ws_faults = working_set(accesses, count, frame_count, tau);

    printf("Optimal page faults: %d\n", optimal_faults);
    printf("Working Set page faults: %d\n", ws_faults);

    if (optimal_faults > 0) {
        double diff = (double)(ws_faults - optimal_faults) * 100.0 / optimal_faults;
        printf("Working Set had %.6f%% more page faults than Optimal.\n", diff);
    }

    free(accesses);
    return 0;
}
