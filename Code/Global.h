/*Name of file: Global.h
  Author: Evan Lowe
  Version: 1.5
  This file defines macros to access specific amount of bits, the
  memory length, and various other constant values and declares the
  main and cache memory and register file
*/
//The macros below extract certains bit from a value. Will work on values larger than a word as well
#define B0(x) (x & 0x1) //Extract bit 0
#define B0_2(x) (x & 0x7) //Extract bits 0 to 2
#define B0_3(x) (x & 0xF)
#define B0_9(x) (x & 0x3FF)
#define B0_12(x) (x & 0x1FFF)
#define B1(x) ((x>>1) & (0x1))
#define B2(x) ((x>>2) & (0x1))
#define B3(x) ((x>>3) & (0x1))
#define B3_5(x) ((x>>3) & (0x7))
#define B3_10(x) ((x>>3) & (0xFF))
#define B4(x) ((x>>4) & (0x1))
#define B4_6(x) ((x>>4) & (0x7))
#define B5(x) ((x>>5) & (0x1)) //Extract bit 5
#define B6(x) ((x>>6) & (0x1)) //Extract bit 6
#define B6_9(x) ((x>>6) & (0xF))
#define B7(x) ((x>>7) & (0x1))
#define B8(x) ((x>>8) & (0x1))
#define B7_9(x) ((x>>7) & (0x7))
#define B7_13(x) ((x>>7) & (0x7F))
#define B8_11(x) ((x>>8) & (0xF))
#define B9(x) ((x>>9) & (0x1))
#define B10(x) ((x>>10) & (0x1))
#define B10_12(x) ((x>>10) & (0x7))
#define B11_12(x) ((x>>11) & (0x3))
#define B12(x) ((x>>12) & (0x1))
#define B13_15(x) ((x>>13) & (0x7))
#define B14(x) ((x>>14) & (0x1))
#define B15(x) ((x>>15) & (0x1))
#define LSB(x) (x & 0xFF)//Extract least significant byte
#define MSB(x) ((x >> 8) & (0xFF))//Extract most significant byte

#define S7(x) (x | 0x80)
#define S7_15(x) (x | 0xFF80)
#define S10_15(x) (x | 0xFC00)//Set bits 10 t0 15
#define S13_15(x) (x | 0xE000)
#define S15(x) (x | 0x8000)
#define C0(x) (x & 0xFFFE) //Clear bit 0

#define wMemSz 1<<15
#define bMemSz 1<<16 // 64 KiB
#define FALSE 0
#define TRUE 1
#define WORD 0
#define BYTE 1
#define READ 0
#define WRITE 1
#define SPACE 32 //ASCII value of space
#define TILDE 126 //ASCII value of tilde
#define L 0 //Least significant byte
#define M 1 //Most significant byte
#define REGNUM 8 //Number of registers in the register file
#define REG 0 //Index into the register file with contains the registers
#define CNST 1 //Index into the register file with contains the constants
#define BPINDX 4 ///Index into the register files registers that contains the base pointer
#define LRINDX 5 ///Index into the register files registers that contains the link register
#define SPINDX 6 ///Index into the register files registers that contains the stack pointer
#define DEFAULTSP 0x800 //Default Stack pointer
#define PCINDX 7 ///Index into the register files registers that contains the program counter
#define CTRLC 0 //Used to indicate a ctrl C was used
#define CACHESIZE 16 //Amount of word cache lines
#define COMBINEDSIZE 4 //Amount of cache lines in a combined cache
#define BACK 0 //Write back policy
#define THROUGH 1 //Write through policy

#define EXRETURN 0xFFFF //Value indicating an exception just happened

unsigned char MSb(unsigned short value, char wb);
void printChar(unsigned short value);
void resetRF();


//Memory as words and bytes
union mem
{
	unsigned short wMem[wMemSz];
	unsigned char bMem[bMemSz];
};

//data as word or bytes
union wordByte
{
	unsigned short w;
	unsigned char b[2];
};

//Split a short into its nibbles
struct nibbles
{
	unsigned short n0 : 4;
	unsigned short n1 : 4;
	unsigned short n2 : 4;
	unsigned short n3 : 4;

};

//Define as a word and as niblles
union wordNib
{
	unsigned short word;
	struct nibbles nibs;
};

//The contents of a cache line
struct cacheLine
{
	unsigned short addr;
	union wordByte cont;
	unsigned char usage : 4;
	unsigned char dirtyL : 1; //Least significant byte bit
	unsigned char dirtyM : 1; //Most significant byte bit
};

//The state of a CEX instruction
struct CEXState
{
	unsigned char active : 1;
	unsigned char reserve0 : 3; //Allows each cex state to occupy one nibble and be read easily
	unsigned char true : 1;
	unsigned char reserve1 : 3;
	unsigned char trueCount : 3;
	unsigned char reserve2 : 1;
	unsigned char falseCount : 3;
};

//State of CEX as word and individual state
union CEXws
{
	unsigned short w;
	struct CEXState s;
};

extern union mem memory;
extern union wordByte RF[2][REGNUM]; // Register file
extern struct cacheLine cacheMem[CACHESIZE]; //Declared the cache memory
extern struct cacheLine combinedMem0[COMBINEDSIZE]; //Declared the cache memory
extern struct cacheLine combinedMem1[COMBINEDSIZE]; //Declared the cache memory
extern struct cacheLine combinedMem2[COMBINEDSIZE]; //Declared the cache memory
extern struct cacheLine combinedMem3[COMBINEDSIZE]; //Declared the cache memory
extern char cacheOrg; //the cache organization
extern char wp; //Write policy

extern char running;
extern unsigned short brkPnt; //Breakpoint
extern unsigned char step;
extern unsigned short clock; //CPU clock

extern union CEXws cexState;

extern unsigned short prevPriority; //Previous priority register