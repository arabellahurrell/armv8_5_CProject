#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define WORD_BYTES 4
#define NO_REGISTERS 31
#define NO_BYTES_MEMORY 0x200000
#define NOP_INSTRUCTION 0xD503201F
#define HALT_INSTRUCTION 0x8A000000
#define ZERO_REGISTER 0b11111
#define STACK_POINTER 0b11111

#define MAX_UNSIGNED_INT_32 0xFFFFFFFF
#define MAX_UNSIGNED_INT_64 0xFFFFFFFFFFFFFFFF
#define MAX_SIGNED_INT_32 0x7FFFFFFF
#define MAX_SIGNED_INT_64 0x7FFFFFFFFFFFFFFF
#define MIN_SIGNED_INT_32 (-0x80000000)
#define MIN_SIGNED_INT_64 (-0x8000000000000000)

// Potential bugs
// signed/unsigned int32
// little endian
// int/int64/int32
// Spec has changed
// When working on zero register

// Todo
// Add more errors

struct PState {
    int N, Z, C, V;
};

// Errors that can occur during emulation
enum Error {
    NONE,
    OUT_OF_RANGE,
    LOAD_LITERAL,
    UnKNOWN_OPCODE
};

// Represents the hardware of the ARMv8 machine
struct Machine {
    char memory[NO_BYTES_MEMORY]; // 2MB memory
    uint64_t registers[NO_REGISTERS + 1], PC; // 64-bit registers and ZR
    struct PState state; // PSTATE register
    uint32_t instruction; // The current instruction being executed
    enum Error error; // If an error occurred during execution
};

void readFileIntoMemory(char filename[], struct Machine machine) {
    FILE *file = fopen(filename, "rb"); // rb = read binary

    // Determine size of file
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    // Read file into memory
    fread(machine.memory, size, 1, file);
    fclose(file);
}

// Returns mask of length bits all set to 1
uint64_t getMask(uint64_t bits) {
    return (1 << bits) - 1;
}

// Returns mask of length bits all set to 1
uint64_t getMaskBetween(uint64_t left, uint64_t right) {
    return getMask(left) ^ getMask(right);
}

// Returns mask of length bits all set to 1
uint64_t getResultMask(bool sf) {
    return getMask(sf ? 64 : 32);
}

// Returns the given bit of the given value
bool getBit(uint64_t value, uint64_t bit) {
    return (value >> bit) & 1;
}

// Returns the given bit of the given value
bool getMSB(uint64_t value, uint64_t sf) {
    return getBit(value, sf ? 63 : 31);
}

// Returns the given portion of the instruction
bool isMaskEquals(uint64_t value, uint64_t mask, uint64_t equalTo) {
    return (value & mask) == equalTo;
}

// Returns the given portion of the current instruction
uint64_t getInstructionPart(struct Machine machine, uint64_t littleEnd, uint64_t length) {
    return (machine.instruction >> littleEnd) & getMask(length);
}

// Returns the given signed portion of the current instruction
int64_t getInstructionPartSigned(struct Machine machine, uint64_t littleEnd, uint64_t length) {
    int32_t instruction = (int32_t) machine.instruction;
    uint64_t shift = 32 - littleEnd - length + 1;
    return (instruction << shift) >> shift;
}

void executeArithmeticInstruction(struct Machine machine, uint64_t rd, uint64_t rn, uint64_t sf,
                                  uint64_t op, uint64_t opc) {
    // Unsigned values
    uint64_t result, a, b;
    a = machine.registers[rn];
    b = op;
    // Signed values
    int64_t sa, sb;
    sa = (int64_t) machine.registers[rn];
    sb = (int64_t) op;

    // Determine operation type
    switch (opc) {
        case 0b00: // Add
            result = machine.registers[rn] + op;
        case 0b01: // Add and set flags
            result = machine.registers[rn] + op;

            machine.state.N = getMSB(result, sf);
            machine.state.Z = result == 0;
            if (sf) { // 64 bit
                machine.state.C = a > MAX_UNSIGNED_INT_64 - b;
                machine.state.V = (sa > MAX_SIGNED_INT_64 - sb) || (sa < MIN_SIGNED_INT_64 - sb);
            } else { // 32 bit
                machine.state.C = a > MAX_UNSIGNED_INT_32 - b;
                machine.state.V = (sa > MAX_SIGNED_INT_32 - sb) || (sa < MIN_SIGNED_INT_32 - sb);
            }
        case 0b10: // Subtract
            result = machine.registers[rn] - op;
        case 0b11: // Subtract and set flags
            result = machine.registers[rn] - op;

            machine.state.N = getMSB(result, sf);
            machine.state.Z = result == 0;
            machine.state.C = machine.registers[rn] < op;
            if (sf) { // 64 bit
                machine.state.V = (sa > MAX_SIGNED_INT_64 + sb) || (sa < MIN_SIGNED_INT_64 + sb);
            } else { // 32 bit
                machine.state.V = (sa > MAX_SIGNED_INT_32 + sb) || (sa < MIN_SIGNED_INT_32 + sb);
            }
    }

    // Store calculated value
    machine.registers[rd] = result;
}

void executeDPImmediate(struct Machine machine) {
    uint64_t sf, opc, opi, rd, sh, imm12, rn, hw, imm16, op, shift, lower_part, upper_part;
    sf = getInstructionPart(machine, 31, 1);
    opc = getInstructionPart(machine, 29, 2);
    opi = getInstructionPart(machine, 23, 3);
    rd = getInstructionPart(machine, 0, 5);
    sh = getInstructionPart(machine, 22, 1);
    rn = getInstructionPart(machine, 5, 5);
    hw = getInstructionPart(machine, 21, 2);
    imm12 = getInstructionPart(machine, 10, 12);
    imm16 = getInstructionPart(machine, 5, 16);

    if (rd == ZERO_REGISTER) {
        // Cannot assign to the zero register
        return;
    }

    switch (opi) {
        case 0b010: // Arithmetic instruction
            // Handle shift flag
            if (sh) {
                imm12 = imm12 << 12;
            }
            executeArithmeticInstruction(machine, rd, rn, sf, imm12, opc);
        case 0b101: // Wide move
            shift = hw * 16;
            op = imm16 << shift;
            switch (opc) {
                case 0b00: // Move wide with NOT
                    machine.registers[rd] = ~op & getResultMask(sf);
                case 0b10: // Move wide with zero
                    machine.registers[rd] = op;
                case 0b11: // Move wide with keep
                    lower_part = machine.registers[rd] & getMask(shift);
                    upper_part = ((hw == 0b11) ? 0 : (machine.registers[rd] & getMaskBetween(64, (hw + 1) * 16)));
                    machine.registers[rd] = (lower_part | imm16 | upper_part) & getResultMask(sf);
            }
    }
}

// Performs rotate right shift to the given value
uint64_t rotateRight(uint64_t value, uint64_t shift, uint64_t sf) {
    uint64_t shifted, rotated;
    shifted = value >> shift;
    rotated = value << ((sf ? 64 : 32) - shift);
    return (shifted | rotated) & getResultMask(sf);
}

// Applies the given shift to the given value
uint64_t applyShift(uint64_t value, uint64_t shift, uint64_t sf) {
    switch (shift) {
        case 0b00: // lsl
            return value << shift;
        case 0b01: // lsr
            return value >> shift;
        case 0b10: // asr
            return value >> shift;
        case 0b11: // ror
            return rotateRight(value, shift, sf);
    }
}

void executeDPRegister(struct Machine machine) {
    uint32_t sf, opc, M, rm, rn, rd, shift, imm6, isArithmetic, N, x, ra, op2, mul;
    sf = getInstructionPart(machine, 31, 1);
    opc = getInstructionPart(machine, 29, 2);
    M = getInstructionPart(machine, 28, 1);
    rm = getInstructionPart(machine, 16, 5);
    rn = getInstructionPart(machine, 5, 5);
    rd = getInstructionPart(machine, 0, 5);
    shift = getInstructionPart(machine, 22, 2);
    imm6 = getInstructionPart(machine, 10, 6);
    isArithmetic = getInstructionPart(machine, 24, 1);
    N = getInstructionPart(machine, 21, 1);
    ra = getInstructionPart(machine, 10, 5);
    x = getInstructionPart(machine, 15, 1);

    if (M) { // Multiply
        if (ra == 0b11111) {
            // Encode the zero register
        }
        mul = machine.registers[rn] * machine.registers[rm];
        if (x) { // Multiply-Sub
            machine.registers[rd] = machine.registers[ra] - mul;
        } else { // Multiply-Add
            machine.registers[rd] = machine.registers[ra] + mul;
        }
    } else { // Arithmetic and Logical
        if (!(shift == 0b11 && isArithmetic)) {
            op2 = applyShift(machine.registers[rm], shift, sf);
        }
        if (isArithmetic) { // Arithmetic
            executeArithmeticInstruction(machine, rd, rn, sf, imm6, opc);
        } else { // Logical
            if (N) { // Bitwise negate op2
                op2 = ~op2;
            }
            switch (opc) {
                case 0b00:// AND
                    machine.registers[rd] = machine.registers[rn] & op2;
                case 0b01: // OR
                    machine.registers[rd] = machine.registers[rn] | op2;
                case 0b10: // OR NOT
                    machine.registers[rd] = machine.registers[rn] ^ ~op2;
                case 0b11: // AND and set flags
                    machine.registers[rd] = machine.registers[rn] & op2;

                    machine.state.N = getBit(machine.registers[rd], sf ? 63 : 31);
                    machine.state.Z = getBit(machine.registers[rd] == 0, sf ? 63 : 31);
                    machine.state.C = 0;
                    machine.state.V = 0;
            }
        }
    }
}

void executeSingleDataTransfer(struct Machine machine) {
    uint64_t sf, U, L, xn, rt, xm, imm12, I, bit21, msb;
    int64_t simm9, simm19;
    sf = getInstructionPart(machine, 30, 1);
    U = getInstructionPart(machine, 24, 1);
    L = getInstructionPart(machine, 22, 1);
    xn = getInstructionPart(machine, 5, 5);
    rt = getInstructionPart(machine, 0, 5);
    xm = getInstructionPart(machine, 16, 5);
    imm12 = getInstructionPart(machine, 10, 12);
    I = getInstructionPart(machine, 11, 1);
    bit21 = getInstructionPart(machine, 21, 1);
    simm9 = getInstructionPartSigned(machine, 12, 9);
    simm19 = getInstructionPartSigned(machine, 5, 19);
    msb = getInstructionPart(machine, 31, 1);

    if (xn == STACK_POINTER) {
        // Stack pointer not implemented
        return;
    }

    // Determine address to load/store
    uint64_t address;
    if (msb) { // Single data transfer
        if (U) { // Unsigned immediate offset
            address = machine.registers[xn] + imm12;
        } else if (bit21) { // Register offset
            address = machine.registers[xn] + machine.registers[xm];
        } else if (I) { // Pre-indexed
            address = machine.registers[xn] + simm9;
            machine.registers[xn] = address; // Write-back
        } else { // Post-indexed
            address = machine.registers[xn];
            machine.registers[xn] = address; // Write-back
        }
    } else { // Load literal
        address = machine.PC + simm19;
    }

    // Execute
    if (!msb || L) { // Load
        machine.registers[rt] = machine.memory[address];
    } else { // Store
        machine.memory[address] = machine.registers[rt];
    }
}

void executeBranch(struct Machine machine) {
    uint64_t xn, cond, opc, jump;
    int64_t simm26, simm19;
    xn = getInstructionPart(machine, 5, 5);
    cond = getInstructionPart(machine, 0, 4);
    opc = getInstructionPart(machine, 30, 2);
    simm26 = getInstructionPartSigned(machine, 0, 26);
    simm19 = getInstructionPartSigned(machine, 5, 19);

    switch (opc) {
        case 0b00: // Unconditional
            machine.PC += simm26 * WORD_BYTES;
        case 0b11: // Register
            machine.PC = machine.registers[xn];
        case 0b01: // Conditional
            // Determine whether to jump
            switch (cond) {
                case 0b0000: // Equal
                    jump = machine.state.Z;
                case 0b0001: // Not equal
                    jump = !machine.state.Z;
                case 0b1010: // Signed greater or equal
                    jump = machine.state.N;
                case 0b1011: // Signed less than
                    jump = !machine.state.Z;
                case 0b1100: // Signed greater than
                    jump = !machine.state.N && machine.state.N == machine.state.V;
                case 0b1101: // Signed less than or equal
                    jump = !(!machine.state.N && machine.state.N == machine.state.V);
                case 0b1110: // Always
                    jump = true;
            }
            // Execute jump if condition satisfied
            if (jump) {
                machine.PC += simm19 * WORD_BYTES;
            }
    }
}

// Outputs the state of the given ARMv8 machine
void output(struct Machine machine) {
    // Registers
    printf("Registers:\n");
    for (int i = 0; i < NO_REGISTERS; i++) {
        printf("X");
        printf("%02d", i); // Pad with zeros to length 2
        printf(" = ");
        printf("%016llx", machine.registers[i]); // Pad with zeros to length 16
        printf("\n");
    }

    // PC
    printf("PC = ");
    printf("%016llx", machine.PC);
    printf("\n");

    // PState
    printf("PSTATE : ");
    printf(machine.state.N ? "N" : "-");
    printf(machine.state.Z ? "Z" : "-");
    printf(machine.state.C ? "C" : "-");
    printf(machine.state.V ? "V" : "-");
    printf("\n");

    // Non-zero memory
    printf("Non-zero memory:\n");
    for (int i = 0; i < NO_BYTES_MEMORY; i++) {
        int64_t value = machine.memory[i];
        if (value != 0) {
            printf("0x");
            printf("%08x", i); // Pad with zeros to length 8
            printf(": ");
            printf("0x");
            printf("%08llx", value); // Pad with zeros to length 8
            printf("\n");
        }
    }
}

// Emulates running given file
void emulate(char filename[]) {
    // Initialise ARMv8 machine with memory and registers
    struct Machine machine;

    // Read binary file into memory
    readFileIntoMemory(filename, machine);

    // Main emulate loop
    while (true) {
        // Fetch
        if (machine.PC > NO_BYTES_MEMORY) {
            // Instruction out of range of memory
        }
        machine.instruction = *(machine.memory + machine.PC);
        machine.PC += 4;

        // Decode
        uint32_t op0 = getInstructionPart(machine, 25, 4);

        // Execute
        if (machine.instruction == HALT_INSTRUCTION) {
            // Stop emulation
            break;
        } else if (machine.instruction == NOP_INSTRUCTION) {
            // Pass to next
        } else if (isMaskEquals(op0, 0b1110, 0b1000)) { // DP Immediate
            executeDPImmediate(machine);
        } else if (isMaskEquals(op0, 0b0111, 0b0101)) { // DP Register
            executeDPRegister(machine);
        } else if (isMaskEquals(op0, 0b0101, 0b0100)) { // Loads and Stores
            executeSingleDataTransfer(machine);
        } else if (isMaskEquals(op0, 0b1110, 0b1010)) { // Branch
            executeBranch(machine);
        } else {
            // Unknown opcode
        }
    }

    // Output result
    output(machine);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("No binary file was inputted.\n");
    } else if (argc > 2) {
        printf("Too many arguments were inputted.\n");
    } else {
        emulate(argv[1]);
    }
    return EXIT_SUCCESS;
}
