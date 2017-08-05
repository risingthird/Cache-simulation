/* Summer 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "getFromCache.h"
#include "setInCache.h"
#include "cacheRead.h"

/*
	Used when memory cannot be allocated.
*/
void allocationFailed() {
	fprintf(stderr, "\nError: allocation failed\n");
    exit(1);
}

/*
	Used to indicate the cache specs given are invalid.
*/
void invalidCache() {
    fprintf(stderr, "\nError: invalid cache parameters\n");
}

/*
	Used to indicate a physical memory file does not
	exist.
*/
void physicalMemFailed() {
	fprintf(stderr, "\nError: physical memory not found\n");
}

/*
	Creates a new cache with N ways that has a block size of blockDataSize,
	and a total data of size totalDataSize, both in Bytes. Also takes in a string
	which holds the name of a physical memory file and copys it into the
	cache. You CANNOT assume the pointer will remain valid in the function
	without copying. Returns a pointer to the cache. If any error occurs call
	the appropriate error function and return NULL.
*/ 
cache_t* createCache(uint32_t n, uint32_t blockDataSize, uint32_t totalDataSize, char* physicalMemoryName) {
	/* Your Code Here. */
	if (access(physicalMemoryName, F_OK) == -1) {
		physicalMemFailed();
		return NULL;
	}
	/* Your Code Here. */
	return NULL;
}

/*
	Function that frees all of the memory taken up by a cache.
*/
void deleteCache(cache_t* cache) {
	/* Your Code Here. */
}

/*
	Takes in a memory address and the cache it will be written to and
	returns the value of the tag as the rightmost bits with leading
	0s.
*/
uint32_t getTag(cache_t* cache, uint32_t address) {
	uint8_t len = getTagSize(cache);
	return address >> (32 - len);
}

/*
	Takes in a memory address and the cache it will be written to and
	returns the value of the index as the rightmost bits with leading
	0s.
*/
uint32_t getIndex(cache_t* cache, uint32_t address) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a memory address and the cache it will be written to and
	returns the value of the offset as the rightmost bits with leading
	0s.
*/
uint32_t getOffset(cache_t* cache, uint32_t address) {
	/* Your Code Here. */
	return 0;
}

/*
	Returns for a cache the number sets the cache contains.
*/
uint32_t getNumSets(cache_t* cache) {
	/* Your Code Here. */
	return 0;
}

/*
	Given a cache returns the tag size in bits.
*/
uint8_t getTagSize(cache_t* cache) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a cache and determines the number of LRU bits the cache
	needs for each block.
*/
uint8_t numLRUBits(cache_t* cache) {
	/* Your Code Here. */
	return 0;
}

/*
	Returns the total size a block takes up for a cache.
*/
uint64_t totalBlockBits(cache_t* cache) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a cache and returns the space it occupies in bits. 
*/
uint64_t cacheSizeBits(cache_t* cache) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a cache and returns how many bytes it should malloc.
*/
uint64_t cacheSizeBytes(cache_t* cache) {
	uint64_t x = cacheSizeBits(cache);
	if (!(x & 7)) {
		return x >> 3;
	}
	return (x >> 3) + 1;
}

/*
	Calculates the number of garbage bits for the cache. Garbage Bits are
	extra bits that have to be malloced because we are forced to malloc
	a multiple of a byte. This will only be an issue for small caches but
	should always be accounted for.
*/
uint8_t numGarbageBits(cache_t* cache) {
	return (uint8_t) ((8 - (cacheSizeBits(cache) & 7)) & 7);
}

/*
	Takes in a cache and a blocknumber and returns the bit index at which
	the block begins.
*/
uint64_t getBlockStartBits(cache_t* cache, uint32_t blocknumber) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a block number and an address.
	Returns 1 if the tag constructed from the address equals the
	tag in the block specified and otherwise 0.
*/
int tagEquals(uint32_t blockNumber, uint32_t tag, cache_t* cache) {
	return tag == extractTag(cache, blockNumber);
}



/* 
	Prints out the contents of the cache in this format. Each Cache will be
	formatted with 6 columns, the set number the valid bit, the dirty bit, 
	the LRU, the tag, and the data. Data and tag should be printed out in 
	hexadecimal. A horizontal line should begin and end the cache. Every 
	entry should be on a new line. A space and a verticle line should 
	separate each column in each row. A newline space should also be printed
	after each cache.
	EX:

	-----------------------------------------------
	set | valid | dirty | shared | LRU | tag | data
	0 | 1 | 1 | 0 | 0 | 0x6500 | 0x785237895239
	0 | 0 | 1 | 1 | 0 | 0x9568 | 0x87234fd72947
	1 | 1 | 1 | 1 | 0 | 0x7484 | 0x274024729074
	1 | 1 | 1 | 0 | 0 | 0x7481 | 0x736945236858
	-----------------------------------------------

*/
void printCache(cache_t* cache) {
	uint8_t* data;
	uint32_t sets = getNumSets(cache);
	uint32_t iterations = cache->n;
	uint64_t blockDataSize = cache->blockDataSize;
	printf("----------------------------------------------------\n");
	printf("set | valid | dirty | shared | LRU | tag | data\n");
	for (uint64_t i = 0; i < sets * iterations; i++) {
		printf("%ld | ", (i >> log_2(iterations)));
		printf("%d | ", getValid(cache, i));
		printf("%d | ", getDirty(cache, i));
		printf("%d | ", getShared(cache, i));
		printf("%ld | ", getLRU(cache, i));
		printf("0x%x | ", extractTag(cache, i));
		data = fetchBlock(cache, i);
		printf("0x");
		for (uint64_t j = 0; j < blockDataSize; j++) {
			if (data[j] < 16) {
				printf("0");
			}
			printf("%x", data[j]);
		}
		free(data);
		printf("\n");
	}
	printf("----------------------------------------------------\n");
}

/*
	Takes in a number and determines if exactly one bit is turned on. Returns
	1 if there is 1 bit turned on and otherwise 0.
*/
int oneBitOn(uint32_t val) {
	if (val == 0) {
		return 0;
	}
	return (val & (val - 1)) == 0;
}

/*
	Takes in a positive power of two and computes its logartihm base 2.
*/
uint8_t log_2(uint32_t val) {
	return __builtin_ctz(val);
}

/*
	Takes in a cache and a block number and gets the location of the 
	of the valid bit in bits.
*/
uint64_t getValidLocation(cache_t* cache, uint32_t blockNumber) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a cache and a block number and gets the location of the
	dirty bit in bits.
*/
uint64_t getDirtyLocation(cache_t* cache, uint32_t blockNumber) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a cache and a block number and gets the location of the
	shared bit in bits.
*/
uint64_t getSharedLocation(cache_t* cache, uint32_t blockNumber) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a cache and a block number and gets the location of the
	start of the LRU bits in bits.
*/
uint64_t getLRULocation(cache_t* cache, uint32_t blockNumber) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a cache and a block number and gets the location of the
	start of the tag bits in bits.
*/
uint64_t getTagLocation(cache_t* cache, uint32_t blockNumber) {
	/* Your Code Here. */
	return 0;
}

/*
	Takes in a cache, a block number, and an offset in bytes which can be 
	represented in the offset portion of the T:I:O and gets the location
	for the start of data section at that offset in bits.
*/
uint64_t getDataLocation(cache_t* cache, uint32_t blockNumber, uint32_t offset) {
	/* Your Code Here. */
	return 0;
}