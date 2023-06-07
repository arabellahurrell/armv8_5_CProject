#include <stdio.h>
#include "structures.c"

#define WORD_BYTES 4

// Fetches a word or double word from memory starting at `startIndex`
uint64_t loadFromMemory(uint64_t startIndex, bool sf) {
    uint64_t value = 0, byte;
    for (int i = 0; i < (sf ? 8 : 4); i++) {
        byte = machine.memory[startIndex + i];
        value |= byte << (i * 8);
    }
    return value;
}

// Stores the given value in memory
void storeInMemory(uint64_t value, uint64_t startIndex, bool sf) {
    for (int i = 0; i < (sf ? 8 : 4); ++i) {
        machine.memory[startIndex + i] = (uint8_t) value;
        value >>= 8;
    }
}

// Reads the given file into the given machine's memory
void readFileIntoMemory(char filename[]) {
    FILE *file = fopen(filename, "rb"); // rb = read binary

    // Determine size of file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    // Read file into memory
    fread(machine.memory, size, 1, file);
    fclose(file);
}

// Outputs the current state of the ARMv8 machine
void outputStateToFile(char filename[]) {
    FILE *file = fopen(filename, "w"); // write

    // Registers
    fprintf(file, "Registers:\n");
    for (int i = 0; i < NO_REGISTERS; i++) {
        fprintf(file, "X%02d = %016llx\n", i, machine.registers[i]);
    }
    // PC
    fprintf(file, "PC = %016llx\n", machine.PC);
    // PState
    fprintf(file, "PSTATE: %s%s", machine.state.N ? "N" : "-", machine.state.Z ? "Z" : "-");
    fprintf(file, "%s%s\n", machine.state.C ? "C" : "-", machine.state.V ? "V" : "-");
    // Non-zero memory
    fprintf(file, "Non-zero memory:\n");
    for (uint32_t i = 0; i < NO_BYTES_MEMORY; i += WORD_BYTES) {
        uint32_t value = loadFromMemory(i, 0);
        if (value != 0) {
            fprintf(file, "0x%08x: 0x%08x\n", i, value);
        }
    }
    // todo: print errors?
    fclose(file);
}
