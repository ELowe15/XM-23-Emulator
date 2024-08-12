/*Name of file: Loader.h
  Author: Evan Lowe
  Version: 1.1
  This file contains the function definition for loader()
  */
#pragma once
#include <stdio.h>

  //These definitions are all in length of nibbles
#define MAXSRECSIZE 74 // 2 type + 2 count + 4 address + 64 data + 2 checksum
#define MINCOUNT 6 // Length of an S-record with not data
#define SSTART 0 // The index for the S character is in the S-record
#define RECTYPESTART 1
#define COUNTSTART 2
#define ADDRSTART 4
#define DATASTART 8

void loader(char* fileName);