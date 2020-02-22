/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Jon Frosch
 * Date: 2019-11-22
 */

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Iterator to go through every filled entry in a hash table
 */
struct HashMapIterator {
	struct HashMap * map;
	struct HashLink * currentLink;
	struct HashLink * frontSentinel;
	int currentBucket;
};

/*
 * object to hold a key-value pair
 */
struct Association {
	char * key;
	int value;
};

/*
 * creates a new Association wth the given key-value pair
 * @param key
 * @param value
 * @return pointer to allocated struct Association
 */
struct Association * assocNew(char * key, int value) {
	struct Association * temp;
	temp = malloc(sizeof(struct Association));
	assert(temp);
	
	temp->key = malloc(sizeof(char) * (strlen(key) + 1));
	assert(temp->key);
	strcpy(temp->key, key);
	
	temp->value = value;
	
	return temp;
}

/*
 * deallocates a struct Association
 * @param assoc
 */
void assocDestroy(struct Association * assoc) {
	assert(assoc);
	assert(assoc->key);
	free(assoc->key);
	free(assoc);
}

/*
 * initializes a hash map iterator to the given map and sets currentLink to
 * a dummy value whose next pointer points to the first entry in the hash
 * table
 * @param map
 * @param itr
 */
void hashMapItrInit(struct HashMap * map, struct HashMapIterator * itr) {
	assert(map);
	assert(itr);
	
	itr->map = map;
	
	// set up front sentinel so first call to next is not a special case
	if (itr->frontSentinel) { //make reinitializing safe
		free(itr->frontSentinel);
	}
	itr->frontSentinel = malloc(sizeof(struct HashLink));
	assert(itr->frontSentinel);
	
	itr->frontSentinel->key = NULL;
	itr->frontSentinel->value = 0;
	itr->frontSentinel->next = itr->map->table[0];
	
	itr->currentLink = itr->frontSentinel;
	
	itr->currentBucket = 0;
}

/*
 * return a pointer to an initialized hash map iterator for the hash
 * table map
 * @param map
 * @return pointer to allocated iterator
 */
struct HashMapIterator * hashMapItrNew(struct HashMap * map) {
	assert(map);
	
	struct HashMapIterator * temp = NULL;
	temp = malloc(sizeof(struct HashMapIterator));
	assert(temp);
	
	temp->frontSentinel = NULL;  // so init does not try to free it
	
	hashMapItrInit(map, temp);
	
	return temp;
}

/*
 * deallocates a hash map iterator object
 * @param itr
 */
void hashMapItrDestroy(struct HashMapIterator * itr) {
	assert(itr);
	assert(itr->frontSentinel);
	free(itr->frontSentinel);
	free(itr);
}

/*
 * Determine if the map itr is for has a next link
 * @param itr
 * @return 0 if a next entry exists; 0 if it does not
 */
int hashMapItrHasNext(struct HashMapIterator * itr) {
	assert(itr);
	assert(itr->map);
	struct HashLink * temp = itr->currentLink;

	int startBucket;
	
	/*
	 * if the next link exits, return true; otherwise, we have reached the
	 * end of the chain, so proceed to the following buckets until a filled
	 * entry is found (true) or the end of the table is reached (false)
	 */
	if (temp->next) {
		return 1;
	}
	else {
		startBucket = itr->currentBucket + 1;
		for (int i = startBucket; i < itr->map->capacity; ++i) {
			temp = itr->map->table[i];
			if (temp) {
				return 1;
			}
		}
	}
	return 0;
}

/*
 * Return the contents of the next entry in the hash table as a pointer
 * to a new association. must be interleaved with hashMapItrHasNext() to
 * function properly
 * @param itr
 * @return pointer to an allocated struct Association containing the key
 *         and value of the next link in the hash table associated with itr
 */
struct Association * hashMapItrNext(struct HashMapIterator * itr) {
	assert(itr);
	assert(hashMapItrHasNext(itr));
	
	/*
	 * go to the next link in the current bucket, if it exists, return its
	 * key-value pair, otherwise, proceed to following buckets until a filled
	 * one is found and return the key-value pair
	 */
	itr->currentLink = itr->currentLink->next;
	
	if (!(itr->currentLink)) { // end of current chain
		++(itr->currentBucket);
		// try additional chains
		while (itr->currentBucket < itr->map->capacity
			   && !(itr->currentLink)) {
			itr->currentLink = itr->map->table[itr->currentBucket];
			if (!(itr->currentLink)) ++(itr->currentBucket);
		}
	}
	
	//build the return data (copy of info in currentLink)
	struct Association * temp;
	temp = assocNew(itr->currentLink->key, itr->currentLink->value);
	
	return temp;
}

/*
 * calculate and return the levenshtein distance between 2 strings.
 * Based on pseudocode from https://en.wikipedia.org/wiki/Levenshtein_distance#Iterative_with_two_matrix_rows
 * @param string1
 * @param string2
 * @return levenshtein distance between string1 and string2
 */
int levenshtein(char * string1, char * string2) {
	const int NUM_VECTORS = 2;
	int * vectors[NUM_VECTORS];
	int * temp = NULL;
	int length1 = strlen(string1);
	int length2 = strlen(string2);
	int vecLength = length2 + 1;
	int levDist = 0;
	
	int del, ins, sub, min;
	
	// allocate the two vectors to use
	for (int i = 0; i < NUM_VECTORS; ++i) {
		vectors[i] = malloc(sizeof(int) * vecLength);
		assert(vectors[i]);
	}
	
	/*
	 * initialize vector0 to the levenshtien disances of string2's
	 * substrings from a null string
	 */
	for (int i = 0; i < vecLength; ++i) {
		vectors[0][i] = i;
	}
	
	for (int i = 0; i < length1; ++i) {
		/*
		 * calculate vector1 from vector0. vector1 represents the
		 * levenshtein distance between substrings for string1 and substrings
		 * of string2. the results of the previous substring of string1 are
		 * in vector0
		 */
		
		//first element is between substrings of 1 and null string
		vectors[1][0] = i + 1;
		
		//subsequent elements based on needed edit
		for (int j = 0; j < length2; ++j) {
			del = vectors[0][j+1] + 1; //deletion edit
			ins = vectors[1][j] + 1; //insertion edit
			//substituion edit
			if (string1[i] == string2[j]) sub = vectors[0][j];
			else sub = vectors[0][j] + 1;
			
			//determine minimum edit distance
			min = del;
			if (ins < min) min = ins;
			if (sub < min) min = sub;
			vectors[1][j + 1] = min;
		}
		
		/*
		 * swap vetors to preserve vector0 as previous iteration and
		 * vector1 as working space.
		 */
		temp = vectors[0];
		vectors[0] = vectors[1];
		vectors[1] = temp;
		temp = NULL;
	}
	
	// distance between full strings is last element in vector0
	levDist = vectors[0][length2];
	
	for (int i = 0; i < NUM_VECTORS; ++i) {
		free(vectors[i]);
		vectors[i] = NULL;
	}
	
	return levDist;
}

/*
 * iterate through the map and return an array of associations with the
 * smallest levenshtein distances stored in the value field. Map must have
 * the distances loaded in the value field to function correctly
 * @param map
 * @param numSuggestions
 * @return pointer to allocated array of struct Associations of
 *         size numSuggestions
 */
struct Association ** suggest(HashMap *map, int numSuggestions) {
	assert(map);
	assert(numSuggestions <= hashMapSize(map));
	
	struct Association ** outputArray = NULL;
	struct Association * current = NULL;
	struct Association * temp = NULL;
	struct HashMapIterator * itr = NULL;
	itr = hashMapItrNew(map);
	int i;
	
	// set up the return structure
	outputArray = malloc(sizeof(struct Association *) * numSuggestions);
	assert(outputArray);
	for (i = 0; i < numSuggestions; ++i) {
		outputArray[i] = assocNew("", 255); // 255 is biggest buffer in main
	}
	
	/*
	 * Iterate through the hash table. If the value of current is smaller
	 * than an element of the output array, remove the largest value element
	 * from the array. Maintain the array sorted ascending by value
	 */
	while (hashMapItrHasNext(itr)) {
		current = hashMapItrNext(itr);
		i = 0;
		while (i < numSuggestions && !temp) {
			if (current->value < outputArray[i]->value){
				temp = outputArray[numSuggestions - 1];
				for (int j = numSuggestions - 1; j > i; --j) {
					outputArray[j] = outputArray[j - 1];
				}
				outputArray[i] = current;
				current = temp;
			}
			++i;
		}
		temp = NULL;
		assocDestroy(current);
	}
	
	hashMapItrDestroy(itr);
	return outputArray;
}

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
	assert(file);
	assert(map);
	char * currentWord = nextWord(file);
	while (currentWord) {
		hashMapPut(map, currentWord, 0);
		free(currentWord);
		currentWord = nextWord(file);
	}
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
	struct Association * tempAssoc = NULL;
	struct HashMapIterator * hashItr = NULL;
	struct Association ** suggestions = NULL;
	const int NUM_SUGGESTIONS = 5;
	char lowerCaseWord[256];
	
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..
		
		//make check case-insensitive
		strcpy(lowerCaseWord, inputBuffer);
		for (int i = 0; i < strlen(lowerCaseWord); ++i) {
			lowerCaseWord[i] = tolower(lowerCaseWord[i]);
		}
		
		if (hashMapContainsKey(map, lowerCaseWord)) {
			// input is spelled correctly
			printf("The inputted word \"%s\" is spelled correctly\n", inputBuffer);
		}
		else {
			// input is misspelled
			
			// iterate through the dictionary computing levenshtein distances
			hashItr = hashMapItrNew(map);
			while (hashMapItrHasNext(hashItr)) {
				tempAssoc = hashMapItrNext(hashItr);
				tempAssoc->value = levenshtein(lowerCaseWord, tempAssoc->key);
				hashMapPut(map, tempAssoc->key, tempAssoc->value);
				assocDestroy(tempAssoc);
			}
			hashMapItrDestroy(hashItr);
			//hashMapPrint(map);
			
			// generate vector of suggetions
			suggestions = suggest(map, NUM_SUGGESTIONS);
			
			// print the suggestions
			printf("The inputted word \"%s\" is spelled incorrectly\n",
				inputBuffer);
			printf("Did you mean:\n");
			for (int i = 0; i < NUM_SUGGESTIONS; ++i) {
				printf("%s\n", suggestions[i]->key);
				/*printf("%s, %d\n", suggestions[i]->key, suggestions[i]->value);*/
				assocDestroy(suggestions[i]);
			}
			free(suggestions);
		}
		
		strcpy(inputBuffer, lowerCaseWord);
		
		// quit if the user enters the word "quit"
		if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
		
    }

    hashMapDelete(map);
    return 0;
}
