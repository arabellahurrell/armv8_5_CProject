/*
 * Structures and initializers
 */

// Represents the P-STATE register
struct PState {
    bool N, Z, C, V;
};

// Errors that can occur during emulation
enum Error {
    OUT_OF_RANGE = 1,
    UNKNOWN_OPCODE
};

// Represents the state of an ARMv8 machine
struct Machine {
    uint8_t memory[NO_BYTES_MEMORY]; // 2MiB memory
    uint64_t registers[NO_REGISTERS + 1]; // 31 64-bit registers // Index 31 represents the Zero Register
    uint64_t PC; // Program Counter
    uint64_t instruction; // The current instruction being executed
    struct PState state; // The P-STATE register
    enum Error error; // Set if an error occurred during execution
};

// The single machine entity used across all execute functions
struct Machine machine;
// Used to reset the machine to all zero values
struct Machine empty_machine = {0};

// Resets the machine to the initial state
void resetMachine() {
    machine = empty_machine;
    machine.state.Z = true;
}
