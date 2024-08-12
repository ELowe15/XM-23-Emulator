/*Name of file: Debugger.c
  Author: Evan Lowe
  Version: 1.5
  This file prompts the user for input and then uses the input
  to run commands
*/
#include <stdio.h>
#include <signal.h> /* Signal handling software */
#include "Loader.h"
#include "CPU.h"
#include "Global.h"
#include "Debugger.h"
#include "PSW.h"
#include "Mem.h"
#include "Cache.h"
#include "Exceptions.h"

volatile sig_atomic_t ctrlCFnd;
char input[INPUTLEN];

//Starts the debugger and sets ctrlCFnd to false
void debugger()
{
    ctrlCFnd = FALSE;
    signal(SIGINT, (_crt_signal_t)sigint_hdlr);
    printOptions();
    commands();
}

//Prints the command options the user has
void printOptions()
{
    printf("? - List all options\n"
        "L - Load.XML file\n"
        "R - Display registers\n"
        "P - Display PSW bits\n"
        "M - Display memory\n"
        "C - Display cache lines\n"
        "O - Switch cache orgagnization\n"
        "D - Different write policy\n"
        "W - Write to memory location\n"
        "A - Adjust program counter\n"
        "S - Set register value\n"
        "B - Add breakpoint\n"
        "T - Toggle increment program counter\n"
        "I - Increment program counter\n"
        "G - Go (increment with 'I')\n"
        "X - Exit running program\n");
}

//Handles the user input
void commands()
{
    char command = TRUE; //Used to determine whether the command while loop should still run

    while (command)
    {
        promptUser("Enter a command: "); //Prompt the user to enter a command and read the input
        switch (input[0]) // The first letter inputed indicating the command
        {
        case '?':
            printOptions();
            break;
            //Load a .xme file
        case 'l':
        case 'L':
            //loader("Setpri.xme");
            load();
            break;
            //Print all registers
        case 'r':
        case 'R':
            printRF();
            break;
            //Display PSW
        case 'p':
        case 'P':
            printPSW();
            break;
            //Display memory
        case 'm':
        case 'M':
            displayMem();
            break;
            //Display cache
        case 'c':
        case 'C':
            cacheDisp();
            break;
        case 'o':
        case 'O':
            switchCache();
            break;
        case 'd':
        case 'D':
            differentWrite();
            break;
        //Write to memory
        case 'w':
        case 'W':
            writeMemory();
            break;
            //Change the program counter
        case 'a':
        case 'A':
            adjustPC();
            break;
            //Set register value
        case 's':
        case 'S':
            setReg();
            break;
            //Set breakpoint
        case 'b':
        case 'B':
            setBP();
            break;
            //Toggle increment PC
        case 't':
        case 'T':
            command = toggleStep();
            break;
            //Increment program counter
        case 'i':
        case 'I':
            command = incPC();
            break;
            //Run program
        case 'g':
        case 'G':
            if (running)
            {
                printf("program is already running\n");
            }
            else
            {
                runCPU();
            }
            break;
            // Exit program
        case 'x':
        case 'X':
            command = exitProgram();
            break;
            // Handle a case where control C was entered
        case CTRLC:
            if (running)
            {
                command = FALSE;
            }
            break;
        default:
            printf("Invalid input\n");
            break;
        }
    }
}

/* Prompts user for input and writes the input the the variable "input". This
function takes a message as a parameter to display to the user what they should enter */
void promptUser(char* message)
{
    printf("%s", message);
    //Read user input
    if (scanf_s("%s", input, INPUTLEN) != 1)
    {   //Used to indated if a control C was entered
        if (ctrlCFnd == TRUE)
        {
            input[0] = CTRLC;
        }
        else
        {
            printf("Input error\n");
        }
    }
}

//Load an .xme file
void load()
{
    //Do not load a file while a program is already running
    if (running)
    {
        printf("Program is running\n");
        return;
    }
    promptUser("Enter .XME file to load: ");
    printf("\n");
    loader(input); //Call loader
}

//Print contents of the register in the register file
void printRF()
{
    for (int regIndx = 0; regIndx < REGNUM; regIndx++)
    {
        switch (regIndx)
        {
        case BPINDX:
            printf("R%i (BP): 0x%04X\n", regIndx, RF[REG][regIndx].w);
            break;
        case LRINDX:
            printf("R%i (LR): 0x%04X\n", regIndx, RF[REG][regIndx].w);
            break;
        case SPINDX:
            printf("R%i (SP): 0x%04X\n", regIndx, RF[REG][regIndx].w);
            break;
        case PCINDX:
            printf("R%i (PC): 0x%04X\n", regIndx, RF[REG][regIndx].w);
            break;
        default:
            printf("R%i: 0x%04X\n", regIndx, RF[REG][regIndx].w);
            break;
        }
    }
}

//Print the values of the PSW bits
void printPSW()
{
    printf("Carry: %i\n"
        "Zero: %i\n"
        "Negative: %i\n"
        "Sleep: %i\n"
        "Overflow: %i\n"
        "Current Priority: %i\n"
        "Faulting: %i\n"
        "Reserved: %i\n"
        "Previous priority: %i\n", psw.bit.c, psw.bit.z, psw.bit.n, psw.bit.slp, psw.bit.v, psw.bit.current, psw.bit.flt, psw.bit.reserved, psw.bit.previous);
}

//Display a section of memory
void displayMem()
{
    promptUser("Enter range of addresses, lower and upper bound (hex): ");
    printf("\n");
    unsigned int lower;
    unsigned int upper;
    if ((sscanf_s(input, "%4x", &lower)) != 1)
    {
        printf("Invalid lower address\n");
        return;
    }
    promptUser("");
    if ((sscanf_s(input, "%4x", &upper)) != 1)
    {
        printf("Invalid upper address\n");
        return;
    }
    printf("%04X %04X\n", lower, upper);
    memDisp(lower, upper);
}

//Switch cache organization
void switchCache()
{
    if (cacheOrg == ASSOCIATIVE)
    {
        cacheOrg = DIRECT;
        printf("Direct mapping cache organization enabled\n");
    }
    else if (cacheOrg == DIRECT)
    {
        cacheOrg = COMBINED;
        printf("Combined cache organization enabled\n");
    }
    else
    {
        cacheOrg = ASSOCIATIVE;
        printf("Associative cache organization enabled\n");
    }
}


//Switch between write policies
void differentWrite()
{
    if (wp == BACK)
    {
        wp = THROUGH;
        printf("Write through policy enabled\n");
    }
    else
    {
        wp = BACK;
        printf("Write back policy enabled\n");
    }
}

//Write to memory
void writeMemory()
{
    promptUser("Word (0) or Byte (1): ");
    printf("\n");
    unsigned int data;
    unsigned int wb;
    unsigned int addr;
    if ((sscanf_s(input, "%i", &wb)) != 1)
    {
        printf("Invalid word/byte\n");
        return;
    }
    promptUser("Enter memory to write, address then data (hex): ");
    printf("\n");
    if ((sscanf_s(input, "%4x", &addr)) != 1)
    {
        printf("Invalid address\n");
        return;
    }
    promptUser("");
    //Read word
    if (!wb)
    {
        if ((sscanf_s(input, "%4x", &data)) != 1)
        {
            printf("Invalid data\n");
            return;
        }
    }
    else
    {
        if ((sscanf_s(input, "%2x", &data)) != 1)
        {
            printf("Invalid data\n");
            return;
        }
    }
    memWrite(addr, data, wb);
}

//Change the value of the program counter
void adjustPC()
{
    unsigned int newPC;
    promptUser("Enter new value of PC (hex): ");
    printf("\n");
    //Convert input string to a numerical value
    if ((sscanf_s(input, "%4x", &newPC)) != 1)
    {
        printf("Invalid address\n");
        return;
    }
    //Check if address is even
    if ((newPC % 2) != 0)
    {
        printf("PC must be an even address\n");
    }
    else
    {
        RF[REG][PCINDX].w = newPC;// Set value of program counter
        printf("new PC: %X\n", RF[REG][PCINDX].w);
    }
}

//Set a registers value in either the least significant byte or a word
void setReg()
{
    unsigned int regNum;
    unsigned int data;
    unsigned int wb;
    promptUser("Word (0) or Byte (1): ");
    printf("\n");
    if ((sscanf_s(input, "%i", &wb)) != 1)
    {
        printf("Invalid word/byte\n");
        return;
    }
    promptUser("Enter register to change, reg number (0-7) then data (hex): ");
    printf("\n");
    if ((sscanf_s(input, "%i", &regNum)) != 1)
    {
        printf("Invalid reg number\n");
        return;
    }
    if ((regNum < 0) || (regNum > 7))
    {
        printf("Reg number must be between 0 and 7\n");
        promptUser("");
        return;
    }
    promptUser("");
    //Read a word
    if (wb == WORD)
    {
        if ((sscanf_s(input, "%4x", &data)) != 1)
        {
            printf("Invalid data\n");
            return;
        }
        RF[REG][regNum].w = data;
    }
    //Read a byte
    else if (wb == BYTE)
    {
        if ((sscanf_s(input, "%2x", &data)) != 1)
        {
            printf("Invalid data\n");
            return;
        }
        RF[REG][regNum].b[L] = data;
    }
    else
    {
        printf("Invalid word/byte\n");
    }
}

//Set a new breakpoint
void setBP()
{
    unsigned int brk;
    if (brkPnt)
    {
        printf("Previous breakpoint: 0x%04X\n", brkPnt);
    }
    promptUser("Enter breakpoint address (hex): ");
    printf("\n");
    //Convert input string to a numerical value
    if ((sscanf_s(input, "%4x", &brk)) != 1)
    {
        printf("Invalid address\n");
        return;
    }
    if ((brk % 2) != 0)
    {
        printf("Breakpoint must be an even address\n");
        return;
    }
    brkPnt = brk;
    printf("New breakpoint at 0x%04X\n", brkPnt);
}

//Toggle step and return command which stops the while loop if false
char toggleStep()
{
    char command = TRUE;
    if (step)
    {
        step = FALSE;
        printf("Single step disabled\n");
        //Stop prompting user for commands and run the program
        if (running)
        {
            command = FALSE;
        }
    }
    else
    {
        step = TRUE;
        printf("Single step enabled\n");
    }
    return command;
}

//increment PC by returning command which stops the while loop if false
char incPC()
{
    char command = TRUE;
    // Break out of command loop
    if (running)
    {
        command = FALSE;
    }
    else
    {
        printf("No program is running\n");
    }
    return command;
}

//Run the cpu
void runCPU()
{
    ctrlCFnd = FALSE; //Set to false so a control C input can be read properly
    running = TRUE; //indicate a prorgam is running
    clock = 0; //reset clock
    char fault;
    printf("Start of program - PC: 0x%04X, Breakpoint: 0x%04X, PSW: 0x%04X, Clock: %u\n", RF[REG][PCINDX].w, brkPnt, psw.w, clock);
    while (running)
    {
        fault = fetch();
        //Do not decode and execute if a fault occured on the fetch
        if (fault == -1)
        {
            exception(ILL_ADDR);
        }
        else
        {
            decodeNExecute();
        }
        //Stop CPU at breakpoint
        if (RF[REG][PCINDX].w == brkPnt)
        {
            printf("Reached breakpoint: 0x%04X, Clock: %u\n", brkPnt, clock);
            commands(); //Prompt user to enter a command
        }
        //Stop CPU if step is enabled
        else if (step)
        {
            printf("Current PC: 0x%04X, PSW: 0x%04X, Clock: %u\n", RF[REG][PCINDX].w, psw.w, clock);
            commands(); //Prompt user to enter a command
        }
        running = !ctrlCFnd; //Will stop the while loop if a Control C interrupt occurs
    }
    printf("End of program - PC: 0x%04X, Clock: %i\n", RF[REG][PCINDX].w, clock);
    printf("Exiting program\n");
}

//exit running program by returning command which stops the while loop if false
char exitProgram()
{
    char command = TRUE;
    if (running)
    {
        ctrlCFnd = TRUE; // Used to break out of cpu cycle
        command = FALSE;
    }
    else
    {
        printf("No program is running\n");
    }
    return command;
}
//Handle a control C input
void sigint_hdlr()
{
    /*
    - Invoked when SIGINT (control-C) is detected
    - changes state of waiting_for_signal
    - signal must be reinitialized
    */
    ctrlCFnd = TRUE;
    signal(SIGINT, (_crt_signal_t)sigint_hdlr); /* Reinitialize SIGINT */
    printf("^C");
}