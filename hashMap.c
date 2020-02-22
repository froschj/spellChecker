/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Jon Frosch
 * Date: 2019-11-20
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement
	assert(map);
	assert(map->table);
	int buckets = map->capacity;
	int i;
	struct HashLink *previousLink = NULL;
	struct HashLink *currentLink = NULL;
	/*
	 * Iterate through each bucket. if the bucket is not null, proceed down
	 * the chain deleting each element before freeing the table
	 */
	for (i = 0; i < buckets; ++i){
		currentLink = map->table[i];
		while (currentLink) {
			previousLink = currentLink;
			currentLink = currentLink->next;
			hashLinkDelete(previousLink);
			previousLink = NULL;
		}
	}
	free(map->table);
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
	assert(map);
	assert(map->table);
	int hashIndex;
	struct HashLink *currentLink = NULL;
	
	/*
	 * Enter the hash table at the hash associated with key
	 */
	hashIndex = HASH_FUNCTION(key);
	hashIndex %= map->capacity;
	if (hashIndex < 0) {
		hashIndex += map->capacity;
	}
	
	currentLink = map->table[hashIndex];
	
	/*
	 * run through the bucket associated with the hash of key until the
	 * bucket ends or the key is found. return a pointer to the value if found
	 */
	while (currentLink) {
		if (!strcmp(currentLink->key, key)) {
			return &(currentLink->value);
		}
		currentLink = currentLink->next;
	}
	
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: implement
	assert(map);
	assert(map->table);
	
	//move existing table into a new map
	struct HashMap *oldMap = malloc(sizeof(struct HashMap));
	oldMap->size = map->size;
	oldMap->capacity = map->capacity;
	oldMap->table = map->table;
	map->table = NULL;
	
	// resize the incoming map to the selected size
	hashMapInit(map, capacity);
	
	struct HashLink *currentLink = NULL;
	
	//copy each key-value pair from the old map to the new map
	for (int i = 0; i < oldMap->capacity; ++i) {
		currentLink = oldMap->table[i];
		while (currentLink) {
			hashMapPut(map, currentLink->key, currentLink->value);
			currentLink = currentLink->next;
		}
	}
	
	hashMapDelete(oldMap);
	
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement
	assert(map);
	assert(map->table);
	
	struct HashLink *newLink = NULL;
	int index;
	
	if (hashMapContainsKey(map, key)) {
		*(hashMapGet(map, key)) = value;
	}
	else {
		if (hashMapTableLoad(map) >= MAX_TABLE_LOAD) {
			resizeTable(map, map->capacity * 2);
		}
		
		index = HASH_FUNCTION(key);
		index %= map->capacity;
		if (index < 0) index += map->capacity;
		
		newLink = hashLinkNew(key, value, map->table[index]);
		map->table[index] = newLink;
		++(map->size);
	}
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement
	assert(map);
	assert(map->table);
	
	//select the bucket
	int index = HASH_FUNCTION(key);
	index %= map->capacity;
	if (index < 0) index += map->capacity;
	
	struct HashLink *currentLink;
	struct HashLink *previousLink;
	
	currentLink = map->table[index];
	previousLink = NULL;
	
	//traverse the chain at the bucket and delete the given link if it exists
	while (currentLink) {
		if(!strcmp(key, currentLink->key)) {
			if(!previousLink) {
				map->table[index] = currentLink->next;
			}
			else {
				previousLink->next = currentLink->next;
			}
			hashLinkDelete(currentLink);
			currentLink = NULL;
			--(map->size);
			return;
		}
		previousLink = currentLink;
		currentLink = currentLink->next;
	}
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement
	assert(map);
	assert(map->table);
	
	// select the correct bucket
	int index = HASH_FUNCTION(key);
	index %= map->capacity;
	if (index < 0) index += map->capacity;
	
	struct HashLink *currentLink;
	currentLink = map->table[index];
	
	// traverse the chain at bucket searching for the given key
	while (currentLink) {
		if (!strcmp(key, currentLink->key)) {
			return 1;
		}
		currentLink = currentLink->next;
	}
	
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
	assert(map);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
	assert(map);
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
	assert(map);
	assert(map->table);
	
	int empties = 0;
	
	for (int i = 0; i < map->capacity; ++i) {
		if (!(map->table[i])) {
			++empties;
		}
	}
	return empties;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
	assert(map);
	float loadFactor;
	loadFactor = (float)(map->size) / (float)(map->capacity);
    return loadFactor;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
  // FIXME: implement
	assert(map);
	assert(map->table);
	
	int i;
	struct HashLink *currentLink;
	
	for (i = 0; i < map->capacity; ++i) {
		currentLink = map->table[i];
		while (currentLink) {
			printf("(%s, %d) ", currentLink->key, currentLink->value);
			currentLink = currentLink->next;
			if (!currentLink) {
				printf("\n");
			}
		}
	}
	
   
}
