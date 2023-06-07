// Potential bugs
//  - signed/unsigned int32
//  - little endian
//  - int/int64/int32
//  - Spec has changed
//  - Add more errors

// New potential bugs
// - 32/64-bit registers and add/subtract bit-width etc.
// - Working with zero register

#include <stdlib.h>
#include "dp_immediate.c"
#include "dp_register.c"
#include "load_or_store.c"
#include "emulate_branch.c"

#define NOP_INSTRUCTION 0xD503201F
#define HALT_INSTRUCTION 0x8A000000

// Emulator running given file
void emulate(char readFile[], char writeFile[]) {
    // Setup ARMv8 machine
    resetMachine();

    // Read binary file into machine memory
    readFileIntoMemory(readFile);

    // Main emulate loop
    while (true) {
        // Fetch
        if (machine.PC > NO_BYTES_MEMORY) { // Instruction out of range of memory
            machine.error = OUT_OF_RANGE;
            break;
        }
        machine.instruction = loadFromMemory(machine.PC, 0);

        // Decode
        uint64_t op0 = getInstructionPart(25, 4);

        // Execute
        if (machine.instruction == NOP_INSTRUCTION) { // Move to next instruction
            machine.PC += WORD_BYTES;
        } else if (machine.instruction == HALT_INSTRUCTION) { // Stop emulation
            break;
        } else if (isMaskEquals(op0, 0b1110, 0b1010)) { // Branch
            executeBranch();
        } else {
            if (isMaskEquals(op0, 0b1110, 0b1000)) { // DP Immediate
                executeDPImmediate();
            } else if (isMaskEquals(op0, 0b0111, 0b0101)) { // DP Register
                executeDPRegister();
            } else if (isMaskEquals(op0, 0b0101, 0b0100)) { // Loads and Stores
                executeLoadOrStore();
            } else { // Unknown instruction
                machine.error = UNKNOWN_OPCODE;
                break;
            }

            // Increment PC
            machine.PC += WORD_BYTES;
        }
    }

    // Output result
    outputStateToFile(writeFile);
}

int main(int argc, char *argv[]) {
    if (argc == 3) {
        emulate(argv[1], argv[2]);
        return EXIT_SUCCESS;
    } else {
        printf("Invalid number of arguments\n");
        return EXIT_FAILURE;
    }
}