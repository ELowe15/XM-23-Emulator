/*Name of file: Global.c
  Author: Evan Lowe
  Version: 1.2
  This file defines the Register File and functions to return the
  most significant bit, print characters, and reset RF
*/
#include <stdio.h>
#include "Global.h"

//Return most significant bit for a value based on if it is a word or byte
unsigned char MSb(unsigned short value, char wb)
{
	unsigned char msb;
	if (!wb)
	{
		msb = B15(value);
	}
	else
	{
		msb = B7(value);
	}
	return msb;
}

//Prints value ASCII character, or a '*' if invalid
void printChar(unsigned short value)
{
	if ((value >= SPACE) && (value <= TILDE))
	{
		printf("%c", value);
	}
	else
	{
		printf("*");
	}
}

//Set all registers to 0
void resetRF()
{
	for (int regIndx = 0; regIndx < REGNUM; regIndx++)
	{
		RF[REG][regIndx].w = 0;
	}
	RF[REG][SPINDX].w = DEFAULTSP;
}

//Default values for the register file
union wordByte RF[2][REGNUM] = { 0, 0, 0, 0, 0, 0, 0, 0, 0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0xFFFF };
struct cacheLine cacheMem[CACHESIZE]; //Cache used for associative and direct
//4 caches used for cobined organization
struct cacheLine combinedMem0[COMBINEDSIZE];
struct cacheLine combinedMem1[COMBINEDSIZE];
struct cacheLine combinedMem2[COMBINEDSIZE];
struct cacheLine combinedMem3[COMBINEDSIZE];
char cacheOrg;
char wp;

char running; //Indicates if program is running
union mem memory;
unsigned short brkPnt; //Breakpoint
unsigned char step; //Indicates if step mode or continous are enabled
unsigned short clock; //CPU clock

union CEXws cexState;

unsigned short prevPriority;
