#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "message.h"

#define CACHE_SIZE 16

// Structure for a cache entry
typedef struct {
    int id;
    Message *message;
    time_t last_used; // For LRU
} CacheEntry;

// Cache structure
typedef struct {
    CacheEntry entries[CACHE_SIZE];
    int next_available; // Index of next CacheEntry to be replaced
} MessageCache;

// Global cache instance
MessageCache cache;

// Function to initialize the cache
void init_cache();

// Function to find a message in the cache and return the index of it.
int find_msg_in_cache(int id);

// Function to add a message to the cache (FIFO replacement).
int add_msg_to_cache(Message *msg);

// Modified store_msg function to also store in cache
int store_msg_cached(Message *msg);

// Modified retrieve_msg function to first check cache
Message* retrieve_msg_cached(int id, bool *msg_in_cache);

// Store_msg function to also store in cache by a strategy either random replace or lru
int store_msg_cached_by_strategy(Message *msg, int use_lru);

// Retrieve_msg function with a strategy
Message* retrieve_msg_cached_by_strategy(int id, bool *msg_in_cache, int use_lru);

// Function to free the cache
void free_cache();

#endif
