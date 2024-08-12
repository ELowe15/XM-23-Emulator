/*Name of file: Cache.h
  Author: Evan Lowe
  Version: 1.1
  This file declares cache and push and pull functions and the cache organization definitions
*/
#pragma once

#define ASSOCIATIVE 0 //Indicates associative cache
#define DIRECT 1
#define COMBINED 2
#define CACHEDISPLEN 2 //Cache lines to be displayed in one line
#define CACHEMASK 0x1E //mask bit 1 to 4
#define COMBINEDMASK 0x18 //mask bits 3 and 4
#define cacheKey(x) ((x & CACHEMASK) >> 1) //used to index into the cache
#define combinedKey(x) ((x & COMBINEDMASK) >> 3)//used to map to a spefic cache for combined

char cache(unsigned short mar, unsigned short* mbr, char cr, char wb);
void cacheDisp();
void resetCache();
char associative(unsigned short mar, unsigned short* mbr, char cr, char wb, struct cacheLine* combinedMem);
char direct(unsigned short mar, unsigned short* mbr, char cr, char wb);
char LRU(struct cacheLine* currentCache, char cacheSize);
void updateUsage(unsigned short updateAddr, struct cacheLine* currentCache, char cacheSize);
void writeCache(char cacheAddr, unsigned short mar, unsigned short* mbr, char wb, char hit, char rw, struct cacheLine* currentCache);
unsigned short readCache(char cacheAddr, unsigned short mar, char wb, struct cacheLine* currentCache);
unsigned short pull();
void push(unsigned short data);
void bus(unsigned short mar, unsigned short* mbr, char cr, char wb);
