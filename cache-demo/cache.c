/* 211913587 oded didi */
#include "cache.h"
#include <stdlib.h>
#include <stdio.h>

cache_t initialize_cache(uchar s, uchar t, uchar b, uchar E) {
    cache_t cache;
    cache.s = s;
    cache.t = t;
    cache.b = b;
    cache.E = E;
    // Calculate the number of sets (2^s)
    int num_sets = 1 << s; 
    // Calculate the block size (2^b)
    int block_size = 1 << b; 
    // Allocate memory for the sets
    cache.cache = malloc(num_sets * sizeof(cache_line_t*)); 
    if (!cache.cache) {
        printf("failed");
        exit(0);
    }
    for (int i = 0; i < num_sets; i++) {
        // Allocate memory for the lines in each set
        cache.cache[i] = malloc(E * sizeof(cache_line_t)); 
        if (!cache.cache[i]) {
            printf("failed");
            exit(0);
        }
         //Initialize lines
        for (int j = 0; j < E; j++) {
            cache.cache[i][j].valid = 0; 
            cache.cache[i][j].frequency = 0; 
            cache.cache[i][j].tag = 0; 
            // Allocate memory for the block
            cache.cache[i][j].block = malloc(block_size * sizeof(uchar)); 
            if (!cache.cache[i][j].block) {
                printf("failed");
                exit(0);
            }
            // Initialize block data to 0
            for (int k = 0; k < block_size; k++) {
                cache.cache[i][j].block[k] = 0; 
            }
        }
    }
    return cache;
}

uchar read_byte(cache_t cache, uchar* start, long int off) {
    int block_size = 1 << cache.b; 
    int num_sets = 1 << cache.s;

    // Calculate block offset
    int block_off = off & (block_size - 1); 
    // Calculate set index
    int set_index = (off >> cache.b) & (num_sets - 1); 
    // Calculate tag
    long int tag = off >> (cache.b + cache.s); 

    // Check for a HIT
    for (int i = 0; i < cache.E; i++) {
        if (cache.cache[set_index][i].valid && cache.cache[set_index][i].tag == tag) {
            // Increment frequency for LFU
            cache.cache[set_index][i].frequency++; 
            return cache.cache[set_index][i].block[block_off]; 
        }
    }

    // MISS 
    int line_index = 0;
    int min_frequency = cache.cache[set_index][0].frequency;

    // Find the line to replace
    for (int i = 0; i < cache.E; i++) {
        if (!cache.cache[set_index][i].valid) {
            line_index = i; 
            break; 
        }
        // Use LFU to find the line to replace
        if (cache.cache[set_index][i].frequency < min_frequency) {
            line_index = i; 
            min_frequency = cache.cache[set_index][i].frequency;
        }
    }

    // Load the block into the cache
    int block_start = (off / block_size) * block_size; 
    cache.cache[set_index][line_index].valid = 1; 
    cache.cache[set_index][line_index].frequency = 1; 
    cache.cache[set_index][line_index].tag = tag; 
    // Load data from memory
    for (int i = 0; i < block_size; i++) {
        cache.cache[set_index][line_index].block[i] = start[block_start + i]; 
    }

    return cache.cache[set_index][line_index].block[block_off]; // Return the requested byte
}

void write_byte(cache_t cache, uchar* start, long int off, uchar new) {
    int block_size = 1 << cache.b; 
    int num_sets = 1 << cache.s;
   // Calculate block offset
    int block_off = off & (block_size - 1); 
    // Calculate set index
    int set_index = (off >> cache.b) & (num_sets - 1); 
    // Calculate tag
    long int tag = off >> (cache.b + cache.s); 

    // Check if the value is in the cache HIT
    for (int i = 0; i < cache.E; i++) {
        if (cache.cache[set_index][i].valid && cache.cache[set_index][i].tag == tag) {
            cache.cache[set_index][i].block[block_off] = new;
            cache.cache[set_index][i].frequency++;
            break;
        }
    }
    //Update  value in the "memory" (case of miss changing just in memory "no allocate")
    start[off] = new;
}

void print_cache(cache_t cache) {
    int S = 1 << cache.s; // Number of sets
    int B = 1 << cache.b; // Block size

    for (int i = 0; i < S; i++) {
        printf("Set %d\n", i);
        for (int j = 0; j < cache.E; j++) {
            printf("%1d %d 0x%0*lx ", cache.cache[i][j].valid,
                   cache.cache[i][j].frequency, cache.t, cache.cache[i][j].tag);
            for (int k = 0; k < B; k++) {
                printf("%02x ", cache.cache[i][j].block[k]);
            }
            puts("");
        }
    } 
}
//in case we can change main add this func to free all memory
void free_cache(cache_t cache) {
    int num_sets = 1 << cache.s;

    for (int i = 0; i < num_sets; i++) {
        for (int j = 0; j < cache.E; j++) {
            free(cache.cache[i][j].block); // Free each block
        }
        free(cache.cache[i]); // Free each set
    }
    free(cache.cache); // Free the cache
}

int main() {
    int n;
    printf("Size of data: ");
    scanf("%d", &n);
    uchar* mem = malloc(n);
    printf("Input data >> ");
    for (int i = 0; i < n; i++)
        scanf("%hhd", mem + i);

    int s, t, b, E;
    printf("s t b E: ");
    scanf("%d %d %d %d", &s, &t, &b, &E);
    cache_t cache = initialize_cache(s, t, b, E);

    while (1) {
        scanf("%d", &n);
        if (n < 0) break;
        read_byte(cache, mem, n);
    }

    puts("");
    print_cache(cache);

    free(mem);
}
