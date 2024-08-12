/* Name of file : PSW.c
Author : Evan Lowe and Dr. Hughes
Version : 1.1
This file defines the carry and overflow arrays and the functions updatePSW(), update(PSWZN) and resetPSW()
*/
#include "PSW.h"
#include "Global.h"
#include "Mem.h"

union PSW psw;
unsigned char carry[2][2][2] = { 0, 0, 1, 0, 1, 0, 1, 1 }; //Used to determine if a carry occured
unsigned char overflow[2][2][2] = { 0, 1, 0, 0, 0, 0, 1, 0 };

//Takes a source, destination and result and based on if a word or byte updates the PSW bits accordingly
void updatePSW(unsigned short src, unsigned short dst, unsigned short res, char wb)
{
	unsigned char mss, msd, msr;
	mss = MSb(src, wb);//Most significant bit of source
	msd = MSb(dst, wb);
	msr = MSb(res, wb);
	psw.bit.c = carry[mss][msd][msr];//Set carry bit if carry occured
	updatePSWZN(res, wb);//update zero and negative bits
	psw.bit.v = overflow[mss][msd][msr];
}

//Takes the result, most significant bit of result and sets the zero and negative bits based on a word or byte
void updatePSWZN(unsigned short res, char wb)
{
	if (!wb)
	{
		psw.bit.z = (res == 0);
	}
	else
	{
		psw.bit.z = (LSB(res) == 0);
	}
	psw.bit.n = (MSb(res, wb));
}

//Set PSW to default value
void resetPSW()
{
	psw.w = DEFAULT_PSW;
}