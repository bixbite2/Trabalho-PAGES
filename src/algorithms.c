#include "algorithms.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Hash Table Implementation for Optimal Algorithm ---
#define HASH_MAP_SIZE 20000

typedef struct Node {
    char *key;
    int value;
    struct Node *next;
} Node;

typedef struct HashMap {
    Node **buckets;
} HashMap;

unsigned long hash_function(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

HashMap* create_hash_map() {
    HashMap *map = malloc(sizeof(HashMap));
    map->buckets = calloc(HASH_MAP_SIZE, sizeof(Node*));
    return map;
}

void hash_map_put(HashMap *map, const char *key, int value) {
    unsigned long bucket_index = hash_function(key) % HASH_MAP_SIZE;
    Node *newNode = malloc(sizeof(Node));
    newNode->key = strdup(key);
    newNode->value = value;
    newNode->next = map->buckets[bucket_index];
    map->buckets[bucket_index] = newNode;
}

int hash_map_get(HashMap *map, const char *key) {
    unsigned long bucket_index = hash_function(key) % HASH_MAP_SIZE;
    Node *current = map->buckets[bucket_index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;
}

void free_hash_map(HashMap *map) {
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        Node *current = map->buckets[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    free(map->buckets);
    free(map);
}
// --- End of Hash Table Implementation ---

int optimal(Access *accesses, int count, int frame_count) {
    if (frame_count <= 0) return count;

    fprintf(stderr, "Starting pre-computation for Optimal algorithm...\n");

    // Step 1: Find unique pages and map them to an integer index using a hash map.
    HashMap *page_to_index_map = create_hash_map();
    int unique_pages_count = 0;
    for (int i = 0; i < count; i++) {
        if (hash_map_get(page_to_index_map, accesses[i].page) == -1) {
            hash_map_put(page_to_index_map, accesses[i].page, unique_pages_count++);
        }
    }
    fprintf(stderr, "Found %d unique pages.\n", unique_pages_count);

    // Step 2: Compute the next use for each access.
    int *next_use = malloc(count * sizeof(int));
    int *last_occurrence = malloc(unique_pages_count * sizeof(int));
    for (int i = 0; i < unique_pages_count; i++) {
        last_occurrence[i] = -1;
    }

    for (int i = count - 1; i >= 0; i--) {
        if (i % 1000000 == 0) {
            fprintf(stderr, "Preprocessing... %.2f%%\r", (double)(count - i) * 100.0 / count);
        }
        int page_index = hash_map_get(page_to_index_map, accesses[i].page);
        next_use[i] = (last_occurrence[page_index] == -1) ? count : last_occurrence[page_index];
        last_occurrence[page_index] = i;
    }
    fprintf(stderr, "Preprocessing... 100.00%%\n");
    
    free_hash_map(page_to_index_map);
    free(last_occurrence);

    // Step 3: Simulate page replacement.
    char **frames = calloc(frame_count, sizeof(char*));
    int *frame_next_use = calloc(frame_count, sizeof(int));
    int page_faults = 0;

    for (int i = 0; i < count; i++) {
        if (i % 1000000 == 0) {
            fprintf(stderr, "Simulating... %.2f%%\r", (double)i * 100.0 / count);
        }

        int in_memory = 0;
        for (int j = 0; j < frame_count; j++) {
            if (frames[j] && strcmp(frames[j], accesses[i].page) == 0) {
                in_memory = 1;
                frame_next_use[j] = next_use[i];
                break;
            }
        }

        if (in_memory) continue;

        page_faults++;

        int empty_slot = -1;
        for (int j = 0; j < frame_count; j++) {
            if (!frames[j]) {
                empty_slot = j;
                break;
            }
        }

        if (empty_slot != -1) {
            frames[empty_slot] = accesses[i].page;
            frame_next_use[empty_slot] = next_use[i];
            continue;
        }

        int farthest_index = 0;
        for (int j = 1; j < frame_count; j++) {
            if (frame_next_use[j] > frame_next_use[farthest_index]) {
                farthest_index = j;
            }
        }
        frames[farthest_index] = accesses[i].page;
        frame_next_use[farthest_index] = next_use[i];
    }
    fprintf(stderr, "Simulating... 100.00%%\n");

    free(next_use);
    free(frames);
    free(frame_next_use);

    return page_faults;
}

int working_set(Access *accesses, int count, int frame_count, int tau)
{
    char **frames = calloc(frame_count, sizeof(char*));
    int *last_access = calloc(frame_count, sizeof(int));
    int page_faults = 0;
    int current_time = 0;

    for (int i = 0; i < count; i++) {
        current_time++;
        char *current = accesses[i].page;

        int in_memory = 0;
        int frame_index = -1;
        for (int j = 0; j < frame_count; j++) {
            if (frames[j] && strcmp(frames[j], current) == 0) {
                in_memory = 1;
                frame_index = j;
                break;
            }
        }

        if (in_memory) {
            last_access[frame_index] = current_time;
            continue;
        }

        page_faults++;

        int empty_slot = -1;
        for (int j = 0; j < frame_count; j++) {
            if (!frames[j]) {
                empty_slot = j;
                break;
            }
        }

        if (empty_slot != -1) {
            frames[empty_slot] = strdup(current);
            last_access[empty_slot] = current_time;
            continue;
        }

        int lru_index = -1;
        int min_time = current_time + 1;
        for (int j = 0; j < frame_count; j++) {
            if (current_time - last_access[j] > tau) {
                lru_index = j;
                break;
            }
            if (last_access[j] < min_time) {
                min_time = last_access[j];
                lru_index = j;
            }
        }

        free(frames[lru_index]);
        frames[lru_index] = strdup(current);
        last_access[lru_index] = current_time;
    }

    for (int j = 0; j < frame_count; j++)
        free(frames[j]);
    free(frames);
    free(last_access);

    return page_faults;
}
