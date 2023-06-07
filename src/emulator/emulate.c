/*
 * Main emulator file
 */

#include "emulate.h"

// Run the emulator on the binary file `readFile` and write the final state to `writeFile`
void emulate(char readFile[], char writeFile[]) {
    // Setup ARMv8 machine
    resetMachine();

    // Read binary file into machine memory
    readFileIntoMemory(readFile);

    // Main emulator loop
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
            incrementPC();
        } else if (machine.instruction == HALT_INSTRUCTION) { // Stop emulation
            break;
        } else if (isMaskEquals(op0, 0b1110, 0b1010)) { // Branch
            executeBranch();
        } else if (isMaskEquals(op0, 0b1110, 0b1000)) { // DP Immediate
            executeDPImmediate();
            incrementPC();
        } else if (isMaskEquals(op0, 0b0111, 0b0101)) { // DP Register
            executeDPRegister();
            incrementPC();
        } else if (isMaskEquals(op0, 0b0101, 0b0100)) { // Loads and Stores
            executeLoadOrStore();
            incrementPC();
        } else { // Unknown instruction
            machine.error = UNKNOWN_OPCODE;
            break;
        }
    }

    // Output final machine state
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
