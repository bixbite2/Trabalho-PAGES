#include "algorithms.h"

int optimal(Access *accesses, int count, int frame_count) {
    char **frames = calloc(frame_count, sizeof(char*));
    int page_faults = 0;

    for (int i = 0; i < count; i++) {
        char *current = accesses[i].page;

        // check if current page is in memory
        int in_memory = 0;
        for (int j = 0; j < frame_count; j++) {
            if (frames[j] && strcmp(frames[j], current) == 0) {
                in_memory = 1;
                break;
            }
        }

        if (in_memory) continue;

        page_faults++;

        // find empty slot
        int replaced = 0;
        for (int j = 0; j < frame_count; j++) {
            if (!frames[j]) {
                frames[j] = strdup(current);
                replaced = 1;
                break;
            }
        }
        if (replaced) continue;

        // compute next use for each page in memory
        int farthest = -1, index_to_replace = -1;
        for (int j = 0; j < frame_count; j++) {
            int k;
            for (k = i + 1; k < count; k++) {
                if (strcmp(frames[j], accesses[k].page) == 0)
                    break;
            }
            if (k > farthest) {
                farthest = k;
                index_to_replace = j;
            }
        }

        free(frames[index_to_replace]);
        frames[index_to_replace] = strdup(current);
    }

    for (int j = 0; j < frame_count; j++)
        free(frames[j]);
    free(frames);

    return page_faults;
}

int working_set(Access *accesses, int frame_count, int tau)
{
    return -1;
}
