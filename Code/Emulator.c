/*Name of file: Emulator.c
  Author: Evan Lowe
  Version: 1.2
  This file contains the main function which calls the debugger
  */
#include <stdio.h>
#include "Debugger.h"
char versionNum[] = "1.2";

int main()
{
    printf("XM-23 Emulator - Version: %s\n", versionNum);
    debugger();
    return 0;
}