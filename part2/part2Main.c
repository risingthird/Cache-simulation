/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "../part1/utils.h"
#include "../part1/cacheRead.h"
#include "../part1/cacheWrite.h"
#include "hitRate.h"

int main() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	uint32_t address;
	char* memFile;
	cache_t* cache;
	wordInfo_t wordVal;
	double access;
	double hit;
	memFile = "testFiles/physicalMemory1.txt";

	//Adapted from Sp16 Midterm 2
	n = 1;
	blockDataSize = 8;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);

	address = 0x61c00080;
	int i, j;
	int total = 0;

	/*Loop 1*/
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			writeWord(cache, address + (((i << 3) + j) << 2), i + j);
		}
	}

	findHitRate(cache);
	access = cache->access;
	hit = cache->hit;

	/*Loop 2*/
	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			wordVal = readWord(cache, address + (((i << 3) + j) << 2));
			total += (int) wordVal.data;
		}
	}

	findHitRate(cache);

	/*Local Hit Rate for Loop 2*/
	cache->access = cache->access - access;
	cache->hit = cache->hit - hit;
	findHitRate(cache);
	deleteCache(cache);
	return 0;
}