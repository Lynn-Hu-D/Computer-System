#include "cache.h"
#include "message.h"

/* See more details in README
Grading Rubric: https://docs.google.com/spreadsheets/d/1VwPUngwyTjj77Y_s-cDB5Gvu4gK9vcVny4EXtezFcCY/edit?gid=0#gid=0
------------------------------------------------Part2-------------------------------------------------
Caching Strategy and Design:

I chose to implement a direct-mapped cache structure for simplicity in this initial phase.
The cache is an array of CacheEntry structure, where each entry can hold a message, its ID and its last used timestamp, 
and a `next_available` index is used to implement a FIFO (First-In, First-Out) replacement policy initially.

`add_msg_to_cache`: First get the index `next_available`, then store the message in it and update the value of `next_available`

Lookup Data Structure:
The `find_msg_in_cache` function performs a linear scan of the cache entries to find a message by its ID since the cache size is fixed to 16.

Alternatives Considered:
1. Associative Cache with a Hash Map:
   - Design: Instead of a direct array, I could have used a hash map where the key is the message ID and the value is a pointer to the cached message.
   - Why not considered now: While this would provide O(1) average time complexity for lookups, it adds complexity with hash function implementation and collision handling, which might be overkill for this initial part. 
                            A linear scan is sufficient for a small cache size like 16.

2. Using an Array Sorted by ID:
   - Design: The cache array could be kept sorted by message ID.
   - Why not considered now: While it would allow for binary search (O(log N) lookup), maintaining the sorted order during insertions and deletions would add overhead.

For this stage, a simple array with linear search provides a good balance between complexity and performance for the given cache size. In subsequent parts, especially when implementing LRU, the data structure might need to be adjusted.
*/

/**
 * @brief Function to initialize the cache.
 * 
 */
void init_cache() {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache.entries[i].id = -1; // Mark as empty
        cache.entries[i].message = NULL;
        cache.entries[i].last_used = 0;
    }
    cache.next_available = 0;
}

/**
 * @brief Function to find a message in the cache and return the index of it. Return -1 if not in the cache.
 * 
 * @param id The ID of the message.
 * @return int Return the index of the cacheEntry if found in cache else -1.
 */
int find_msg_in_cache(int id) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache.entries[i].id == id && cache.entries[i].message != NULL) {
            cache.entries[i].last_used = time(NULL); // Update last used time
            return i; // Return the index of the cache entry
        }
    }
    return -1; // Message not found in cache
}

/**
 * @brief Function to add a message to the cache and return the index of it in the cache
 * 
 * @param msg The message pointer to be added to the cache
 * @return int Return -1 if not successfully added to the cache else the next available index of CacheEntry
 */
int add_msg_to_cache(Message *msg) {
    if (msg == NULL) {
        return -1;
    }

    // Get the index of CacheEntry to place the current message
    int index_to_replace = cache.next_available;

    // Free existing message if any
    if (cache.entries[index_to_replace].message != NULL) {
        free_msg(cache.entries[index_to_replace].message);
    }

    cache.entries[index_to_replace].id = msg->id;
    cache.entries[index_to_replace].message = msg;
    cache.entries[index_to_replace].last_used = time(NULL);

    cache.next_available = (cache.next_available + 1) % CACHE_SIZE;
    return index_to_replace;
}

/**
 * @brief Stores a message both on disk and in cache.
 * 
 * This function first writes the message to persistent storage using `store_msg()`. 
 * If successful, it then creates a copy of the message and stores it in cache to enable faster retrieval.
 * 
 * @param msg The pointer to the message that needs to be stored.
 * @return int Returns 0 on success, or -1 on failure.
 */
int store_msg_cached(Message *msg) {
    if (msg == NULL) {
        fprintf(stderr, "Error: Cannot store a NULL message.\n");
        return -1;
    }
    if (!store_msg(msg)) {
        fprintf(stderr, "Error: Failed to store message to disk.\n");
        return -1;
    }
    // Create a copy for the cache to avoid potential issues with external modifications
    Message *cache_copy = create_msg(msg->id, msg->sender, msg->receiver, msg->content);
    if (cache_copy) {
        add_msg_to_cache(cache_copy);
    }
    return 0;
}

/**
 * @brief Retrieves a message by first checking cache, then disk if necessary.
 * 
 * This function looks for a message in the cache first. If found, it returns the cached message 
 * and sets `msg_in_cache` to true. If not found, it retrieves the message from disk, adds it 
 * to the cache, and returns it while setting `msg_in_cache` to false.
 * 
 * @param id The unique identifier of the message to retrieve.
 * @param msg_in_cache Pointer to a boolean that will be set to true if the message is found in cache, otherwise false.
 * @return Message* A pointer to the retrieved message. Returns NULL if the message is not found.
 */
Message* retrieve_msg_cached(int id, bool *msg_in_cache) {
    int cache_index = find_msg_in_cache(id);
    if (cache_index != -1) {
        // Cache hits
        *msg_in_cache = true;
        return cache.entries[cache_index].message;
    } else {
        // Cache misses
        *msg_in_cache = false;

        // Not in cache, retrieve from disk
        Message *msg_from_disk = retrieve_msg(id);
        if (msg_from_disk != NULL) {
            // Add msg_from_disk to cache
            Message *cache_copy = create_msg(msg_from_disk->id, msg_from_disk->sender, msg_from_disk->receiver, msg_from_disk->content);
            if (cache_copy) {
                add_msg_to_cache(cache_copy);
            }
        }

        return msg_from_disk;
    }
}

//--------------------------------------------------Part 3------------------------------------------------------//


/**
 * @brief Generates a random index for cache replacement.
 * 
 * This function randomly selects an index within the cache size range. 
 * It is used for implementing a random replacement policy in a cache.
 * 
 * @return int The randomly selected cache index (0 to CACHE_SIZE - 1).
 */
int find_random_replacement_index() {
    return rand() % CACHE_SIZE;
}

/**
 * @brief Finds the least recently used (LRU) index in the cache for replacement.
 * 
 * This function iterates through the cache entries and identifies the index 
 * of the entry with the oldest `last_used` timestamp, following the LRU 
 * (Least Recently Used) replacement policy.
 * 
 * @return int The index of the least recently used cache entry.
 */
int find_lru_replacement_index() {
    int lru_index = 0;
    time_t min_last_used = cache.entries[0].last_used;
    for (int i = 1; i < CACHE_SIZE; i++) {
        if (cache.entries[i].last_used < min_last_used) {
            min_last_used = cache.entries[i].last_used;
            lru_index = i;
        }
    }
    return lru_index;
}

/**
 * @brief Adds a message to the cache using the specified replacement strategy(Radndom or LRU)
 * 
 * This function targets to add a new message to the cache. If an empty slot is available, 
 * it stores the message there. If the cache is full, it replaces an existing message using 
 * either the Least Recently Used (LRU) algorithm or a random replacement strategy.
 * 
 * @param msg Pointer to the message to be added to the cache.
 * @param use_lru If nonzero, the LRU replacement strategy is used; otherwise, random replacement is used.
 * @return int The index where the message was stored in the cache, or -1 if the message is NULL.
 */
int add_msg_to_cache_by_strategy(Message *msg, int use_lru) {
    if (msg == NULL) {
        return -1;
    }
    int index_to_replace = -1;

    // Check for an empty slot first
    int empty_slot = -1;
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache.entries[i].id == -1) {
            empty_slot = i;
            break;
        }
    }

    if (empty_slot != -1) {
        // Found an empty slot, use it
        index_to_replace = empty_slot;
    } else {
        // Cache is full, select a replacement index
        if (use_lru) {
            index_to_replace = find_lru_replacement_index();
        } else {
            index_to_replace = find_random_replacement_index();
        }
    }

    // Free existing message at the chosen index before replacing it
    if (cache.entries[index_to_replace].message != NULL) {
        free_msg(cache.entries[index_to_replace].message);
    }

    // Store the new message in the selected index
    cache.entries[index_to_replace].id = msg->id;
    cache.entries[index_to_replace].message = msg;
    cache.entries[index_to_replace].last_used = time(NULL);

    return index_to_replace;
}

/**
 * @brief Stores a message to disk and caches it using the specified replacement strategy.
 * 
 * This function first attempts to store the message on disk. If successful, it creates a 
 * copy of the message to avoid external modifications and adds it to the cache. 
 * The caching strategy used is either Least Recently Used (LRU) or random replacement.
 * 
 * @param msg Pointer to the message to be stored.
 * @param use_lru If nonzero, the LRU replacement strategy is used; otherwise, random replacement is used.
 * @return int The index where the message was stored in the cache, or -1 if an error occurs.
 */
int store_msg_cached_by_strategy(Message *msg, int use_lru) {
    if (msg == NULL) {
        fprintf(stderr, "Error: Cannot store a NULL message.\n");
        return -1;
    }
    
    // Attempt to store message on disk
    if (!store_msg(msg)) {
        fprintf(stderr, "Error: Failed to store message to disk.\n");
        return -1;
    }

    // Create a copy for caching to prevent external modifications
    Message *cache_copy = create_msg(msg->id, msg->sender, msg->receiver, msg->content);
    if (cache_copy) {
        return add_msg_to_cache_by_strategy(cache_copy, use_lru);
    }

    return 0; // Return 0 if cache addition fails but disk storage succeeds
}

/**
 * @brief Retrieves a message from cache or disk using the specified caching strategy.
 * 
 * This function first attempts to locate the message in the cache. If found, it returns the cached message.
 * Otherwise, it retrieves the message from disk, caches a copy using either Least Recently Used (LRU) 
 * or random replacement strategy, and then returns the retrieved message.
 * 
 * @param id The unique identifier of the message to retrieve.
 * @param msg_in_cache Pointer to a boolean variable that will be set to true if the message is found in cache, false otherwise.
 * @param use_lru If nonzero, the LRU replacement strategy is used; otherwise, random replacement is used.
 * @return Message* Pointer to the retrieved message, or NULL if the message could not be found.
 */
Message* retrieve_msg_cached_by_strategy(int id, bool *msg_in_cache, int use_lru) {
    if (msg_in_cache == NULL) {
        fprintf(stderr, "Error: msg_in_cache pointer is NULL.\n");
        return NULL;
    }

    int cache_index = find_msg_in_cache(id);
    if (cache_index != -1) {
        // Cache hit
        *msg_in_cache = true;
        return cache.entries[cache_index].message;
    } else {
        // Cache miss - retrieve from disk
        Message *msg_from_disk = retrieve_msg(id);
        *msg_in_cache = false;

        if (msg_from_disk != NULL) {
            // Create a copy for caching to avoid external modifications
            Message *cache_copy = create_msg(msg_from_disk->id, msg_from_disk->sender, msg_from_disk->receiver, msg_from_disk->content);
            if (cache_copy) {
                add_msg_to_cache_by_strategy(cache_copy, use_lru);
            }
        }

        return msg_from_disk;
    }
}

/**
 * @brief Frees all allocated messages in the cache and resets cache entries.
 * 
 * This function iterates through the cache and deallocates any stored messages, ensuring 
 * that memory is properly freed before program termination or cache reset.
 */
void free_cache() {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache.entries[i].message != NULL) {
            free_msg(cache.entries[i].message);
            cache.entries[i].message = NULL;
        }
    }
}
