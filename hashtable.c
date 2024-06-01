#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef HASHTABLE_DEFAULT_CAPACITY
#define HASHTABLE_DEFAULT_CAPACITY 128
#endif

// FNV-1a hash function

#define FNV_PRIME_32 16777619
#define FNV_OFFSET_32 2166136261

__attribute__((always_inline)) static inline size_t fnv1a(const char* key)
{
    const unsigned char* d = (const unsigned char*)key;
    size_t hash = FNV_OFFSET_32;

    for (size_t i = 0; key[i]; i++) {
        hash ^= (size_t)d[i];
        hash *= FNV_PRIME_32;
    }

    return hash;
}

/**
 * Grow the hashtable by doubling its capacity and rehashing all the items
 */
static void hashtable_grow(hashtable_t* self)
{
    size_t new_capacity = self->capacity * 2;
    item_t* new_items = malloc(sizeof(item_t) * new_capacity);

    bzero(new_items, sizeof(item_t) * new_capacity);

    for (size_t i = 0; i < self->capacity; i++) {
        if (self->items[i].key == NULL) {
            continue;
        }

        size_t new_index = self->hash_func(self->items[i].key) % new_capacity;

        new_items[new_index] = self->items[i];
    }

    free(self->items);
    self->items = new_items;
    self->capacity = new_capacity;
}

hashtable_t* hashtable_new(void)
{
    return hashtable_new_with_capacity(HASHTABLE_DEFAULT_CAPACITY);
}

hashtable_t* hashtable_new_with_capacity(size_t initial_capacity)
{
    hashtable_t* self = malloc(sizeof(*self));

    *self = (hashtable_t) {
        .items = malloc(sizeof(item_t) * initial_capacity),
        .capacity = initial_capacity,
        .free_func = NULL,
        .hash_func = &fnv1a,
    };

    bzero(self->items, sizeof(item_t) * initial_capacity);

    return self;
}

__attribute__((always_inline)) inline void hashtable_set_free_func(
    hashtable_t* self, hashtable_free_func_t func)

{
    self->free_func = func;
}

// Whenever we implement hashtable_set_hash_func we need to re-hash the entire
// table

bool hashtable_exists(hashtable_t* self, const char* key)
{
    size_t index = self->hash_func(key) % self->capacity;
    return self->items[index].key != NULL;
}

void* hashtable_get(hashtable_t* self, const char* key)
{
    size_t index = self->hash_func(key) % self->capacity;
    return self->items[index].value;
}

void hashtable_set(hashtable_t* self, const char* key, void* value)
{
    size_t index = self->hash_func(key) % self->capacity;
    item_t* item = &self->items[index];
    // There's already an item here
    if (item->key) {
        // Same key, we're replacing the item
        if (strcmp(item->key, key) == 0) {
            if (item->value && self->free_func) {
                self->free_func(item->value);
            }
            item->value = value;
        }
        // Collision. Same index, different key
        else {
            // Grow the table and try again
            hashtable_grow(self);
            hashtable_set(self, key, value);
            return;
        }
    }
    // Setting a new item
    else {
        item->value = value;
        item->key = strdup(key);
    }
}

bool hashtable_remove(hashtable_t* self, const char* key, void** value)
{
    size_t index = self->hash_func(key) % self->capacity;
    item_t* item = &self->items[index];

    if (item->key == NULL) {
        *value = NULL;
        return false;
    }

    *value = item->value;
    free(item->key);
    item->key = NULL;
    item->value = NULL;
    return true;
}

void hashtable_free(hashtable_t* self)
{
    if (!self)
        return;

    for (size_t i = 0; i < self->capacity; i++) {
        if (self->items[i].key != NULL) {
            if (self->free_func) {
                self->free_func(self->items[i].value);
            }
            free(self->items[i].key);
        }
    }

    free(self->items);
    free(self);
}

bool hashtable_delete(hashtable_t* self, const char* key)
{
    size_t index = self->hash_func(key) % self->capacity;
    item_t* item = &self->items[index];

    if (item->key == NULL) {
        return false;
    }

    if (item->value && self->free_func) {
        self->free_func(item->value);
    }
    free(item->key);
    item->key = NULL;
    item->value = NULL;
    return true;
}
