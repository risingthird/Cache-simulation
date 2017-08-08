/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "../part1/utils.h"
#include "hitRate.h"

/*
	Function used to return the hit rate for a cache.
*/
double findHitRate(cache_t* cache) {
	/* Your Code Here. */
	double hitrate = cache->hit/cache->access;
	return hitrate;
}

/*
	Function used to update the cache indicating there has been a cache access.
*/
void reportAccess(cache_t* cache) {
	cache->access++;
	/* Your Code Here. */
}

/*
	Function used to update the cache indicating there has been a cache hit.
*/
void reportHit(cache_t* cache) {
	cache->hit++;
	/* Your Code Here. */
}