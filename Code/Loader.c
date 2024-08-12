/*Name of file: Loader.c
  Author: Evan Lowe
  Version: 1.2
  This file reads an XML file's S-records
  and and writes them into correct memory locations. It also checks
  the validity of the records.
  */
#include <stdio.h>
#include <stdlib.h>
#include "Mem.h"
#include "PSW.h"
#include "Cache.h"
#include "Global.h"
#include "Loader.h"

  /*readFile takes a FILE pointer as a parameter and reads the XML file's S-records
  and and writes them into correct memory locations.It also checks
  the validity of the records.
  */
void loader(char* fileName)
{
    char buff[MAXSRECSIZE]; // A buffer to hold S-records
    int header[MAXSRECSIZE] = { 0 };
    unsigned int count;
    unsigned int lAddrByte;
    unsigned int hAddrByte;
    unsigned short strtAddr = 0; // Default starting address is 0x0000
    unsigned int addr;
    unsigned int data;
    unsigned int checksum;
    unsigned char checksumCalc; // Value to compare with checksum to confirm no errors, address + count + data
    unsigned char checksumStart;

    FILE* fptr;
    if (fileName[0] != 0)
    {
        fopen_s(&fptr, fileName, "rb"); // Open a file and have fptr point to it
    }
    else
    {
        printf("No file was entered\n");
        return; // Terminate program
    }
    if (!fptr)
    {
        printf("File has not been read\n");
        return;
    }
    //Reset the machine
    resetMem();
    resetRF();
    resetPSW();
    resetCache();

    //Reads one S-record at a time
    while (fgets(buff, MAXSRECSIZE - 1, fptr) != NULL)
    {
        sscanf_s(&buff[COUNTSTART], "%2x", &count); // Convert the characters in the S-record to hexadecimal, 'A' -> 0xA
        sscanf_s(&buff[ADDRSTART], "%2x%2x", &hAddrByte, &lAddrByte);
        addr = (hAddrByte << 8) | lAddrByte;
        checksumCalc = hAddrByte + lAddrByte + count;
        checksumStart = count * 2 + DATASTART - MINCOUNT; //convert count to number of nibbles and find the index for the start of the checksum in S-record
        sscanf_s(&buff[checksumStart], "%2x", &checksum);

        if (buff[SSTART] != 'S')
        {
            printf("This is not a valid S-record: %s\n", buff);
            return; // Terminate program
        }
        switch (buff[RECTYPESTART])
        {
        case '0':
            // Loop through the data portion of the S-record, 
            for (unsigned short i = DATASTART, j = 0; i < checksumStart; i += 2, j++)
            {
                sscanf_s(&buff[i], "%2x", &header[j]); // Extract the file name to display later
                checksumCalc += header[j]; // Add data to checksumCalc
            }
            break;

        case '1':
            //Code to read and write as words
            for (unsigned short i = DATASTART; i < (checksumStart); i += 4)
            {
                if (i == checksumStart - 2)
                {
                    sscanf_s(&buff[i], "%2x", &data);
                    checksumCalc += data;
                }
                else
                {
                    int data1;
                    int data2;
                    sscanf_s(&buff[i], "%2x%2x", &data1, &data2);
                    data = data2 << 8 | data1;
                    checksumCalc += data1 + data2;
                }
                memWrite(addr, data, WORD); // Writes words of data to memory
                addr += 2;
            }
            break;

        case '9':
            strtAddr = addr;
            break;

        default:
            printf("only S0, S1 and S9 records are supported not %s\n", buff);
            return;
        }
        // This is the calculation to check is error has occured
        if (checksumCalc + checksum != 255)
        {
            printf("Checksum indicates error in S-record %s\n", buff);
            return;
        }
    }
    // First character of the string is NULL
    if (header[0] == 0)
    {
        printf("WARNING S0 record is missing or imcomplete\n");
    }
    else
    {
        printf("Source module: ");
        for (int i = 0; header[i] != 0; i++)
        {
            printf("%c", header[i]);
        }
        printf("\n");
    }
    RF[REG][PCINDX].w = strtAddr;// Set program counter
    printf("Starting address: 0x%X\n", strtAddr);
    printf("File has been read\n");
}
