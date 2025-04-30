#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "message.h"
#include "cache.h"

#define NUM_OF_MSG 200
#define NUM_OF_TRIAL 1000

void test_create_msg() {
    Message* msg0 = create_msg(0, "Alice", "Bob", "Hello, Bob!");
    assert(msg0 != NULL);
    assert(msg0->id == 0);
    assert(strcmp(msg0->sender, "Alice") == 0);
    assert(strcmp(msg0->receiver, "Bob") == 0);
    assert(strcmp(msg0->content, "Hello, Bob!") == 0);
    assert(msg0->delivered == false);
    store_msg(msg0);
    
    Message* msg = create_msg(1, "Alice", "Bob", "Hello, Bob!");
    assert(msg != NULL);
    assert(msg->id == 1);
    assert(strcmp(msg->sender, "Alice") == 0);
    assert(strcmp(msg->receiver, "Bob") == 0);
    assert(strcmp(msg->content, "Hello, Bob!") == 0);
    assert(msg->delivered == false);
    store_msg(msg);

    printf("test_create_msg passed!\n");
    free(msg);
}

void test_store_and_retrieve_msg() {
    Message* msg1 = create_msg(22, "Charlie_1", "David_1", "Test message 1");
    assert(msg1 != NULL);
    
    // Store the message
    bool stored = store_msg(msg1);
    assert(stored == true);
    
    // Retrieve the message
    Message* retrieved = retrieve_msg(22);
    assert(retrieved != NULL);
    assert(retrieved->id == 22);
    assert(strcmp(retrieved->sender, "Charlie_1") == 0);
    assert(strcmp(retrieved->receiver, "David_1") == 0);
    assert(strcmp(retrieved->content, "Test message 1") == 0);

    printf("test_store_and_retrieve_msg passed!\n");

    free(msg1);
    free(retrieved);
}

// use_lru 0->random 1->lru other->FIFO
void random_access_and_metrics(int use_lru) {
    int num_cache_hit = 0;
    int num_cache_miss = 0;
    bool msg_in_cache = false;

    // Simulate 1000 random accesses
    for (int i = 0; i < NUM_OF_TRIAL; i++) {
        int msg_id = rand() % NUM_OF_MSG; // Random message ID

        // Retrieve message from cache based on the caching strategy
        if (use_lru == 0 || use_lru == 1) {
            retrieve_msg_cached_by_strategy(msg_id, &msg_in_cache, use_lru);
        } else {
            retrieve_msg_cached(msg_id, &msg_in_cache);
        }

        // Increment cache hit/miss counters based on the cache status
        if (msg_in_cache) {
            num_cache_hit++;
        } else {
            num_cache_miss++;
        }
    }

    // Output cache statistics
    printf("Number of cache hits per %d random message accesses: %d\n", NUM_OF_TRIAL, num_cache_hit);
    printf("Number of cache misses per %d random message accesses: %d\n", NUM_OF_TRIAL,num_cache_miss);
    printf("Cache hit ratio per %d random message accesses: %f\n", NUM_OF_TRIAL, (float)num_cache_hit / (float)NUM_OF_TRIAL);
}



int main() {
    printf("Part 1 tests start!\n");
    test_create_msg();
    test_store_and_retrieve_msg();
    printf("Part 1 tests end!\n");
    printf("-----------------------------------------\n");

    printf("Part 2 tests start!\n");
    init_cache();

    Message *msg1 = create_msg(101, "Charlie", "David", "Message one.");
    Message *msg2 = create_msg(102, "Eve", "Frank", "Message two.");
    Message *msg3 = create_msg(103, "Ian", "Jason", "Message three.");

    store_msg_cached(msg1);
    store_msg_cached(msg2);
    store_msg(msg3);
    bool actual_bool_variable;
    bool *msg_in_cache = &actual_bool_variable;
    *msg_in_cache = false;
    Message *retrieved1 = retrieve_msg_cached(101, msg_in_cache);
    if (retrieved1) {
        if (*msg_in_cache) {
            printf("Retrieved from cache.\n");
        } else {
            printf("Retrieved from disk.\n");
        }
        printf("Retrieved mgs: ID=%d, Content=%s\n", retrieved1->id, retrieved1->content);
    }

    Message *retrieved2 = retrieve_msg_cached(102, msg_in_cache);
    if (retrieved2) {
        if (*msg_in_cache) {
            printf("Retrieved from cache.\n");
        } else {
            printf("Retrieved from disk.\n");
        }
        printf("Retrieved msg: ID=%d, Content=%s\n", retrieved2->id, retrieved2->content);
    }

    Message *retrieved3 = retrieve_msg_cached(103, msg_in_cache); // Should come from disk (if exists)
    if (retrieved3) {
        if (*msg_in_cache) {
            printf("Retrieved from cache.\n");
        } else {
            printf("Retrieved from disk.\n");
        }
        printf("Retrieved msg: ID=%d, Content=%s\n", retrieved3->id, retrieved3->content);
    } else {
        printf("Message with ID 103 not found.\n");
    }
    free_cache();

    printf("Part 2 tests end!\n");
    printf("-----------------------------------------\n");
    
    printf("Part 4 tests start!\n");
    printf("Num of messages stored: %d\n", NUM_OF_MSG);
    printf("Cache Size: %d\n", CACHE_SIZE);
    printf("\n");
    srand(time(NULL)); // Simulate the random message acess

    //-----------------Metrics for FIFO replacement Algorithm----------------
    init_cache();
    for (int i = 0; i < NUM_OF_MSG; i++) {
        Message *msg = create_msg(i, "placeholder_sender", "placeholder_receiver", "placeholder_content");
        store_msg_cached(msg);
        free_msg(msg);
    }
    printf("Testing %d random access for FIFO replacement cache strategy!\n", NUM_OF_TRIAL);
    random_access_and_metrics(2);
    free_cache();

    //-----------------Metrics for Random replacement Algorithm----------------
    init_cache();
    for (int i = 0; i < NUM_OF_MSG; i++) {
        Message *msg = create_msg(i, "placeholder_sender", "placeholder_receiver", "placeholder_content");
        store_msg_cached_by_strategy(msg, 0);
        free_msg(msg);
    }
    printf("\n");
    printf("Testing %d random access for Random replacement cache strategy!\n", NUM_OF_TRIAL);
    random_access_and_metrics(0);
    free_cache();

    //-----------------Metrics for LRU replacement Algorithm----------------
    init_cache();
    for (int i = 0; i < NUM_OF_MSG; i++) {
        Message *msg = create_msg(i, "placeholder_sender", "placeholder_receiver", "placeholder_content");
        store_msg_cached_by_strategy(msg, 1);
        free_msg(msg);
    }
    printf("\n");
    printf("Testing %d random access for LRU cache strategy!\n", NUM_OF_TRIAL);
    random_access_and_metrics(1);
    free_cache();

    printf("Part 4 tests end!\n");
    printf("-----------------------------------------\n");


    printf("All tests passed!\n");
    return 0;
}