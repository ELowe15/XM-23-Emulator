/*Name of file: Exceptions.h
  Author: Evan Lowe
  Version: 1
  This file defines a macro to caluclate a vector address, vector table indexes
  for faults, vector table information and the exception function
*/
#pragma once

#define VEC_ADDR(x) ((x * VECTOR_LEN) + VECTOR_BASE) //Calculates the vectors address

//These define a vectors index into the vector table
#define ILL_INST 8
#define ILL_ADDR 9
#define PRI_FLT 10
#define DBL_FLT 11

#define VECTOR_LEN 4 //Vector is 4 bytes long
#define VECTOR_BASE 0xFFC0 //Vectors reside in this memory location and higher
#define MAX_PRI 7 //Max priority

void exception(unsigned char vIndx);