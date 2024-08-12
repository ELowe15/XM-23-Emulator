/*Name of file: Debugger.h
  Author: Evan Lowe
  Version: 1.2
  This file defines the the debugger and control C handler functions
*/
#pragma once
#include <signal.h> /* Signal handling software */

#define INPUTLEN 256 //Max length for input

void debugger();
void promptUser(char* message);
void printOptions();
void commands();
void load();
void printRF();
void printPSW();
void displayMem();
void switchCache();
void differentWrite();
void writeMemory();
char toggleStep();
char incPC();
void adjustPC();
void setReg();
void setBP();
void runCPU();
char exitProgram();
void sigint_hdlr();

extern volatile sig_atomic_t ctrlCFnd; //Used to indicate a Ctrl C