/*Name of file: PSW.h
  Author: Evan Lowe
  Version: 1.1
  This file defines the PSW bits and functions to update the PSW bits and reset them
*/
#pragma once

#define DEFAULT_PSW 0x60E0; //Default value for the PSW

struct pswBits
{
	unsigned char c : 1;
	unsigned char z : 1;
	unsigned char n : 1;
	unsigned char slp : 1;
	unsigned char v : 1;
	unsigned char current : 3;
	unsigned char flt : 1;
	unsigned char reserved : 4;
	unsigned char previous : 3;
};

union PSW
{
	unsigned short w;
	struct pswBits bit;
};

extern union PSW psw;

void updatePSW(unsigned short src, unsigned short dst, unsigned short res, char wb);
void updatePSWZN(unsigned short res, char wb);
void resetPSW();