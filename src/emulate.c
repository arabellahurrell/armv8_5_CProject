#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define WORD_BYTES 4
#define NO_REGISTERS 31
#define NO_BYTES_MEMORY 0x200000
#define NOP_INSTRUCTION 0xD503201F
#define HALT_INSTRUCTION 0x8A000000
#define STACK_POINTER 0b11111

#define MAX_INT_32 0xFFFFFFFF
#define MIN_INT_32 (-2147483647)
#define MAX_INT_64 0xFFFFFFFFFFFFFFFF
#define MIN_INT_64 (-9223372036854775807)

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
    uint64_t registers[NO_REGISTERS], ZR, PC; // 64-bit registers
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

// Returns the mask with given length set to 1
uint32_t getMask(uint32_t length) {
    return (1 << length) - 1;
}

// Returns the given portion of the current instruction
uint32_t instructionPart(struct Machine machine, uint32_t littleEnd, uint32_t length) {
    return (machine.instruction >> littleEnd) & getMask(length);
}

// Returns the most significant bit of the given value
bool msb(uint64_t value) {
    return value >> 63;
}

// Returns the given signed portion of the instruction
uint32_t instructionPartSigned(struct Machine machine, uint32_t littleEnd, uint32_t length) {
    uint32_t part, sign, mask = 0;
    part = instructionPart(machine, littleEnd, length);
    sign = part >> (length - 1);
    if (sign) {
        mask = getMask(32) ^ getMask(length);
    }
    return part | mask;
}

// Returns the given portion of the instruction
bool maskEquals(uint32_t value, uint32_t mask, uint32_t equalTo) {
    return (value & mask) == equalTo;
}

// Sets the PSTATE overflow value for arithmetic subtract
void subtractOverflow(struct Machine machine, uint32_t rn, uint32_t imm12, uint32_t sf) {
    switch (sf) {
        case 0: // Rd is 32-bit
            if ((machine.registers[rn] < 0 && imm12 > MAX_INT_32 + machine.registers[rn]) ||
                (machine.registers[rn] > 0 && imm12 < MIN_INT_32 + machine.registers[rn])) {
                machine.state.V = 1;
            }
        case 1: // Rd is 64-bit
            if ((machine.registers[rn] < 0 && imm12 > MAX_INT_64 + machine.registers[rn]) ||
                (machine.registers[rn] > 0 && imm12 < MIN_INT_64 + machine.registers[rn])) {
                machine.state.V = 1;
            }
    }
    machine.state.V = 0;
}

void executeArithmetic(struct Machine machine, uint32_t rd, uint32_t rn, uint32_t sf, uint32_t imm, uint32_t opc) {
    uint64_t result, a, b;
    switch (opc) {
        case 0b00: // Add
            result = machine.registers[rn] + imm;
        case 0b01: // Add and set flags
            a = machine.registers[rn];
            b = imm;
            result = a + b;

            machine.state.N = msb(result);
            machine.state.Z = result == 0;
            if (sf) { // 64 bit
                machine.state.C = a > MAX_INT_64 - b;
            } else { // 32 bit
                machine.state.C = a > MAX_INT_32 - b;
            }
            switch (sf) {
                case 0: // Rd is 32-bit
                    if ((machine.registers[rn] > 0 && imm > MAX_INT_32 - machine.registers[rn]) ||
                        (machine.registers[rn] < 0 && imm < MIN_INT_32 - machine.registers[rn])) {
                        machine.state.V = 1;
                        break;
                    }
                case 1: // Rd is 64-bit
                    if ((machine.registers[rn] > 0 && imm > MAX_INT_64 - machine.registers[rn]) ||
                        (machine.registers[rn] < 0 && imm < MIN_INT_64 - machine.registers[rn])) {
                        machine.state.V = 1;
                        break;
                    }
                default:
                    machine.state.V = 0;
            }
        case 0b10: // Subtract
            result = machine.registers[rn] - imm;
        case 0b11: // Subtract and set flags
            result = machine.registers[rn] - imm;
            machine.state.N = msb(result);
            machine.state.Z = result == 0;
            machine.state.C = machine.registers[rn] < imm;
            switch (sf) {
                case 0: // Rd is 32-bit
                    if ((machine.registers[rn] < 0 && imm > MAX_INT_32 + machine.registers[rn]) ||
                        (machine.registers[rn] > 0 && imm < MIN_INT_32 + machine.registers[rn])) {
                        machine.state.V = 1;
                        break;
                    }
                case 1: // Rd is 64-bit
                    if ((machine.registers[rn] < 0 && imm > MAX_INT_64 + machine.registers[rn]) ||
                        (machine.registers[rn] > 0 && imm < MIN_INT_64 + machine.registers[rn])) {
                        machine.state.V = 1;
                        break;
                    }
                default:
                    machine.state.V = 0;
            }
            machine.registers[rd] = result;
    }
}

void executeDPImmediate(struct Machine machine) {
    uint32_t sf, opc, opi, rd, sh, imm12, rn, hw, imm16;
    uint64_t lower_temp, upper_temp;
    sf = instructionPart(machine, 31, 1);
    opc = instructionPart(machine, 29, 2);
    opi = instructionPart(machine, 23, 3);
    rd = instructionPart(machine, 0, 5);
    sh = instructionPart(machine, 22, 1);
    rn = instructionPart(machine, 5, 5);
    hw = instructionPart(machine, 21, 2);
    imm12 = instructionPart(machine, 10, 12);
    imm16 = instructionPart(machine, 5, 16);

    switch (opi) {
        case 0b010: // Arithmetic instruction
            // Shift flag
            if (sh == 1) {
                imm12 <<= 12;
            }
            // Handle arithmetic operation
            executeArithmetic(machine, rd, rn, sf, imm12, opc);
            if (rn == 0b11111) {
                // Encode Stack Pointer
            }
            if (rd == 0b11111) {
                if (opc == 0b01 | opc == 0b11) {
                    // Encode Stack Pointer
                } else {
                    // Encode Zero Register
                }
            }
        case 0b101: // Wide move
            imm16 <<= (hw * 16);
            switch (opc) {
                case 0b00: // Move wide with NOT
                    machine.registers[rd] = ~imm16;
                case 0b10: // Move wide with Zero
                    machine.registers[rd] = imm16;
                case 0b11: // Move wide with keep
                    switch (hw) {
                        case 0:
                            lower_temp = 0;
                            upper_temp = (machine.registers[rd] >> 16) << 16;
                        case 1:
                            lower_temp = machine.registers[rd] & 0xFFFF;
                            upper_temp = (machine.registers[rd] >> 32) << 32;
                    }
                    if (sf == 0) {
                        upper_temp = (upper_temp << 32) >> 32;
                    }
                    machine.registers[rd] = lower_temp | imm16 | upper_temp;
            }
    }
}

uint32_t ror32(uint32_t operand, uint32_t shift) {
    uint32_t shifted, rotated;
    shifted = operand >> shift;
    rotated = operand << (32 - shift);
    return shifted | rotated;
}

uint32_t bitwiseShifts32(uint32_t operand, uint32_t shift) {
    switch (shift) {
        case 0b00: // lsl
            return operand << shift;
        case 0b01: // lsr
            return operand >> shift;
        case 0b10: // asr
            return operand >> shift;
        case 0b11: // ror
            return ror32(operand, shift);
    }
    return 0;
}

uint64_t ror64(uint64_t operand, uint64_t shift) {
    uint64_t shifted, rotated;
    shifted = operand >> shift;
    rotated = operand << (64 - shift);
    return shifted | rotated;
}

uint64_t bitwiseShifts64(uint64_t operand, uint64_t shift) {
    switch (shift) {
        case 0b00: // lsl
            return operand << shift;
        case 0b01: // lsr
            return operand >> shift;
        case 0b10: // asr
            return operand >> shift;
        case 0b11: // ror
            return ror64(operand, shift);
    }
    return 0;
}

void executeDPRegister(struct Machine machine) {
    uint32_t sf, opc, M, opr, rm, rn, rd, shift, imm6, bit24, N, x, ra, op2;
    uint64_t mul;
    sf = instructionPart(machine, 31, 1);
    opc = instructionPart(machine, 29, 2);
    M = instructionPart(machine, 28, 1);
    opr = instructionPart(machine, 21, 4);
    rm = instructionPart(machine, 16, 5);
    rn = instructionPart(machine, 5, 5);
    rd = instructionPart(machine, 0, 5);
    shift = instructionPart(machine, 22, 2);
    imm6 = instructionPart(machine, 10, 6);
    bit24 = instructionPart(machine, 24, 1);
    N = instructionPart(machine, 21, 1);
    ra = instructionPart(machine, 10, 5);
    x = instructionPart(machine, 15, 1);

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
        if (!(shift == 0b11 && bit24)) {
            if (sf) {
                op2 = bitwiseShifts64(rm, shift);
            } else {
                op2 = bitwiseShifts32(rm, shift);
            }
        }
        if (bit24) { // Arithmetic
            executeArithmetic(machine, rd, rn, sf, imm6, opc);
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
                    machine.state.N = msb(machine.registers[rd]);
                    machine.state.Z = msb(machine.registers[rd] == 0);
                    machine.state.C = 0;
                    machine.state.V = 0;
            }
        }
    }
}

void executeSingleDataTransfer(struct Machine machine) {
    uint32_t sf, U, L, xn, rt, xm, imm12, I, bit21, simm9, simm19, msb;
    sf = instructionPart(machine, 30, 1);
    U = instructionPart(machine, 24, 1);
    L = instructionPart(machine, 22, 1);
    xn = instructionPart(machine, 5, 5);
    rt = instructionPart(machine, 0, 5);
    xm = instructionPart(machine, 16, 5);
    imm12 = instructionPart(machine, 10, 12);
    I = instructionPart(machine, 11, 1);
    bit21 = instructionPart(machine, 21, 1);
    simm9 = instructionPartSigned(machine, 12, 9);
    simm19 = instructionPartSigned(machine, 5, 19);
    msb = instructionPart(machine, 31, 1);

    if (xn == STACK_POINTER) {
        // Stack pointer error
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
    uint32_t xn, cond, opc, jump, simm26, simm19;
    xn = instructionPart(machine, 5, 5);
    cond = instructionPart(machine, 0, 4);
    opc = instructionPart(machine, 30, 2);
    simm26 = instructionPartSigned(machine, 0, 26);
    simm19 = instructionPartSigned(machine, 5, 19);

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
        printf("%016lx", machine.registers[i]); // Pad with zeros to length 16
        printf("\n");
    }

    // PC
    printf("PC = ");
    printf("%016lx", machine.PC);
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
            printf("%08lx", value); // Pad with zeros to length 8
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
        uint32_t op0 = instructionPart(machine, 25, 4);

        // Execute
        if (machine.instruction == HALT_INSTRUCTION) {
            // Stop emulation
            break;
        } else if (machine.instruction == NOP_INSTRUCTION) {
            // Pass to next
        } else if (maskEquals(op0, 0b1110, 0b1000)) { // DP Immediate
            executeDPImmediate(machine);
        } else if (maskEquals(op0, 0b0111, 0b0101)) { // DP Register
            executeDPRegister(machine);
        } else if (maskEquals(op0, 0b0101, 0b0100)) { // Loads and Stores
            executeSingleDataTransfer(machine);
        } else if (maskEquals(op0, 0b1110, 0b1010)) { // Branch
            executeBranch(machine);
        } else {
            // Unknown opcode
        }
    }

    // Output result
    output(machine);
}

int main(int argc, char **argv) {
    //readFileIntoMemory(char *filename, struct Machine machine);
    //emulate(char *filename);
    return EXIT_SUCCESS;
}
