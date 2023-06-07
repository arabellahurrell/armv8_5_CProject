/*
 * Helper functions for reading from and writing to files
 */

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
