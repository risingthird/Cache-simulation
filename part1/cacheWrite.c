/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "cacheWrite.h"
#include "getFromCache.h"
#include "mem.h"
#include "setInCache.h"
#include "../part2/hitRate.h"

/*
	Takes in a cache and a block number and evicts the block at that number
	from the cache. This does not change any of the bits in the cache but 
	checks if data needs to be written to main memory or and then makes 
	calls to the appropriate functions to do so.
*/
void evict(cache_t* cache, uint32_t blockNumber) {
	uint8_t valid = getValid(cache, blockNumber);
	uint8_t dirty = getDirty(cache, blockNumber);
	if (valid && dirty) {
		uint32_t tag = extractTag(cache, blockNumber);
		uint32_t address = extractAddress(cache, tag, blockNumber, 0);
		writeToMem(cache, blockNumber, address);
	}
}

/*
	Takes in a cache, an address, a pointer to data, and a size of data
	and writes the updated data to the cache. If the data block is already
	in the cache it updates the contents and sets the dirty bit. If the
	contents are not in the cache it is written to a new slot and 
	if necessary something is evicted from the cache.
*/
void writeToCache(cache_t* cache, uint32_t address, uint8_t* data, uint32_t dataSize) {
    /* Your Code Here. */
    reportAccess(cache);
    if(!validAddresses(address,dataSize)) return;
    uint32_t tag = getTag(cache,address);
    uint32_t offset = getOffset(cache,address);
    uint32_t indexBits = getIndex(cache, address);
    evictionInfo_t* info = findEviction(cache,address);
    uint32_t oldTag = extractTag(cache,info->blockNumber);
    if(info->match){
        reportHit(cache);
        writeDataToCache(cache,address,data,dataSize,tag,info);
        free(info);
    }
    else{
        evict(cache,info->blockNumber);
        uint32_t newaddress = address/(cache->blockDataSize)*(cache->blockDataSize);
        uint8_t* data1 = readFromMem(cache, newaddress);
        //uint32_t oldTag = extractTag(cache,info->blockNumber);
        //data1[offset] = data;
        setTag(cache, tag,info->blockNumber);
        //writeDataToCache(cache, address-offset,data1,cache->blockDataSize,oldTag, info);
        writeDataToCache(cache,newaddress,data1,cache->blockDataSize,tag,info);
        setData(cache, data, info->blockNumber, dataSize, offset);
        free(data1);
        //writeDataToCache(cache,address,data,dataSize,tag,info);
        free(info);
    }
}

/*
	Takes in a cache, an address to write to, a pointer containing the data
	to write, the size of the data, a tag, and a pointer to an evictionInfo
	struct and writes the data given to the cache based upon the location
	given by the evictionInfo struct.
*/
void writeDataToCache(cache_t* cache, uint32_t address, uint8_t* data, uint32_t dataSize, uint32_t tag, evictionInfo_t* evictionInfo) {
	uint32_t idx = getIndex(cache, address);
	setData(cache, data, evictionInfo->blockNumber, dataSize , getOffset(cache, address));
	setDirty(cache, evictionInfo->blockNumber, 1);
	setValid(cache, evictionInfo->blockNumber, 1);
	setShared(cache, evictionInfo->blockNumber, 0);
	updateLRU(cache, tag, idx, evictionInfo->LRU);
}

/*
	Takes in a cache, an address, and a byte of data and writes the byte
	of data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns -1
	if the address is invalid, otherwise 0.
*/
int writeByte(cache_t* cache, uint32_t address, uint8_t data) {
	/* Your Code Here. */
    if(!validAddresses(address,1)) return -1;
    uint8_t* written = &data;
    //written[0] = data;
    writeToCache(cache,address,written,1);
    //free(written);
	return 0;
}

/*
	Takes in a cache, an address, and a halfword of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid
	address was used.
*/
int writeHalfWord(cache_t* cache, uint32_t address, uint16_t data) {
	/* Your Code Here. */
    if(!validAddresses(address,2) || (address>>1)<<1 !=address) return -1;
    uint32_t blockDataSize = cache->blockDataSize;
    uint32_t offset = getOffset(cache,address);
    if(blockDataSize>=2){
        uint8_t* written = (uint8_t*) malloc(sizeof(uint8_t)*2);
        written[0] = (uint8_t)(data>>8);
        written[1] = (uint8_t)(data &255);
        writeToCache(cache,address,written,2);
        free(written);
        return 0;
    }
    else{
        uint8_t* written1 = (uint8_t*) malloc(sizeof(uint8_t));
        uint8_t* written2 = (uint8_t*) malloc(sizeof(uint8_t));
        written1[0] = (uint8_t)(data>>8);
        written2[0] = (uint8_t)(data &255);
        writeToCache(cache,address,written1,1);
        writeToCache(cache,address+1,written2,1);
        free(written1);
        free(written2);
        return 0;
    }
	return 0;
}

/*
	Takes in a cache, an address, and a word of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid
	address was used.
*/
int writeWord(cache_t* cache, uint32_t address, uint32_t data) {
	/* Your Code Here. */
    if(!validAddresses(address,4) || (address>>2)<<2 !=address) return -1;
    uint32_t blockDataSize = cache->blockDataSize;
    uint32_t offset = getOffset(cache,address);
    if(blockDataSize>=4){
        uint8_t* written = (uint8_t*) malloc(sizeof(uint8_t)*4);
        written[0] = (uint8_t)(data>>24);
        written[1] = (uint8_t)((data<<8)>>24);
        written[2] = (uint8_t)((data<<16)>>24);
        written[3] = (uint8_t)(data&255);
        writeToCache(cache,address,written,4);
        free(written);
        return 0;
    }
    else if(blockDataSize==2){
        uint16_t data1 = (uint16_t)(data>>16);
        uint16_t data2 = (uint16_t)((data<<16)>>16);
        writeHalfWord(cache,address,data1);
        writeHalfWord(cache,address+2,data2);
        return 0;
    }
    else{
        uint8_t data1 = (uint8_t)(data>>24);
        uint8_t data2 = (uint8_t)((data<<8)>>24);
        uint8_t data3 = (uint8_t)((data<<16)>>24);
        uint8_t data4 = (uint8_t)(data&255);
        writeByte(cache,address,data1);
        writeByte(cache,address+1,data2);
        writeByte(cache,address+2,data3);
        writeByte(cache,address+3,data4);
        return 0;
    }
	return 0;
}

/*
	Takes in a cache, an address, and a double word of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid address
	was used.
*/
int writeDoubleWord(cache_t* cache, uint32_t address, uint64_t data) {
	/* Your Code Here. */
    if(!validAddresses(address,8) || (address>>3)<<3 !=address) return -1;
    uint32_t blockDataSize = cache->blockDataSize;
    if(blockDataSize>=8){
        uint8_t* written = (uint8_t*) malloc(sizeof(uint8_t)*8);
        written[0] = (uint8_t)(data>>56);
        written[1] = (uint8_t)((data<<8)>>56);
        written[2] = (uint8_t)((data<<16)>>56);
        written[3] = (uint8_t)((data<<24)>>56);
        written[4] = (uint8_t)((data<<32)>>56);
        written[5] = (uint8_t)((data<<40)>>56);
        written[6] = (uint8_t)((data<<48)>>56);
        written[7] = (uint8_t)(data&255);
        writeToCache(cache,address,written,8);
        free(written);
        return 0;
    }
    else if(blockDataSize == 4){
        uint32_t data1 = (uint32_t)(data>>32);
        uint32_t data2 = (uint32_t)((data<<32)>>32);
        writeWord(cache,address,data1);
        writeWord(cache,address+4,data1);
        return 0;
    }
    else if(blockDataSize == 2){
        uint16_t data1 = (uint16_t)(data>>48);
        uint16_t data2 = (uint16_t)((data<<16)>>48);
        uint16_t data3 = (uint16_t)((data<<32)>>48);
        uint16_t data4 = (uint16_t)((data<<48)>>48);
        writeHalfWord(cache,address,data1);
        writeHalfWord(cache,address+2,data2);
        writeHalfWord(cache,address+4,data3);
        writeHalfWord(cache,address+6,data4);
        return 0;
    }
    else{
        uint8_t data1 = (uint8_t)(data>>56);
        uint8_t data2 = (uint8_t)((data<<8)>>56);
        uint8_t data3 = (uint8_t)((data<<16)>>56);
        uint8_t data4 = (uint8_t)((data<<24)>>56);
        uint8_t data5 = (uint8_t)((data<<32)>>56);
        uint8_t data6 = (uint8_t)((data<<40)>>56);
        uint8_t data7 = (uint8_t)((data<<48)>>56);
        uint8_t data8 = (uint8_t)(data&255);
        writeByte(cache,address,data1);
        writeByte(cache,address+1,data2);
        writeByte(cache,address+2,data3);
        writeByte(cache,address+3,data4);
        writeByte(cache,address+4,data5);
        writeByte(cache,address+5,data6);
        writeByte(cache,address+6,data7);
        writeByte(cache,address+7,data8);
        return 0;
    }
	return 0;
}

/*
	A function used to write a whole block to a cache without pulling it from
	physical memory. This is useful to transfer information between caches
	without needing to take an intermediate step of going through main memory,
	a primary advantage of MOESI. Takes in a cache to write to, an address
	which is being written to, the block number that the data will be written
	to and an entire block of data from another cache.
*/
void writeWholeBlock(cache_t* cache, uint32_t address, uint32_t evictionBlockNumber, uint8_t* data) {
	uint32_t idx = getIndex(cache, address);
	uint32_t tagVal = getTag(cache, address);
	int oldLRU = getLRU(cache, evictionBlockNumber);
	evict(cache, evictionBlockNumber);
	setValid(cache, evictionBlockNumber, 1);
	setDirty(cache, evictionBlockNumber, 0);
	setTag(cache, tagVal, evictionBlockNumber);
	setData(cache, data, evictionBlockNumber, cache->blockDataSize, 0);
	updateLRU(cache, tagVal, idx, oldLRU);
}
