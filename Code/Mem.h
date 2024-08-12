/*Name of file: Mem.h
  Author: Evan Lowe
  Version: 1.2
  This file contains the function definitions for memwrite() and memRead() and resetMem() and memDisp()
  */
#pragma once

#define DISPLEN 16 //Length of bytes to display on one line with memDisp

void memWrite(unsigned short addr, unsigned short data, unsigned char wb);
unsigned short memRead(unsigned short addr, unsigned char wb);
void resetMem();
void memDisp(unsigned int lower, unsigned int upper);