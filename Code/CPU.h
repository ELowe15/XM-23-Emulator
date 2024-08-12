/*Name of file: CPU.h
  Author: Evan Lowe
  Version: 1.3
  This file declares the fetch and decodeNexecute functions
*/
#pragma once

//The following values are indexes withing their respective instruction to identify them
#define BL 0
#define BEQ_BRA 1
#define ADD_ST 2
#define MOVL_MOVH 3
#define LDR4 4
#define LDR5 5
#define STR6 6
#define STR7 7
#define BEQ 0
#define BNE 1
#define BC 2
#define BNC 3
#define BN 4
#define BGE 5
#define BLT 6
#define BRA 7
#define ADD_SUBC 0
#define DADD_AND 1
#define CMP_BIS 2
#define MOV_CLRCC 3
#define CEX 4
#define LD 6
#define ST 7
#define ADD 0
#define ADDC 1
#define SUB 2
#define SUBC 3
#define DADD 4
#define CMP 5
#define XOR 6
#define AND 7
#define OR 8
#define BIT 9
#define BIC 10
#define BIS 11
#define MOV 0
#define SWAP 1
#define SRA_SXT 2
#define SETPRI_CLRCC 3
#define MOVL 0
#define MOVLZ 1
#define MOVLS 2
#define MOVH 3
#define SRA 0
#define RRC 1
#define COMP 2
#define SWPB 3
#define SXT 4
#define SETPRI 0
#define SVC 1
#define SETCC2 2
#define SETCC3 3
#define CLRCC4 4
#define CLRCC5 5
#define SETCC 1
#define CLRCC 0

//CEX conditions
#define EQ 0 
#define NE 1 
#define CS 2 
#define CC 3 
#define MI 4 
#define PL 5 
#define VS 6 
#define VC 7 
#define HI 8 
#define LS 9 
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define TR 14
#define FL 15 

char fetch();
void decodeNExecute();
void bl();
void beq_bra();
void branchOff(int signBit);
void add_st();
void add_bis();
union wordByte addc(unsigned short srcVal, unsigned short dstVal, char carry, char wb);
unsigned short dadd(unsigned short src, unsigned short dst, unsigned char wb);
unsigned short nibAdd(unsigned short nibA, unsigned short nibB, unsigned char* hcarry);
void mov_clrcc();
void setpri_clrcc();
void cex();
void ld_st();
void ld();
void st();
void movl_movh();
void ldr_str();
void ldr();
void str();

