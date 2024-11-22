# XM-23 Emulator

## Overview
The XM-23 Emulator emulates the hardware behavior of the 16-bit von Neumann XM-23 machine, providing an environment 
to debug, analyze, and execute programs in a virtual environment. The emulator implements the cpu, bus,  8 16-bit registers, 64 kiB memory, a program status word, decoding and execution of 41 instructions, 3 cache organizations, exception handling and debugging tools.

### Macros
Multiple header files including `Global.h`, `Exceptions.h` and `Cache.h` provide numerous macros to manipulate data efficiently. This includes bit extraction/setting/clearing, calculating addresses in the vector table and bit masks with keys to index into the cache tables.

### Data Structures
Unions and structs are used extensively throughout this program to easily access data as words, bytes, nibbles or 
individual bits.

## Components

### Debugger

`Debugger.c` implements a command-line debugger to interact with and control the execution of programs within the emulator. 
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
The data structure defining the 64 kiB memory is declared in `Global.h`. This was done to avoid cyclic dependency of files.

#### Key Features:
- **Memory Write**: Write data (word or byte) to a specified address.
- **Memory Read**: Read data from memory.
- **Display Memory**: Display memory contents in a specified address range.
- **Reset Memory**: Clear all memory values.

#### Error Handling:
- If the address is out of bounds or invalid, the function handles the error appropriately.
- Inconsistent address ranges in memory display trigger error messages.

---

### Register file

`Global.h` Defines the eight registers used by this machine.

1. **General-Purpose Registers (0-3):**  
   - Registers `R0` to `R3` are used for temporary storage, calculations, and general-purpose tasks.

2. **Base Pointer (BP):**  
   - **Index:** `BPINDX = 4`  
   - **Description:** Points to the base of the current stack frame in memory.

3. **Link Register (LR):**  
   - **Index:** `LRINDX = 5`  
   - **Description:** Stores the return address for subroutine calls.

4. **Stack Pointer (SP):**  
   - **Index:** `SPINDX = 6`  
   - **Description:** Points to the top of the stack. Defaults to `DEFAULTSP = 0x800`.

5. **Program Counter (PC):**  
   - **Index:** `PCINDX = 7`  
   - **Description:** Tracks the current instruction being executed.

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

#### Key Features:

- **Fetch**: Retrieves instructions from memory, manages program counter, and handles cache hits/misses.  
- **Decode and Execute**: Differentiates instructions, executes arithmetic, logical, branching, and
  memory operations.  
- **Branching**: Supports conditional and unconditional branching (e.g., BEQ, BNE, BRA).  
- **Arithmetic and Logic**: Handles addition, subtraction, bitwise operations, and comparisons.
- **Load and Store** Data transfer instructions (`LDR`, `STR`).
- **Return from Exception**: Restores CPU state from the stack after handling an exception. I've been in the scary videos

#### Error Handling:

- **Memory Alignment Errors**: Checks stack pointer alignment for word access.  
- **Cache Access**: Identifies illegal address faults during memory access.  
- **Illegal Instructions**: Triggers exceptions for unsupported opcodes.  
- **Branch Offset Calculation**: Ensures signed offset handling and even address alignment.

---

### Cache

`Cache.c` handles cache memory access, supporting different cache organizations (direct, associative, combined) 
and managing cache lines based on memory access patterns.

#### Key Features:

##### Cache Organization
1. **Associative Cache:**  
   - Searches the cache for a specific address using a linear search.  
   - Utilizes the **Least Recently Used (LRU)** algorithm to replace data when the cache is full.  
   - Allows for flexible memory mapping, where any memory block can be stored in any cache line.  
   - **Benefits:** Reduces collisions and increases hit rates for irregular access patterns.  
   - **Usage:** Primarily used when high flexibility in memory mapping is desired.

2. **Direct Cache:**  
   - Uses a fixed mapping mechanism where each memory block maps to a specific cache line determined by a key.  
   - Ensures simple and fast access but may suffer from conflicts when multiple memory blocks map to the same line.  
   - **Benefits:** Provides predictable performance and simplicity in implementation.  
   - **Usage:** Ideal for scenarios where memory access patterns are predictable.

3. **Combined Cache:**  
   - Combines elements of both associative and direct mapping.  
   - Memory blocks are divided among multiple caches, with each cache potentially using associative mapping.  
   - **Benefits:** Balances the advantages of both associative and direct mapping to improve efficiency and flexibility.  
   - **Usage:** Suitable for complex systems requiring both speed and adaptability.

##### Write Policies
1. **Write-Through Policy:**  
   - Data written to the cache is immediately written back to the main memory.  
   - Ensures that main memory always has the most up-to-date data.  
   - **Advantages:** Simple and reliable, ensuring memory consistency.  
   - **Disadvantages:** Higher bus traffic due to frequent memory writes, potentially reducing performance.

2. **Write-Back Policy:**  
   - Data is written to the cache, and the main memory is updated only when the cache line is replaced or marked "dirty."  
   - Uses **dirty bits** to track modified data in the cache.  
   - **Advantages:** Reduces bus traffic by consolidating memory writes.  
   - **Disadvantages:** Requires additional logic to manage dirty bits and introduces complexity for memory consistency during replacements.

##### Bus Interaction:
  Cache misses trigger memory access via the bus to load data into the cache.

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

---

## Compilation and Usage

To run the program, you can directly execute the precompiled binary executable available in the directory.

### Running the Executable
The executable is named `XM23.exe`. The program's main function is also found in the `Emulator.C` file

### Test Files
Custom test files which can be loaded and run on the emulator can be found in the folder `test_files`. This 
includes the assembly files and their respective assembled xml files.

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
