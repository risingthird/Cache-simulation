/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "setInCache.h"
#include "cacheRead.h"
#include "cacheWrite.h"
#include "getFromCache.h"
#include "mem.h"
#include "../part2/hitRate.h"

/*
	Takes in a cache and a block number and fetches that block of data, 
	returning it in a uint8_t* pointer.
*/
uint8_t* fetchBlock(cache_t* cache, uint32_t blockNumber) {
	uint64_t location = getDataLocation(cache, blockNumber, 0);
	uint32_t length = cache->blockDataSize;
	uint8_t* data = malloc(sizeof(uint8_t) << log_2(length));
	if (data == NULL) {
		allocationFailed();
	}
	int shiftAmount = location & 7;
	uint64_t byteLoc = location >> 3;
	if (shiftAmount == 0) {
		for (uint32_t i = 0; i < length; i++) {
			data[i] = cache->contents[byteLoc + i];
		}
	} else {
		length = length << 3;
		data[0] = cache->contents[byteLoc] << shiftAmount;
		length -= (8 - shiftAmount);
		int displacement = 1;
		while (length > 7) {
			data[displacement - 1] = data[displacement - 1] | (cache->contents[byteLoc + displacement] >> (8 - shiftAmount));
			data[displacement] = cache->contents[byteLoc + displacement] << shiftAmount;
			displacement++;
			length -= 8;
		}
		data[displacement - 1] = data[displacement - 1] | (cache->contents[byteLoc + displacement] >> (8 - shiftAmount));
	}
	return data;
}

/*
	Takes in a cache, an address, and a dataSize and reads from the cache at
	that address the number of bytes indicated by the size. If the data block 
	is already in the cache it retrieves the contents. If the contents are not
	in the cache it is read into a new slot and if necessary something is 
	evicted.
*/
uint8_t* readFromCache(cache_t* cache, uint32_t address, uint32_t dataSize) {
	/* Your Code Here. */
    // first we need to check the index bits;
    if(!validAddresses(address,dataSize)) return NULL;
    
    uint32_t tag = getTag(cache,address);
    uint32_t offset = getOffset(cache,address);
    uint32_t indexBits = getIndex(cache, address);
    //uint32_t blockend = (indexBits+1)*(cache->n)-1;
    //uint32_t blockstart = (indexBits)*(cache->n);
    //bool match = false;
    //uint32_t i,j,k;
    /*for(i=blockstart;i<=blockend;i++){
        if(getValid(cache,i)&&tagEquals(i,getTag(cache,address),cache)){
            uint8_t* data = getData(cache,offset,i,dataSize);
            return data;
        }
    }*/
    evictionInfo_t* info = findEviction(cache,address);
    // first check whether target is already in cache
    if(info->match){
        uint8_t* data = getData(cache,offset,info->blockNumber,dataSize);
        free(info);
        return data;
    }
    else{
        uint8_t* data = readFromMem(cache,address);
        uint32_t oldTag = extractTag(cache,info->blockNumber);
        evict(cache,info->blockNumber);
        setValid(cache,info->blockNumber,1);
        setDirty(cache,info->blockNumber,1);
        setShared(cache,info->blockNumber,0);
        //setLRU(cache,info->blockNumber,info->LRU);
        setData(cache,data,info->blockNumber,dataSize,offset);
        updateLRU(cache,oldTag,indexBits,info->LRU);
        setTag(cache,tag,info->blockNumber);
        free(info);
        return data;
    }
    free(info);
	return NULL;
}

/*
	Takes in a cache and an address and fetches a byte of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
byteInfo_t readByte(cache_t* cache, uint32_t address) {
	byteInfo_t retVal;
	/* Your Code Here. */
    if(!validAddresses(address,1)) {retVal.success = false;return retVal;}
    //if((address>>1)<<1 != address) retVal.success = false; // aligment error
    //uint32_t dataSize = cace->blockDataSize;
    uint8_t* temp = readFromCache(cache,address,1);
    retVal.data = temp[0];
    retVal.success = true;
    free(temp);
    printf("%u |", retVal.data);
	return retVal;
}

/*
	Takes in a cache and an address and fetches a halfword of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
halfWordInfo_t readHalfWord(cache_t* cache, uint32_t address) {
	halfWordInfo_t retVal;
	/* Your Code Here. */
    if(!validAddresses(address,2) || (address>>1)<<1 !=address) {
        retVal.success = false;return retVal;
    }
    retVal.success = true;
    if(cache->blockDataSize>2){
        uint8_t* temp = readFromCache(cache,address,2);
        retVal.data = temp[0]<<8 +temp[1];
        free(temp);
        return retVal;
    }
    else{
        
    }
    
    return retVal;
}

/*
	Takes in a cache and an address and fetches a word of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
wordInfo_t readWord(cache_t* cache, uint32_t address) {
	wordInfo_t retVal;
	/* Your Code Here. */
	return retVal;
}

/*
	Takes in a cache and an address and fetches a double word of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
doubleWordInfo_t readDoubleWord(cache_t* cache, uint32_t address) {
	doubleWordInfo_t retVal;
	/* Your Code Here. */
	return retVal;
}
