/*Name of file: CPU.c
  Author: Evan Lowe
  Version: 1.4
  This file fetches, decodes and executes instructions
*/
#include <stdio.h>
#include "Global.h"
#include "Cache.h"
#include "Mem.h"
#include "PSW.h"
#include "CPU.h"
#include "Exceptions.h"

unsigned short mar; //Memory address register
unsigned short mbr; //Memory buffer megister
char cr; //Control register
unsigned short ir; //Instruction register
char rc; //Register or constant
char wb; //Word or Byte
char src; //Source register
union wordByte srcVal; //Srouce value for decimal values
union wordByte dstVal;
union wordByte res;
char dst;
short offset;
short bPC; //Branch PC
char prpo; //Pre or post increment
char dec;
char inc;
char id; //Increment or decrement
unsigned char byte;

//Fetch instructions from memory
char fetch()
{
	////Return from an exception
	if (RF[REG][PCINDX].w == EXRETURN)
	{
		//SP must be even to access data by word
		if ((RF[REG][SPINDX].w % 2) != 0)
		{
			return -1;
		}
		//Pull state from the stack
		cexState.w = pull();
		psw.w = pull();
		psw.bit.slp = 0;//Wake up process
		RF[REG][LRINDX].w = pull();
		RF[REG][PCINDX].w = pull();	
	}
	char hit;
	mar = RF[REG][PCINDX].w; //Address = PC
	cr = READ;
	hit = cache(mar, &ir, cr, WORD); //Access memory
	if (!hit)
	{
		clock -= 2;//Increment clock by one for the fetch on a miss. clock gets incremented by 3 for the bus call so 3 - 2 = 1
	}
	else if (hit == TRUE)
	{
		clock++;// if a hit then the bus was not used so increment the clock by one

	}
	else
	{
		//Illegal address fault has occured if hit is -1
		return hit;
	}
	RF[REG][PCINDX].w += 2; //Increment program counter
	return hit;
}

//decode instruction
void decodeNExecute()
{
	//Handle a CEX instruction
	if (cexState.s.active)
	{
		//Execute or skip the true instrctions
		if (cexState.s.trueCount)
		{
			cexState.s.trueCount--;
			//Do not execute is CEX condition is false
			if (!cexState.s.true)
			{
				return;
			}
		}
		else if (cexState.s.falseCount)
		{
			cexState.s.falseCount--;
			if (cexState.s.true)
			{
				return;
			}
		}
		//CEX is finished
		else
		{
			cexState.s.active = FALSE;
		}
	}
	//diffirentiate between instructions
	switch (B13_15(ir))
	{
	case BL:
		bl();
		//Break out of the CEX if a branch occurs
		if (cexState.s.active)
		{
			cexState.s.active = FALSE;
		}
		break;
	case BEQ_BRA:
		beq_bra();
		if (cexState.s.active)
		{
			cexState.s.active = FALSE;
		}
		break;
	case ADD_ST:
		add_st();
		break;
	case MOVL_MOVH:
		movl_movh();
		break;
	case LDR4:
	case LDR5:
	case STR6:
	case STR7:
		ldr_str();
		break;
	}
	clock += 2; //Add one to the clock for each decode and execute
}

//Handle a BL
void bl()
{
	offset = B0_12(ir); //Offset is bits 0 t0 12
	branchOff(12);//Calculate branch offset
	RF[REG][LRINDX].w = RF[REG][PCINDX].w;
	RF[REG][PCINDX].w += offset; //PC = PC + offset
}

//Handle instructions from BEQ to BRA
void beq_bra()
{
	offset = B0_9(ir);
	branchOff(9); //Calculate branch offset
	switch (B10_12(ir))
	{
	case BEQ:
		RF[REG][PCINDX].w = (psw.bit.z == 1) ? (RF[REG][PCINDX].w + offset) : RF[REG][PCINDX].w; //branch if a zero is found
		break;
	case BNE:
		RF[REG][PCINDX].w = (psw.bit.z == 0) ? (RF[REG][PCINDX].w + offset) : RF[REG][PCINDX].w; //branch if no zero is found
		break;
	case BC:
		RF[REG][PCINDX].w = (psw.bit.c == 1) ? (RF[REG][PCINDX].w + offset) : RF[REG][PCINDX].w; //branch if carry is found
		break;
	case BNC:
		RF[REG][PCINDX].w = (psw.bit.c == 0) ? (RF[REG][PCINDX].w + offset) : RF[REG][PCINDX].w; //branch if no carry is found
		break;
	case BN:
		RF[REG][PCINDX].w = (psw.bit.n == 1) ? (RF[REG][PCINDX].w + offset) : RF[REG][PCINDX].w; //branch if a negative is found
		break;
	case BGE:
		RF[REG][PCINDX].w = ((psw.bit.n == 0) && (psw.bit.v == 0)) ? (RF[REG][PCINDX].w + offset) : RF[REG][PCINDX].w; //branch if greater than or equal
		break;
	case BLT:
		RF[REG][PCINDX].w = ((psw.bit.n == 1) && (psw.bit.v == 1)) ? (RF[REG][PCINDX].w + offset) : RF[REG][PCINDX].w; //branch if less than
		break;
	case BRA:
		RF[REG][PCINDX].w += offset; //branch to branch offset
		break;
	}
}

//return new offset from a branch instruction. signBit is a parameter indication how many bits the offset is
void branchOff(int signBit)
{
	//Set bits if negative offset
	if ((signBit == 9) && B9(offset) == 1)
	{
		offset = S10_15(offset);//Set bits 10 to 15
	}
	else if ((signBit == 12) && B12(offset) == 1)
	{
		offset = S13_15(offset);
	}
	offset = offset << 1; //make offset even
}

//Handle instructions from ADD to ST
void add_st()
{
	switch (B10_12(ir))
	{
	case ADD_SUBC:
	case DADD_AND:
	case CMP_BIS:
		add_bis();
		break;
	case MOV_CLRCC:
		mov_clrcc();
		break;
	case CEX:
		cex();
		break;
	case LD:
	case ST:
		ld_st();
		break;
	default:
		exception(ILL_INST);
		break;
	}
}

//Handle Instructions from ADD to BIS
void add_bis()
{
	src = B3_5(ir); //Source register
	dst = B0_2(ir); //Destination register
	wb = B6(ir);
	rc = B7(ir);
	srcVal = RF[rc][src];
	dstVal = RF[REG][dst];
	switch (B8_11(ir))
	{
	case ADD:
		//add two values if word or byte
		if (!wb)
		{
			RF[REG][dst].w = addc(srcVal.w, dstVal.w, 0, wb).w;
		}
		else
		{
			RF[REG][dst].b[L] = addc(srcVal.w, dstVal.w, 0, wb).b[L];
		}
		break;
		//add with carry
	case ADDC:
		if (!wb)
		{
			RF[REG][dst].w = addc(srcVal.w, dstVal.w, psw.bit.c, wb).w;
		}
		else
		{
			RF[REG][dst].b[L] = addc(srcVal.w, dstVal.w, psw.bit.c, wb).b[L];
		}
		break;
		//substitute
	case SUB:
		if (!wb)
		{
			RF[REG][dst].w = addc(~srcVal.w, dstVal.w, 1, wb).w;
		}
		else
		{
			RF[REG][dst].b[L] = addc(~srcVal.w, dstVal.w, 1, wb).b[L];
		}
		break;
		//substitute with carry
	case SUBC:
		if (!wb)
		{
			RF[REG][dst].w = addc(~srcVal.w, dstVal.w, psw.bit.c, wb).w;
		}
		else
		{
			RF[REG][dst].b[L] = addc(!srcVal.w, dstVal.w, psw.bit.c, wb).b[L];
		}
		break;
		//decimal addition
	case DADD:
		if (!wb)
		{
			RF[REG][dst].w = dadd(srcVal.w, dstVal.w, wb);
		}
		else
		{
			RF[REG][dst].b[L] = LSB(dadd(srcVal.w, dstVal.w, wb));
		}
		break;
		//Compare
	case CMP:
		addc(~srcVal.w, dstVal.w, 1, wb); // dstVal - srcVal
		break;
	case XOR:
		if (!wb)
		{
			RF[REG][dst].w = srcVal.w ^ dstVal.w + psw.bit.c;
		}
		else
		{
			RF[REG][dst].b[L] = srcVal.b[L] ^ dstVal.b[L] + psw.bit.c;
		}
		updatePSWZN(RF[REG][dst].w, wb);
		break;
	case AND:
		if (!wb)
		{
			RF[REG][dst].w = srcVal.w & dstVal.w + psw.bit.c;
		}
		else
		{
			RF[REG][dst].b[L] = srcVal.b[L] & dstVal.b[L] + psw.bit.c;
		}
		updatePSWZN(RF[REG][dst].w, wb);
		break;
	case OR:
		if (!wb)
		{
			RF[REG][dst].w = srcVal.w | dstVal.w;
		}
		else
		{
			RF[REG][dst].b[L] = srcVal.b[L] | dstVal.b[L];
		}
		updatePSWZN(RF[REG][dst].w, wb);
		break;
		//bit test
	case BIT:
		updatePSWZN((dstVal.w & (1 << srcVal.w)), wb);
		break;
		//bit clear
	case BIC:
		if (!wb)
		{
			RF[REG][dst].w = dstVal.w & ~(1 << srcVal.w);
		}
		else
		{
			RF[REG][dst].b[L] = dstVal.b[L] & ~(1 << srcVal.b[L]);
		}
		updatePSWZN(RF[REG][dst].w, wb);
		break;
		//bit set
	case BIS:
		if (!wb)
		{
			RF[REG][dst].w = dstVal.w | (1 << srcVal.w);
		}
		else
		{
			RF[REG][dst].b[L] = dstVal.b[L] | (1 << srcVal.b[L]);
		}
		updatePSWZN(RF[REG][dst].w, wb);
		break;
	default:
		exception(ILL_INST);
		break;;
	}
}

//Handles all forms of addition, add, sub, cmp etc..., parameters are source and destination values, carry bit, and word or byte
//Returns the result of addition
union wordByte addc(unsigned short srcVal, unsigned short dstVal, char carry, char wb)
{
	if (!wb)
	{
		res.w = srcVal + dstVal + carry;
	}
	else
	{
		res.b[L] = LSB(srcVal) + LSB(dstVal) + carry;
	}
	updatePSW(srcVal + carry, dstVal, res.w, wb);
	return res;
}
//Decimal additions takes a source and destination value and word or byte and returns the result of additon
unsigned short dadd(unsigned short src, unsigned short dst, unsigned char wb)
{
	unsigned char hcarry = psw.bit.c; //Incase a carry occured in previous instruction
	union wordNib srcNibs, dstNibs, resNibs;
	srcNibs.word = src;
	dstNibs.word = dst;
	resNibs.nibs.n0 = nibAdd(srcNibs.nibs.n0, dstNibs.nibs.n0, &hcarry); //Perform decimal additon on least significant nibble
	resNibs.nibs.n1 = nibAdd(srcNibs.nibs.n1, dstNibs.nibs.n1, &hcarry);
	//If byte addition stop adding after second nibble
	if (wb)
	{
		psw.bit.c = hcarry; //Set the carry bit
		updatePSWZN(resNibs.word, wb);
		return resNibs.word;
	}
	//Continue addition if a word
	resNibs.nibs.n2 = nibAdd(srcNibs.nibs.n2, dstNibs.nibs.n2, &hcarry);
	resNibs.nibs.n3 = nibAdd(srcNibs.nibs.n3, dstNibs.nibs.n3, &hcarry);
	psw.bit.c = hcarry;
	updatePSWZN(resNibs.word, wb);
	return resNibs.word;
}

//Adds nibbles of decimal numbers, takes two nibble to add and a pointer to the half carry
//Return the result of nibble addition and sets the half carry
unsigned short nibAdd(unsigned short nibA, unsigned short nibB, unsigned char* hcarry)
{
	unsigned short resNib;
	resNib = nibA + nibB + *hcarry;
	//Decimal numbers need to be below ten so half carry is set if greater than 9
	if (resNib > 9)
	{
		*hcarry = 1;
		resNib -= 10;
	}
	else
	{
		*hcarry = 0;
	}
	return resNib;
}

//Handle instructions MOV to CLRCC
void mov_clrcc()
{
	dst = B0_2(ir);
	src = B3_5(ir);
	wb = B6(ir);
	srcVal = RF[REG][src];
	switch (B7_9(ir))
	{
		//Move source value to destination register
	case MOV:
		if (!wb)
		{
			res.w = srcVal.w;
			RF[REG][dst].w = res.w;
		}
		else
		{
			res.b[L] = srcVal.b[L];
			RF[REG][dst].b[L] = res.b[L];
		}
		updatePSWZN(RF[REG][dst].w, wb);
		break;
		//Swap source and destination registers
	case SWAP:
		RF[REG][src].w = RF[REG][dst].w;
		RF[REG][dst].w = srcVal.w;
		updatePSWZN(RF[REG][dst].w, wb);
		break;
	case SRA_SXT:
		dstVal.w = RF[REG][dst].w;
		switch (B3_5(ir))
		{
			//Shift destination right
		case SRA:
			if (!wb)
			{
				RF[REG][dst].w = dstVal.w >> 1;
			}
			else
			{
				RF[REG][dst].b[L] = dstVal.b[L] >> 1;
			}
			break;
			//Rotate right through carry
		case RRC:
			if (!wb)
			{
				RF[REG][dst].w = dstVal.w >> 1;
				//Set the most significant bit to the carry
				if (psw.bit.c)
				{
					RF[REG][dst].w = S15(RF[REG][dst].w);
				}
			}
			else
			{
				RF[REG][dst].b[L] = dstVal.b[L] >> 1;
				if (psw.bit.c)
				{
					RF[REG][dst].b[L] = S7(RF[REG][dst].w);
				}
			}
			break;
			//Ones complement of destination
		case COMP:
			if (!wb)
			{
				RF[REG][dst].w = ~dstVal.w;
			}
			else
			{
				RF[REG][dst].b[L] = ~dstVal.b[L];
			}
			break;
			//Swap least and most significant bytes
		case SWPB:
			RF[REG][dst].b[L] = dstVal.b[M];
			RF[REG][dst].b[M] = dstVal.b[L];
			break;
			//Sign extend LSB
		case SXT:
			if (MSb(dstVal.w, BYTE))
			{
				RF[REG][dst].b[M] = 0xFF;
			}
			else
			{
				RF[REG][dst].b[M] = 0;
			}
			break;
		default:
			exception(ILL_INST);
			return; //Do not continue execution
		}
		updatePSWZN(RF[REG][dst].w, wb);
		break;
	case SETPRI_CLRCC:
		setpri_clrcc();
		break;
	default:
		exception(ILL_INST);
		break;
	}
}

//Handle instructions from setpri to clrcc
void setpri_clrcc()
{
	switch (B4_6(ir))
	{
	case SETPRI:
		;//Extract the new priority
		unsigned char newPriority = B0_2(ir);
		//New priority must be lower than current
		if (newPriority >= psw.bit.current)
		{
			exception(PRI_FLT);
		}
		else
		{
			psw.bit.current = newPriority;
		}
		break;
	case SVC:
		//Execute trap
		exception(B0_3(ir));
		break;
	case SETCC2:
	case SETCC3:
	case CLRCC4:
	case CLRCC5:
		;//Extract bits
		char v = B4(ir);
		char slp = B3(ir);
		char n = B2(ir);
		char z = B1(ir);
		char c = B0(ir);
		switch (B5(ir))
		{
		case SETCC:
			//Single lined if statements used to make code more readable and condense
			if (v) psw.bit.v = 1; //Set psw.bit.v
			if ((slp) && (psw.bit.current != 7)) psw.bit.slp = 1;
			else if ((slp) && (psw.bit.current == 7)) printf("Cannot go to sleep at priority 7\n");
			if (n) psw.bit.n = 1;
			if (z) psw.bit.z = 1;
			if (c) psw.bit.c = 1;
			break;
		case CLRCC:
			if (v) psw.bit.v = 0;
			if (slp) psw.bit.slp = 0;
			if (n) psw.bit.n = 0;
			if (z) psw.bit.z = 0;
			if (c) psw.bit.c = 0;
			break;
		}
		break;
	default:
		exception(ILL_INST);
		break;
	}
	
}

//Execute CEX instruction
void cex()
{
	char cond = B6_9(ir);
	cexState.s.trueCount = B3_5(ir);
	cexState.s.falseCount = B0_2(ir);
	switch (cond)
	{
	case EQ:
		cexState.s.true = (psw.bit.z == 1);
		break;
	case NE:
		cexState.s.true = (psw.bit.z == 0);
		break;
	case CS:
		cexState.s.true = (psw.bit.c == 1);
		break;
	case CC:
		cexState.s.true = (psw.bit.c == 0);
		break;
	case MI:
		cexState.s.true = (psw.bit.n == 1);
		break;
	case PL:
		cexState.s.true = (psw.bit.n == 0);
		break;
	case VS:
		cexState.s.true = (psw.bit.v == 1);
		break;
	case VC:
		cexState.s.true = (psw.bit.v == 0);
		break;
	case HI:
		cexState.s.true = ((psw.bit.c == 1) && (psw.bit.z == 0));
		break;
	case LS:
		cexState.s.true = ((psw.bit.c == 0) || (psw.bit.z == 1));
		break;
	case GE:
		cexState.s.true = (psw.bit.n == psw.bit.v);
		break;
	case LT:
		cexState.s.true = (psw.bit.n != psw.bit.v);
		break;
	case GT:
		cexState.s.true = ((psw.bit.z == 0) && (psw.bit.n == psw.bit.v));
		break;
	case LE:
		cexState.s.true = ((psw.bit.z == 1) || (psw.bit.n != psw.bit.v));
		break;
	case TR:
		cexState.s.true = TRUE;
		break;
	case FL:
		cexState.s.true = FALSE;
		break;
	}
	cexState.s.active = TRUE; //CEX instruction is active
}

//Handles LD and ST instructions
void ld_st()
{
	dst = B0_2(ir);
	src = B3_5(ir);
	prpo = B9(ir);
	dec = B8(ir);
	inc = B7(ir);
	wb = B6(ir);
	id = 0;
	//Decrement or increment value
	if (dec)
	{
		id = (wb == 0) ? -2 : -1; //Decrement 2 if word and 1 if byte
	}
	if (inc)
	{
		id = (wb == 0) ? 2 : 1;
	}
	if (!B10(ir))
	{
		ld();
	}
	else
	{
		st();
	}
}

// Load data from memory address
void ld()
{
	char fault;
	mar = RF[REG][src].w;
	cr = READ;
	if (prpo)
	{
		mar = mar + id; //pre increment.decrement
		fault = cache(mar, &mbr, cr, wb);
	}
	else
	{
		fault = cache(mar, &mbr, cr, wb);
		mar = mar + id;
	}
	//Do not continue executing this instruction on a fault
	if (fault == -1)
	{
		exception(ILL_ADDR);
		return;
	}
	RF[REG][src].w = mar;
	if (!wb)
	{
		res.w = mbr;
		RF[REG][dst].w = res.w;
	}
	else
	{
		res.b[L] = mbr;
		RF[REG][dst].b[L] = res.b[L];
	}
	updatePSWZN(res.w, wb);
}

// store data in memory address
void st()
{
	char fault;
	mar = RF[REG][dst].w;
	cr = WRITE;
	if (!wb)
	{
		mbr = RF[REG][src].w;
	}
	else
	{
		mbr = RF[REG][src].b[L];
	}
	if (prpo)
	{
		mar = mar + id;
		fault = cache(mar, &mbr, cr, wb);
	}
	else
	{
		fault = cache(mar, &mbr, cr, wb);
		mar = mar + id;
	}
	//Do not continue executing this instruction on a fault
	if (fault == -1)
	{
		exception(ILL_ADDR);
		return;
	}
	RF[REG][dst].w = mar;
}

//Handle MOVL through MOVH instructions
void movl_movh()
{
	dst = B0_2(ir);
	res = RF[REG][dst];
	byte = B3_10(ir);
	//printf("ir: %04X, byte: %04X\n", ir, byte);
	switch (B11_12(ir))
	{
	case MOVL:
		res.b[L] = byte;//Set LSB to "byte"
		break;
	case MOVLZ:
		res.b[L] = byte;
		res.b[M] = 0;//Set MSB to 0
		break;
	case MOVLS:
		res.b[L] = byte;
		res.b[M] = 0xFF;
		break;
	case MOVH:
		res.b[M] = byte;
		break;
	}
	RF[REG][dst] = res;
	updatePSWZN(res.w, WORD);
}

//Handle LDR and STR instructions
void ldr_str()
{
	dst = B0_2(ir);
	src = B3_5(ir);
	wb = B6(ir);
	offset = B7_13(ir);
	//Set bits is offset is negative
	if (B6(offset))
	{
		offset = S7_15(offset); //Sign extend
	}
	if (!B14(ir))
	{
		ldr();
	}
	else
	{
		str();
	}
}

//Load a value from memory relative to an address
void ldr()
{
	char fault;
	mar = RF[REG][src].w + offset;
	cr = READ;
	fault = cache(mar, &mbr, cr, wb);
	if (fault == -1)
	{
		exception(ILL_ADDR);
		return; //Do not continue executing this instruction on a fault
	}
	if (wb)
	{
		res.w = mbr;
		RF[REG][dst].b[L] = res.b[L];
	}
	else
	{
		res.w = mbr;
		RF[REG][dst].w = res.w;
	}
	updatePSWZN(res.w, wb);
}

//store a value in memory relative to an address
void str()
{
	char fault;
	mar = RF[REG][dst].w + offset;
	cr = WRITE;
	if (wb)
	{
		mbr = RF[REG][src].b[L];
	}
	else
	{
		mbr = RF[REG][src].w;

	}
	fault = cache(mar, &mbr, cr, wb);
	if (fault == -1)
	{
		exception(ILL_ADDR); 
	}
}

