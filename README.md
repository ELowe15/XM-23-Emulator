# XM-23 Emulator

## Overview
The XM-23 Emulator emulates the hardware behavior of the 16-bit von Neumann XM-23 machine, providing an environment 
to debug, analyze, and execute programs in a virtual environment. The emulator implements the cpu, 8 16-bit registers, 
64 kiB memory, a program status word, decoding and execution of 41 instructions, 3 cache organizations, exception 
handling and debugging tools.

## Key Features

### Debugger

Debugger.c implements a command-line debugger to interact with and control the execution of programs within the emulator. 
It allows loading programs, inspecting/modifying registers and memory, setting breakpoints, and managing CPU state.

#### Key Features:
- **Load Programs**: Load `.xme` files into the simulator.
- **Inspect and Modify State**: Display and modify CPU registers, memory, PSW, and cache.
- **Control Execution**: Start, stop, step through the program with single-step mode.
- **Set Breakpoints**: Halt execution at specific memory addresses.
- **Change CPU Settings**: Modify cache organization and write policies.

#### Error Handling:
- Invalid inputs prompt the user to enter valid commands.
- Some actions are restricted during program execution (e.g., loading a file while running).

---

### Loader

`Loader.c` processes S-records from an XML file, writing them into memory and verifying data integrity via checksum 
validation. It handles various S-record types and sets the program counter.

#### Key Features:
- **Reads S-records**: Processes S0, S1, and S9 records.
- **Checksum Verification**: Ensures data integrity by validating checksums.
- **Memory Handling**: Writes data to memory and sets the program counter.

#### Supported S-record Types:
- **S0**: Header information.
- **S1**: Memory data.
- **S9**: Starting address for the program.

#### Error Handling:
- Invalid S-records (e.g., checksum mismatches) trigger errors.
- Missing or incomplete records display warnings.

---

### Memory

`Mem.c` provides functions to read and write to memory, display memory contents, and reset memory to its default state.

#### Key Features:
- **Memory Write**: Write data (word or byte) to a specified address.
- **Memory Read**: Read data from memory.
- **Display Memory**: Display memory contents in a specified address range.
- **Reset Memory**: Clear all memory values.

#### Error Handling:
- If the address is out of bounds or invalid, the function handles the error appropriately.
- Inconsistent address ranges in memory display trigger error messages.

---

### Program status word

`PSW.c` defines and manages the Program Status Word (PSW), which tracks flags like carry, overflow, zero, and negative. 
It updates and resets the PSW based on arithmetic operations.

#### Key Features:
- **Update PSW**: Update status flags based on operation results.
- **Reset PSW**: Reset PSW to its default state.

#### Error Handling:
- Assumes correct word/byte distinctions are passed for PSW updates.
- Overflow and carry conditions are computed based on predefined arrays.

---

### CPU
  
`CPU.c` fetches, decodes, and executes instructions, interacting with memory, performing arithmetic, and 
handling branching.

#### Key Functions:
- **Fetch**: Retrieves the next instruction and updates the program counter (PC).
- **Decode & Execute**: Decodes and executes the instruction, including arithmetic and logical operations.
- **Branch Instructions**: Includes conditional and unconditional branching .
- **Memory Access**: Performs load/store operations with memory.

#### Error Handling:
- Invalid instructions or memory access errors trigger exceptions.

---

### Cache

`Cache.c` handles cache memory access, supporting different cache organizations (direct, associative, combined) 
and managing cache lines based on memory access patterns.

#### Functions:
- **cache()**: Manages cache memory access.
- **associative()**: Handles cache hits/misses in an associative cache.
- **LRU()**: Finds the least recently used cache line.
- **updateUsage()**: Updates cache usage based on memory access patterns.

#### Error Handling:
- Cache access and organization issues are managed with error checks.

---

### Exception Handling Function  

This file defines the `exception()` function, which manages system exceptions safely by calling the appropriate 
fault handler based on a given exception vector index. It ensures the system operates reliably, even in cases 
like double faults or invalid stack pointers, while displaying fault information to the user.  

#### Key Features  
- **Vector Address Handling**: Reads the exception vector address using the provided index.  
- **Fault Handling**: Manages illegal instruction, illegal address, priority, and double faults.  
- **Stack Management**: Ensures a valid stack pointer, saves machine state, and restores it after fault handling.  
- **Machine State Safety**: Pushes and restores program counter (`PC`), link register (`LR`), processor status word (`PSW`), and context state.  
- **Informative Messages**: Displays clear fault messages for users.  

#### Fault Messages Examples  
- "ILLEGAL INSTRUCTION FAULT has occurred"  
- "ILLEGAL ADDRESS FAULT has occurred"  
- "PRIORITY FAULT has occurred"  
- "DOUBLE FAULT has occurred"  

## Compilation and Usage

To run the program, you can directly execute the precompiled binary executable available in the directory.

### Running the Executable
The executable is named `emulator` (or similar). You can run the emulator with the following command:

# Assembler

This converts assembly code into XML format for further processing. It includes 
an executable that can be run to perform the conversion. The folder contains several assembly files, along with 
their corresponding assembled XML files. The project allows users to easily convert assembly code to XML using 
the provided assembler.

## Features
- **Executable**: A program that can be run to convert assembly code into XML format.
- **Assembly and XML Files**: The folder contains multiple assembly files, as well as their corresponding XML
  output after being processed.
- **Flexible and Efficient**: This tool simplifies the process of converting assembly to XML, making it more
  accessible for further analysis or processing.

## Acknowledgments
- **Dr. Hughes**: Special thanks to Dr. Hughes for providing the assembler used to convert assembly code to XML. His contribution was crucial in the development of this project.
