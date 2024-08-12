/*Name of file: Mem.c
  Author: Evan Lowe
  Version: 1.2
  This file contains two functions that read and write, respectably, either a
  word or byte of data from or to a specific memory location and functions to
  display memory contents and reset memory
  */
#include <stdio.h>
#include <stdlib.h>
#include "Global.h"
#include "Mem.h"

  /* memWrite takes the value for an address location, addr, and a byte of data,
  data.The data can be a word or byte denoted by the parameter wb, 0 for word and
  1 for byte.the data is then written to the address.
  */
void memWrite(unsigned short addr, unsigned short data, unsigned char wb)
{
    if (wb == WORD)
    {
        memory.wMem[addr / 2] = data; // wMem is half as big as bMem
    }
    else
    {
        memory.bMem[addr] = data;
    }
}
/* memRead takes the value for an address location, addr, and returns the data
at that location.The data can be a word or byte denoted by the parameter wb, 0
for word and 1 for byte.the data is then written to the adress.
*/
unsigned short memRead(unsigned short addr, unsigned char wb)
{
    if (wb == WORD)
    {   
        return memory.wMem[addr / 2];
    }
    else
    {
        return memory.bMem[addr];
    }
}

//Display memory contents from a lower to upper bound
void memDisp(unsigned int lower, unsigned int upper)
{
    if (upper < lower)
    {
        printf("Upper address is less than lower\n");
        return;
    }

    int lineCount = (upper - lower - 1) / DISPLEN + 1; //amount of lines to print
    int lineLen; //Amount of bytes in the line to display
    unsigned short addrStart;
    unsigned short data[DISPLEN];
    unsigned short addr;
    int dataCount;

    //Display memory if upper and lower bound are the same
    if (upper == lower)
    {
        data[0] = memRead(lower, BYTE);
        printf("%04X: %02X ", lower, data[0]);
        printChar(data[0]);
        printf("\n");
        return;
    }

    //Loop through each line and print them
    for (int line = 1; line <= lineCount; line++)
    {
        addrStart = lower + ((line - 1) * DISPLEN); //The address at the start of the line
        //If line has less than 16 bytes
        if (upper - addrStart <= 16)
        {
            lineLen = (upper - lower) - ((line - 1) * DISPLEN); //Calcualte amount of bytes to display
        }
        else
        {
            lineLen = DISPLEN;
        }
        printf("%04X: ", addrStart);
        //Loop through the line and print the hex value of the memory 
        for (addr = addrStart, dataCount = 0; addr < addrStart + lineLen; addr++, dataCount++)
        {
            data[dataCount] = memRead(addr, BYTE);
            printf("%02X ", data[dataCount]);
        }

        for (addr = addrStart, dataCount = 0; addr < addrStart + lineLen; addr++, dataCount++)
        {
            printChar(data[dataCount]); //Print the memory contents as characters
        }
        printf("\n");
    }
}

//Sets the entire memory to 0
void resetMem()
{
    for (int i = 0; i < wMemSz; i++)
    {
        memory.wMem[i] = 0;
    }
}