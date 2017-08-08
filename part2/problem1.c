/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "../part1/utils.h"
#include "problem1.h"

/*
	Function used to return the result of the first cache parameters question.
*/
cache_t* params1() {
	uint32_t n = 2;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* fileName;
	fileName = "testFiles/physicalMemory3.txt";
	blockDataSize = 2048;
	totalDataSize = 8192;
	cache_t cache = createCache(n,blockDataSize,totalDataSize,fileName);
	/* Your Code Here. */
	return *cache;
}
