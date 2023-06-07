#include <stdbool.h>
#include <stdint.h>

#define NO_REGISTERS 31
#define NO_BYTES_MEMORY 0x200000


// Represents the PSTATE unit
struct PState {
    bool N, Z, C, V;
};

// Errors that can occur during emulation
enum Error {
    OUT_OF_RANGE = 1,
    UNKNOWN_OPCODE
};

// Represents the state of the ARMv8 machine
struct Machine {
    uint8_t memory[NO_BYTES_MEMORY]; // 2MB memory
    uint64_t registers[NO_REGISTERS + 1], PC, instruction; // 64-bit registers, ZR and PC, and current instruction
    struct PState state; // PState register
    enum Error error; // If an error occurred during execution
};

// Global machine variables made
struct Machine machine;
struct Machine machineEmpty = {0};

// Resets the machine to the initial state
void resetMachine() {
    machine = machineEmpty;
    machine.state.Z = true;
}
