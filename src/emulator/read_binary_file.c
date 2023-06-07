#include <stdio.h>
#include "binary_functions.c"

#define WORD_BYTES 4
#define ZERO_REGISTER    0b11111

// Returns the value of register `reg` using the bit-width `sf`
uint64_t getRegisterValue(uint64_t reg, bool sf) {
    return machine.registers[reg] & getResultMask(sf);
}

// Sets the value of register `reg` to `value` using the bit-width `sf`
void setRegisterValue(uint64_t reg, uint64_t value, bool sf) {
    if (reg != ZERO_REGISTER) { // Cannot assign to the zero register
        machine.registers[reg] = value & getResultMask(sf);
    }
}

// Fetches a word or double word from memory starting at address `startIndex` using the bit-width `sf`
uint64_t loadFromMemory(uint64_t startIndex, bool sf) {
    uint64_t byte, value = 0;
    for (int i = 0; i < getWordBytes(sf); i++) {
        byte = machine.memory[startIndex + i];
        value |= byte << (i * 8);
    }
    return value;
}

// Stores `value` in memory at address `startIndex` using the bit-width `sf`
void storeInMemory(uint64_t value, uint64_t startIndex, bool sf) {
    for (int i = 0; i < getWordBytes(sf); ++i) {
        machine.memory[startIndex + i] = (uint8_t) value;
        value >>= 8;
    }
}

// Increments the machine's PC ready to load the sequentially next instruction
void incrementPC() {
    machine.PC += WORD_BYTES;
}

// Reads the file `filename` into the machine's memory
void readFileIntoMemory(char filename[]) {
    FILE *file = fopen(filename, "rb"); // rb = read binary

    // Determine the size of file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    // Read the file into memory
    fread(machine.memory, size, 1, file);
    fclose(file);
}

// Outputs the current state of the machine to the file `filename`
void outputStateToFile(char filename[]) {
    FILE *file = fopen(filename, "w"); // w = write

    // Registers
    fprintf(file, "Registers:\n");
    for (int i = 0; i < NO_REGISTERS; i++) {
        fprintf(file, "X%02d = %016llx\n", i, getRegisterValue(i, 1));
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
    // Print any errors to the console
    if (machine.error) {
        printf("Error occurred: %i\n", machine.error);
    }
    fclose(file);
}
