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
    Node *current = map->buckets[bucket_index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

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

void hash_map_remove(HashMap *map, const char *key) {
    unsigned long bucket_index = hash_function(key) % HASH_MAP_SIZE;
    Node *current = map->buckets[bucket_index];
    Node *prev = NULL;

    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                map->buckets[bucket_index] = current->next;
            }
            free(current->key);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
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
    HashMap *page_to_frame_index_optimal = create_hash_map();
    char **frames = calloc(frame_count, sizeof(char*));
    int *frame_next_use = calloc(frame_count, sizeof(int));
    int page_faults = 0;
    int frames_used = 0;

    for (int i = 0; i < count; i++) {
        if (i % 1000000 == 0) {
            fprintf(stderr, "Simulating... %.2f%%\r", (double)i * 100.0 / count);
        }

        if (hash_map_get(page_to_frame_index_optimal, accesses[i].page) != -1) { // Page is in memory
            // No need to update next_use here as it's pre-calculated and won't change.
            continue;
        }

        page_faults++;

        if (frames_used < frame_count) { // There is an empty frame
            frames[frames_used] = accesses[i].page;
            frame_next_use[frames_used] = next_use[i];
            hash_map_put(page_to_frame_index_optimal, accesses[i].page, frames_used);
            frames_used++;
            continue;
        }

        // Find the page with the farthest next use
        int farthest_index = 0;
        for (int j = 1; j < frame_count; j++) {
            if (frame_next_use[j] > frame_next_use[farthest_index]) {
                farthest_index = j;
            }
        }

        // Evict the page
        hash_map_remove(page_to_frame_index_optimal, frames[farthest_index]);

        // Place the new page
        frames[farthest_index] = accesses[i].page;
        frame_next_use[farthest_index] = next_use[i];
        hash_map_put(page_to_frame_index_optimal, accesses[i].page, farthest_index);
    }
    fprintf(stderr, "Simulating... 100.00%%\n");

    free(next_use);
    free(frames);
    free(frame_next_use);
    free_hash_map(page_to_frame_index_optimal);

    return page_faults;
}

int working_set(Access *accesses, int count, int frame_count, int tau)
{
    if (frame_count <= 0) return count;

    HashMap *page_to_frame_index = create_hash_map();
    char **frames = calloc(frame_count, sizeof(char*));
    int *last_access = calloc(frame_count, sizeof(int));
    int page_faults = 0;
    int current_time = 0;
    int frames_used = 0;

    for (int i = 0; i < count; i++) {
        current_time++;
        char *current_page = accesses[i].page;

        int frame_index = hash_map_get(page_to_frame_index, current_page);

        if (frame_index != -1) { // Page is in memory
            last_access[frame_index] = current_time;
            continue;
        }

        page_faults++;

        if (frames_used < frame_count) { // There is an empty frame
            frames[frames_used] = strdup(current_page);
            last_access[frames_used] = current_time;
            hash_map_put(page_to_frame_index, current_page, frames_used);
            frames_used++;
            continue;
        }

        // Page replacement
        int evict_index = -1;
        // First, try to find a page outside the working set window
        for (int j = 0; j < frame_count; j++) {
            if (current_time - last_access[j] > tau) {
                evict_index = j;
                break;
            }
        }

        // If no page is outside the window, evict the least recently used (LRU)
        if (evict_index == -1) {
            int lru_time = current_time;
            for (int j = 0; j < frame_count; j++) {
                if (last_access[j] < lru_time) {
                    lru_time = last_access[j];
                    evict_index = j;
                }
            }
        }
        
        // Evict the page
        hash_map_remove(page_to_frame_index, frames[evict_index]);
        free(frames[evict_index]);

        // Place the new page
        frames[evict_index] = strdup(current_page);
        last_access[evict_index] = current_time;
        hash_map_put(page_to_frame_index, current_page, evict_index);
    }

    for (int j = 0; j < frames_used; j++) {
        free(frames[j]);
    }
    free(frames);
    free(last_access);
    free_hash_map(page_to_frame_index);

    return page_faults;
}
