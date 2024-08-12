/*Name of file: Cache.c
  Author: Evan Lowe
  Version: 1.1
  This file handles access to cache memory in associative, direct access and combined, and bus and updates the cache accordingly.
  Also contains the push and pull functions.
*/
#include <stdio.h>
#include "Cache.h"
#include "Global.h"
#include "Mem.h"

/*Takes the mar, mbr, cr and word / byte and uses associative or direct/combined cache organizations.
Returns hit, true, or miss, false*/
char cache(unsigned short mar, unsigned short* mbr, char cr, char wb)
{	
	// Address of a word must be an even number
	if ((wb == WORD) && (mar % 2 != 0))
	{
		return -1; //return value indicating a illegal address
	}
	char hit;
	if (cacheOrg == ASSOCIATIVE)
	{
		hit = associative(mar, mbr, cr, wb, &cacheMem);
	}
	//Direct or combined
	else
	{
		hit = direct(mar, mbr, cr, wb);
	}
	return hit;
}

/*Searches through the cache to see it the inputted address is a hit or miss.
It then updates the cache accordingly and the returns if it is a hit or miss.
Takes the mar, pointer to the mbr, control reg, word/byte, and a pointer to a cache*/
char associative(unsigned short mar, unsigned short* mbr, char cr, char wb, struct cacheLine* combinedMem)
{

	char hit = FALSE; //True if hit, false if miss
	char cacheAddr = 0;
	char cacheSize;
	unsigned short addr = C0(mar); //Clear bit 0 so it is a even address
	struct cacheLine* currentCache; //Pointer to the cache being accesed
	if (cacheOrg == COMBINED)
	{
		currentCache = combinedMem;
		cacheSize = COMBINEDSIZE;

	}
	else
	{
		currentCache = cacheMem;
		cacheSize = CACHESIZE;
	}
	//Loop through the cache
	for (cacheAddr; cacheAddr < cacheSize; cacheAddr++)
	{
		//If address is in the cache then it is a hit
		if (addr == currentCache[cacheAddr].addr)
		{
			hit = TRUE;
			break;
		}
	}
	if ((hit) && (cr == READ))
	{
		*mbr = readCache(cacheAddr, mar, wb, currentCache);
		updateUsage(cacheAddr, currentCache, cacheSize);
	}
	else if ((hit) && (cr == WRITE))
	{
		writeCache(cacheAddr, mar, mbr, wb, hit, cr, currentCache);
		updateUsage(cacheAddr, currentCache, cacheSize);
	}
	else if (!hit)
	{
		char replaceAddr;
		replaceAddr = LRU(currentCache, cacheSize); //Find least recently used cache line
		if (cr == READ)
		{
			bus(mar, mbr, cr, WORD);
			//printf("mbr %04X\n", *mbr);
			writeCache(replaceAddr, mar, mbr, WORD, hit, cr, currentCache);//Write new data to the cache line with lowest usage
			updateUsage(replaceAddr, currentCache, cacheSize);
			if ((mar % 2) != 0)
			{
				*mbr = MSB(*mbr);
			}
		}
		else
		{
			writeCache(replaceAddr, mar, mbr, wb, hit, cr, currentCache);
			updateUsage(replaceAddr, currentCache, cacheSize);
		}
	}
	return hit;
}

/*Returns least recently used cache line for associative and combined cache.
Takes the current cache being accessed and its cach line size*/
char LRU(struct cacheLine* currentCache, char cacheSize)
{
	char cacheAddr;
	//Find least recently used cache line
	for (cacheAddr = 0; cacheAddr < cacheSize; cacheAddr++)
	{
		//0 usage is the least recently used so this cache address will be returned
		if (currentCache[cacheAddr].usage == 0)
		{
			break;
		}
	}
	return cacheAddr;
}
//Update the usage of all cache lines. The cache line and cache being updated and cache size are parameters
void updateUsage(unsigned short updateAddr, struct cacheLine* currentCache, char cacheSize)
{
	char oldUsage = currentCache[updateAddr].usage; //Compare with other usages
	for (char cacheAddr = 0; cacheAddr < cacheSize; cacheAddr++)
	{
		//Decrement the usage of all cache lines with a lower usage than the one being updated
		if (currentCache[cacheAddr].usage > oldUsage)
		{
			currentCache[cacheAddr].usage -= 1;
		}
	}
	currentCache[updateAddr].usage = cacheSize - 1; //Set updated cache line to most recently used
}

/*Uses a key to determine index into cache. Incorporates combined cache as well
It then updates the cache accordingly and the returns if it is a hit or miss
Takes the mar, pointer to the mbr, control reg, and word/byte*/
char direct(unsigned short mar, unsigned short* mbr, char cr, char wb)
{
	char hit = FALSE;
	if (cacheOrg == COMBINED)
	{
		char cacheNum = combinedKey(mar); //Map to one of the caches
		//Used associative memory on the cache mapped with the key
		switch (cacheNum)
		{
		case 0:
			hit = associative(mar, mbr, cr, wb, &combinedMem0);
			break;
		case 1:
			hit = associative(mar, mbr, cr, wb, &combinedMem1);
			break;
		case 2:
			hit = associative(mar, mbr, cr, wb, &combinedMem2);
			break;
		case 3:
			hit = associative(mar, mbr, cr, wb, &combinedMem3);
			break;
		}
		return hit;
	}
	char cacheAddr = cacheKey(mar); //the key indicates what cacheline it should access
	if (C0(mar) == cacheMem[cacheAddr].addr)
	{
		hit = TRUE;
	}
	if ((hit) && (cr == READ))
	{
		*mbr = readCache(cacheAddr, mar, wb, cacheMem);
	}
	else if ((hit) && (cr == WRITE))
	{
		writeCache(cacheAddr, mar, mbr, wb, hit, cr, cacheMem);
	}
	else if ((!hit) && (cr == READ))
	{
		bus(mar, mbr, cr, WORD);
		writeCache(cacheAddr, mar, mbr, WORD, hit, cr, cacheMem);
	}
	else if ((!hit) && (cr == WRITE))
	{
		writeCache(cacheAddr, mar, mbr, wb, hit, cr, cacheMem);
	}
	return hit;
}

/*Write data to a specific cache line indicated by cacheAddr and currentCache.
Also takes takes value of mar, pointer to mbr, control word/byte and hit/miss as parameters*/
void writeCache(char cacheAddr, unsigned short mar, unsigned short* mbr, char wb, char hit, char rw, struct cacheLine* currentCache)
{
	//miss
	if (!hit)
	{
		unsigned short tempAddr = currentCache[cacheAddr].addr; //previous cache line data used if a dirty bit is set
		unsigned short tempCont = currentCache[cacheAddr].cont.w;
		unsigned char tempDL = currentCache[cacheAddr].dirtyL;
		unsigned char tempDM = currentCache[cacheAddr].dirtyM;
		currentCache[cacheAddr].addr = C0(mar); //word addresses must be even
		if (wb == BYTE)
		{
			//if address is odd mbr contains least significant byte
			if ((mar % 2) == 0)
			{
				currentCache[cacheAddr].cont.b[L] = *mbr;
				currentCache[cacheAddr].cont.b[M] = 0; //Set byte that has not been accesed to 0
				currentCache[cacheAddr].dirtyL = (rw == WRITE); //If a write to memory happened on a miss set the correlated dirty bit
				currentCache[cacheAddr].dirtyM = 0; //Set the dirty bit that has not been accesed to 0
			}
			//if address is even mbr contains most significant byte
			else
			{
				currentCache[cacheAddr].cont.b[M] = *mbr;
				currentCache[cacheAddr].cont.b[L] = 0;
				currentCache[cacheAddr].dirtyM = (rw == WRITE);
				currentCache[cacheAddr].dirtyL = 0;
			}
		}
		else
		{
			currentCache[cacheAddr].cont.w = *mbr;
			currentCache[cacheAddr].dirtyM = (rw == WRITE);
			currentCache[cacheAddr].dirtyL = currentCache[cacheAddr].dirtyM; //Both dirty bits should be the same on a word access
		}
		if (wp == BACK)
		{
			//Write data back to main memory if any dirty bits are set
			if ((tempDL) && (tempDM))
			{
				bus(tempAddr, &tempCont, WRITE, WORD); //Both dirty bits are set so write back entire word
			}
			else if (tempDL)
			{
				tempCont = LSB(tempCont);
				bus(tempAddr, &tempCont, WRITE, BYTE); //Only one dirty bit is set so only write back that byte
			}
			else if (tempDM)
			{
				tempCont = MSB(tempCont);
				tempAddr++;
				bus(tempAddr, &tempCont, WRITE, BYTE);
			}
		}
		//Write through to memory on a write
		else if (rw == WRITE)
		{
			//Odd address is MSB
			if ((mar % 2) != 0)
			{
				tempCont = currentCache[cacheAddr].cont.b[M];
				bus(mar, &tempCont, WRITE, BYTE);
			}
			else
			{
				tempCont = currentCache[cacheAddr].cont.w;
				bus(mar, &tempCont, WRITE, wb);
			}
		}
	}
	//hit
	else
	{
		//Content has been changed
		if (currentCache[cacheAddr].cont.w != *mbr)
		{
			//LSB has changed
			if ((wb == BYTE) && ((mar % 2) == 0) && (currentCache[cacheAddr].cont.b[L] != *mbr))
			{
				currentCache[cacheAddr].cont.b[L] = *mbr;
				if (wp == BACK)
				{
					currentCache[cacheAddr].dirtyL = TRUE;
				}
			}
			else if ((wb == BYTE) && ((mar % 2) != 0) && (currentCache[cacheAddr].cont.b[M] != *mbr))
			{
				currentCache[cacheAddr].cont.b[M] = *mbr;
				if (wp == BACK)
				{
					currentCache[cacheAddr].dirtyM = TRUE;
				}
			}
			else if (wb == WORD)
			{
				currentCache[cacheAddr].cont.w = *mbr;
				if (wp == BACK)
				{
					currentCache[cacheAddr].dirtyL = TRUE;
					currentCache[cacheAddr].dirtyM = TRUE;
				}
			}
			//Write through if data has changed
			if ((wp == THROUGH) && (rw == WRITE))
			{
				unsigned short tempCont = *mbr;
				bus(mar, &tempCont, WRITE, wb);
			}
		}
	}
}

//Takes a cache line index, address and word/byte and cache being accessed and returns the content from that line
unsigned short readCache(char cacheAddr, unsigned short mar, char wb, struct cacheLine* currentCache)
{
	//Even address is the least significant byte
	if ((wb == BYTE) && ((mar % 2) == 0))
	{
		return currentCache[cacheAddr].cont.b[L];
	}
	//Odd address is the most significant byte
	else if (wb == BYTE)
	{
		printf("cache %02X\n", currentCache[cacheAddr].cont.b[M]);
		return currentCache[cacheAddr].cont.b[M];
	}
	//word
	else
	{
		return currentCache[cacheAddr].cont.w;
	}
}

//Reset all cache lines
void resetCache()
{
	//reset main caches
	for (char cacheAddr = 0; cacheAddr < CACHESIZE; cacheAddr++)
	{
		cacheMem[cacheAddr].addr = 0;
		cacheMem[cacheAddr].cont.w = 0;
		cacheMem[cacheAddr].usage = 0;
		cacheMem[cacheAddr].dirtyL = 0;
		cacheMem[cacheAddr].dirtyM = 0;
	}
	//reset combined caches
	for (char cacheAddr = 0; cacheAddr < COMBINEDSIZE; cacheAddr++)
	{
		combinedMem0[cacheAddr].addr = 0;
		combinedMem0[cacheAddr].cont.w = 0;
		combinedMem0[cacheAddr].usage = 0;
		combinedMem0[cacheAddr].dirtyL = 0;
		combinedMem0[cacheAddr].dirtyM = 0;
		combinedMem1[cacheAddr].addr = 0;
		combinedMem1[cacheAddr].cont.w = 0;
		combinedMem1[cacheAddr].usage = 0;
		combinedMem1[cacheAddr].dirtyL = 0;
		combinedMem0[cacheAddr].dirtyM = 0;
		combinedMem2[cacheAddr].addr = 0;
		combinedMem2[cacheAddr].cont.w = 0;
		combinedMem2[cacheAddr].usage = 0;
		combinedMem0[cacheAddr].dirtyL = 0;
		combinedMem0[cacheAddr].dirtyM = 0;
		combinedMem3[cacheAddr].addr = 0;
		combinedMem3[cacheAddr].cont.w = 0;
		combinedMem3[cacheAddr].usage = 0;
		combinedMem0[cacheAddr].dirtyL = 0;
		combinedMem0[cacheAddr].dirtyM = 0;
	}
}

//Display all cache lines
void cacheDisp()
{
	if (cacheOrg != COMBINED)
	{
		for (int cacheAddr = 0; cacheAddr < CACHESIZE; cacheAddr++)
		{
			printf("Addr: 0x%04X, Cont: 0x%04X", cacheMem[cacheAddr].addr, cacheMem[cacheAddr].cont.w);
			//Usage is only used for associative
			if (cacheOrg == ASSOCIATIVE)
			{
				printf(", Usage: %2i", cacheMem[cacheAddr].usage);
			}
			//Dirty bits are only used for write back
			if (wp == BACK)
			{
				printf(", DM: %i, DL: %i", cacheMem[cacheAddr].dirtyM, cacheMem[cacheAddr].dirtyL);
			}
			//print two cache lines side by side then new line
			if ((cacheAddr != 0) && ((cacheAddr % CACHEDISPLEN) == 1))
			{
				printf("\n");
			}
			//Differentiate between two cache lines easily
			else
			{
				printf(" | ");
			}
		}
	}
	else
	{
		struct cacheLine* currentCache;
		//Loop through each cache in combined organization
		for (int cacheNum = 0; cacheNum < (CACHESIZE / COMBINEDSIZE); cacheNum++)
		{
			//print each cache independently
			switch (cacheNum)
			{
			case 0:
				currentCache = &combinedMem0;
				break;
			case 1:
				currentCache = &combinedMem1;
				break;
			case 2:
				currentCache = &combinedMem2;
				break;
			case 3:
				currentCache = &combinedMem3;
				break;
			default:
				currentCache = &combinedMem0;
			}
			//Print data in each cache
			for (int cacheAddr = 0; cacheAddr < COMBINEDSIZE; cacheAddr++)
			{
				printf("Addr: 0x%04X, Cont: 0x%04X, Usage: %i"
					"", currentCache[cacheAddr].addr, currentCache[cacheAddr].cont.w, currentCache[cacheAddr].usage);
				if (wp == BACK)
				{
					printf(", DM: %i, DL: %i", currentCache[cacheAddr].dirtyM, currentCache[cacheAddr].dirtyL);
				}
				//print two cache lines side by side then new line
				if ((cacheAddr != 0) && ((cacheAddr % CACHEDISPLEN) == 1))
				{
					printf("\n");
				}
				//Differentiate between two cache lines easily
				else
				{
					printf(" | ");
				}
			}
			//Print a new line between each cache to differentiate them. do not print new line after the last cache
			if (cacheNum != ((CACHESIZE / COMBINEDSIZE) - 1))
			{
				printf("\n");
			}
		}
	}
}

/*Pulls data from the stack one word at a time. Returns the value being pulled*/
unsigned short pull()
{
	unsigned short data;
	cache(RF[REG][SPINDX].w, &data, READ, WORD);
	RF[REG][SPINDX].w += 2;
	return data;
}

/*Pushes data on to the stack. Takes the data as a parameter*/
void push(unsigned short data)
{
	RF[REG][SPINDX].w -= 2;
	cache(RF[REG][SPINDX].w, &data, WRITE, WORD);
}

//The bus either reads or writes to memory. The parameter are an address, data, read or write and word or byte
void bus(unsigned short mar, unsigned short* mbr, char cr, char wb)
{
	if (cr == READ)
	{
		*mbr = memRead(mar, wb);
	}
	else
	{
		memWrite(mar, *mbr, wb);
	}
	clock += 3;
}