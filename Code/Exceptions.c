/*Name of file: Exceptions.c
  Author: Evan Lowe
  Version: 1
  This file contains the exception function which handles exceptions and calls the appropriate handler
*/

#include <stdio.h>
#include "Exceptions.h"
#include "Global.h"
#include "PSW.h"
#include "Cache.h"

/*Handles exceptions by having the index into the vector table passed as an argument.
The correct vector will be accsessed based on this index. Machine state is stored on the stack.
In the case of a fault the fault name and machine state will be displayed to the user*/
void exception(unsigned char vIndx)
{
	unsigned short VA = VEC_ADDR(vIndx); //Vector address in memory
	union PSW tempPSW;
	cache(VA, &tempPSW.w, READ, WORD); //Read vectors PSW

	/*According to Dr.Hughes assume any process running at priority 7 is impossible to cause a fault.
	However, in the case of a double fault it will be at priority 7 but still needs to execute the double fault handler*/
	if ((tempPSW.bit.current <= psw.bit.current) && (!psw.bit.flt))
	{	
		//Avoid program crashing if the current priority is at MAX_PRI and a priority fault occurs. This should not be possible
		if (tempPSW.bit.current == MAX_PRI)
		{
			printf("ERROR: A fault should never occur when proccess is at priority 7\n");
			VA = VEC_ADDR(PRI_FLT); //Manually change the vector address to priority fault
			cache(VA, &tempPSW.w, READ, WORD); //Read vectors PSW
		}
		else
		{
			exception(PRI_FLT);
			return;  //Do not execute further
		}
	}
	//Handle a fault
	if ((vIndx >= ILL_INST) && (vIndx <= PRI_FLT))
	{
		//Indicates a double fault
		if (psw.bit.flt)
		{
			exception(DBL_FLT);
			return; //Do not execute further
		}
		//Fault type will be printed for user
		switch (vIndx)
		{
		case ILL_INST:
			printf("ILLEGAL INSTRUCTION FAULT has occured\n");
			break;
		case ILL_ADDR:
			printf("ILLEGAL ADDRESS FAULT has occured\n");
			break;
		case PRI_FLT:
			printf("PRIORITY FAULT has occured\n");
			break;
		}
		printf("State before fault:\nPC: 0x%04X\nLR: 0x%04X\nPSW: 0x%04X\nCEX: 0x%04X\n", RF[REG][PCINDX].w, RF[REG][LRINDX].w, psw.w, cexState.w);
		tempPSW.bit.flt = 1; //Indicates a fault 
	}
	//Double fault
	else if (vIndx == DBL_FLT)
	{
		printf("DOUBLE FAULT has occured\n");
		printf("State before fault:\nPC: 0x%04X\nLR: 0x%04X\nPSW: 0x%04X\nCEX: 0x%04X\n", RF[REG][PCINDX].w, RF[REG][LRINDX].w, psw.w, cexState.w);
		tempPSW.bit.flt = 1;
	}
	//SP must be even to access data by word, if a fault already occured, continue . This avoids infinite looping.
	if (((RF[REG][SPINDX].w % 2) != 0) && (!tempPSW.bit.flt))
	{
		printf("Error: SP is odd\n");
		exception(ILL_ADDR);
		return;  //Do not execute further
	}
	push(RF[REG][PCINDX].w);
	push(RF[REG][LRINDX].w);
	push(psw.w);
	push(cexState.w);
	prevPriority = psw.bit.current;
	psw.w = tempPSW.w;
	psw.bit.slp = 0; //Wake up process
	psw.bit.previous = prevPriority;
	cache(VA + 2, &RF[REG][PCINDX].w, READ, WORD); //Read handler address
	RF[REG][LRINDX].w = EXRETURN;
	cexState.w = 0;
}