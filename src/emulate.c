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
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h> // Tests fail without this

#define WORD_BYTES 4
#define NO_REGISTERS 31
#define NO_BYTES_MEMORY 0x200000
#define NOP_INSTRUCTION 0xD503201F
#define HALT_INSTRUCTION 0x8A000000
#define ZERO_REGISTER 0b11111

/*
 * Structures
 */

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

/*
 * Global variables
 */

struct Machine machine;
struct Machine empty_machine = {0};

/*
 * Helper Functions
 */

// Returns mask of length `bits` set to 1
uint64_t getMask(uint64_t bits) {
    if (bits >= 64) {
        return -1;
    } else {
        return (1LL << bits) - 1;
    }
}

// Returns mask set to 1 from bit `left` - 1 to bit `right` (inclusive)
uint64_t getMaskBetween(uint64_t left, uint64_t right) {
    return getMask(left) ^ getMask(right);
}

// Negates the given `value`
uint64_t negate(uint64_t value) {
    return ~value + 1;
}

// Returns mask to be applied to the result of an operation
uint64_t getResultMask(bool sf) {
    return getMask(sf ? 64 : 32);
}

// Returns the given `bit` of the given `value`
bool getBit(uint64_t value, uint64_t bit) {
    return (value >> bit) & 1;
}

// Returns the most significant bit of the given value
bool getSignBit(uint64_t value, bool sf) {
    return getBit(value, sf ? 63 : 31);
}

// Returns whether the masked value is equal to the target value
bool isMaskEquals(uint64_t value, uint64_t mask, uint64_t equalTo) {
    return (value & mask) == equalTo;
}

// Returns the given portion of the given value
uint64_t getPart(uint64_t value, uint64_t littleEnd, uint64_t length) {
    return (value >> littleEnd) & getMask(length);
}

// Returns the given portion of the given `value` as a 64-bit signed value
uint64_t getSignedPart(uint64_t value, uint64_t littleEnd, uint64_t length) {
    uint64_t part = getPart(value, littleEnd, length);
    if (getBit(part, length - 1)) { // Signed
        return part | getMaskBetween(64, length);
    } else { // Not signed
        return part;
    }
}

// Returns the given portion of the current instruction
uint64_t getInstructionPart(uint64_t littleEnd, uint64_t length) {
    return getPart(machine.instruction, littleEnd, length);
}

// Returns the given signed portion of the current instruction
uint64_t getInstructionPartSigned(uint64_t littleEnd, uint64_t length) {
    return getSignedPart(machine.instruction, littleEnd, length);
}

// Fetches a word or double word from memory starting at `startIndex`
uint64_t loadFromMemory(uint64_t startIndex, bool sf) {
    uint64_t value = 0, byte;
    for (int i = 0; i < (sf ? 8 : 4); i++) {
        byte = machine.memory[startIndex + i];
        value |= byte << i * 8;
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

// Performs arithmetic shift right
uint64_t arithmeticShiftRight(uint64_t value, uint64_t shiftAmount, uint64_t sf) {
    uint64_t result;
    if (getSignBit(value, sf)) { // Signed
        result = (value >> shiftAmount) | getMaskBetween(64, (sf ? 64 : 32) - shiftAmount);
    } else { // Unsigned
        result = value >> shiftAmount;
    }
    return result & getResultMask(sf);
}

// Performs rotate right shift to the given value
uint64_t rotateRight(uint64_t value, uint64_t shiftAmount, uint64_t sf) {
    uint64_t shifted, rotated;
    shifted = value >> shiftAmount;
    rotated = value << ((sf ? 64 : 32) - shiftAmount);
    return (shifted | rotated) & getResultMask(sf);
}

// Applies the given shift to the given value
uint64_t applyShift(uint64_t value, uint64_t shiftType, uint64_t shiftAmount, uint64_t sf) {
    switch (shiftType) {
        case 0b00: // lsl
            return (value << shiftAmount) & getResultMask(sf);
        case 0b01: // lsr
            return (value >> shiftAmount) & getResultMask(sf);
        case 0b10: // asr
            return arithmeticShiftRight(value, shiftAmount, sf);
        case 0b11: // ror
            return rotateRight(value, shiftAmount, sf);
        default: return 0;
    }
}

/*
 * Execute functions
 */

// Helper function to execute an arithmetic instruction
void executeArithmeticInstruction(uint64_t rd, uint64_t a, uint64_t b, uint64_t opc, uint64_t sf) {
    uint64_t result;

    // Calculate result
    switch (opc) {
        case 0b00: // Add
            result = a + b;
            break;
        case 0b01: // Add and set flags
            result = a + b;

            machine.state.N = getSignBit(result, sf);
            machine.state.Z = result == 0;
            machine.state.C = a > getResultMask(sf) - b;
            // Overflow iff sign bits of a and b the same and result has opposite sign
            machine.state.V = (getSignBit(a, sf) == getSignBit(b, sf))
                           && (getSignBit(a, sf) != getSignBit(result, sf));
        case 0b10: // Subtract
            result = a - b;
            break;
        case 0b11: // Subtract and set flags
            result = a - b;

            machine.state.N = getSignBit(result, sf);
            machine.state.Z = result == 0;
            machine.state.C = a >= b;
            // Overflow iff sign bits of a and b are different and the sign bit of result same as subtrahend
            machine.state.V = (getSignBit(a, sf) != getSignBit(b, sf))
                           && (getSignBit(b, sf) == getSignBit(result, sf));
        default: break;
    }

    // Store calculated value
    if (rd != ZERO_REGISTER) { // Cannot assign to the zero register
        machine.registers[rd] = result & getResultMask(sf);
    }
}

// Executes DP-Immediate instruction
void executeDPImmediate() {
    uint64_t sf, opc, opi, rd, sh, imm12, rn, hw, imm16, op, shift, lower_rd, upper_rd;
    sf = getInstructionPart(31, 1);
    opc = getInstructionPart(29, 2);
    opi = getInstructionPart(23, 3);
    rd = getInstructionPart(0, 5);
    sh = getInstructionPart(22, 1);
    rn = getInstructionPart(5, 5);
    hw = getInstructionPart(21, 2);
    imm12 = getInstructionPart(10, 12);
    imm16 = getInstructionPart(5, 16);

    switch (opi) {
        case 0b010: // Arithmetic instruction
            // Handle shift flag
            if (sh) {
                imm12 <<= 12;
            }

            // Execute instruction
            executeArithmeticInstruction(rd, machine.registers[rn], imm12, opc, sf);
        case 0b101: // Wide move
            if (rd == ZERO_REGISTER) { // Cannot assign to the zero register
                return;
            }

            shift = hw * 16;
            op = imm16 << shift;
            switch (opc) {
                case 0b00: // Move wide with NOT
                    machine.registers[rd] = ~op;
                case 0b10: // Move wide with zero
                    machine.registers[rd] = op;
                case 0b11: // Move wide with keep
                    lower_rd = machine.registers[rd] & getMask(shift);
                    upper_rd = machine.registers[rd] & getMaskBetween(64, shift + 16);
                    machine.registers[rd] = lower_rd | op | upper_rd;
                default: break;
            }

            // Ensure value within bit-width
            machine.registers[rd] &= getResultMask(sf);
        default: break;
    }
}

// Executes DP-Register instruction
void executeDPRegister() {
    uint64_t sf, opc, M, rm, rn, rd, shiftType, imm6, isArithmetic, N, x, ra, op2, mul, result;
    sf = getInstructionPart(31, 1);
    opc = getInstructionPart(29, 2);
    M = getInstructionPart(28, 1);
    rm = getInstructionPart(16, 5);
    rn = getInstructionPart(5, 5);
    rd = getInstructionPart(0, 5);
    shiftType = getInstructionPart(22, 2);
    imm6 = getInstructionPart(10, 6);
    isArithmetic = getInstructionPart(24, 1);
    N = getInstructionPart(21, 1);
    ra = getInstructionPart(10, 5);
    x = getInstructionPart(15, 1);

    if (M) { // Multiply
        if (rd == ZERO_REGISTER) { // Cannot assign to the zero register
            return;
        }
        mul = machine.registers[rn] * machine.registers[rm] * (x ? -1 : 1);
        machine.registers[rd] = machine.registers[ra] + mul;
    } else { // Arithmetic or Logical
        // ror shift (0b11) not valid for arithmetic instructions todo: what to do if given 0b11 with arithmetic?
        op2 = applyShift(machine.registers[rm], shiftType, imm6, sf);

        if (isArithmetic) { // Arithmetic
            executeArithmeticInstruction(rd, machine.registers[rn], op2, opc, sf);
        } else { // Logical
            if (N) { // Bitwise negate op2
                op2 = ~op2;
            }
            switch (opc) {
                case 0b00: // AND
                    result = machine.registers[rn] & op2;
                    break;
                case 0b01: // OR
                    result = machine.registers[rn] | op2;
                    break;
                case 0b10: // XOR
                    result = machine.registers[rn] ^ op2;
                    break;
                case 0b11: // AND, and set flags
                    result = machine.registers[rn] & op2;

                    machine.state.N = getSignBit(result, sf);
                    machine.state.Z = result == 0;
                    machine.state.C = 0;
                    machine.state.V = 0;
                default: break;
            }

            // Store calculated value
            if (rd != ZERO_REGISTER) { // Cannot assign to the zero register
                machine.registers[rd] = result & getResultMask(sf);
            }
        }
    }
}

// Executes Load or Store Instruction
void executeLoadOrStore() {
    uint64_t sf, U, L, xn, rt, xm, imm12, I, isRegOffset, isSDT, simm9, simm19;
    sf = getInstructionPart(30, 1);
    U = getInstructionPart(24, 1);
    L = getInstructionPart(22, 1);
    xn = getInstructionPart(5, 5);
    rt = getInstructionPart(0, 5);
    xm = getInstructionPart(16, 5);
    imm12 = getInstructionPart(10, 12);
    I = getInstructionPart(11, 1);
    isRegOffset = getInstructionPart(21, 1);
    simm9 = getInstructionPartSigned(12, 9);
    simm19 = getInstructionPartSigned(5, 19);
    isSDT = getInstructionPart(31, 1);

    // Determine address to load/store
    uint64_t address;
    if (isSDT) { // Single data transfer
        if (U) { // Unsigned immediate offset
            address = machine.registers[xn] + imm12;
        } else if (isRegOffset) { // Register offset
            address = machine.registers[xn] + machine.registers[xm];
        } else if (I) { // Pre-indexed
            address = machine.registers[xn] + simm9;

            // Implement write-back
            if (xn != ZERO_REGISTER) {
                machine.registers[xn] = address;
            }
        } else { // Post-indexed
            address = machine.registers[xn];

            // Implement write-back
            if (xn != ZERO_REGISTER) {
                machine.registers[xn] = address + simm9;
            }
        }
    } else { // Load literal
        address = machine.PC + simm19 * WORD_BYTES;
    }

    // Execute
    if (!isSDT || L) { // Load
        if (rt != ZERO_REGISTER) { // Cannot write to ZR
            machine.registers[rt] = loadFromMemory(address, sf);
        }
    } else { // Store
        storeInMemory(machine.registers[rt], address, sf);
    }
}

// Executes Branch instruction
void executeBranch() {
    uint64_t xn, cond, opc, simm26, simm19;
    xn = getInstructionPart(5, 5);
    cond = getInstructionPart(0, 4);
    opc = getInstructionPart(30, 2);
    simm26 = getInstructionPartSigned(0, 26);
    simm19 = getInstructionPartSigned(5, 19);

    bool jump = true;
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
                    break;
                case 0b0001: // Not equal
                    jump = !machine.state.Z;
                    break;
                case 0b1010: // Signed greater or equal
                    jump = machine.state.N == machine.state.Z;
                    break;
                case 0b1011: // Signed less than
                    jump = machine.state.N != machine.state.Z;
                    break;
                case 0b1100: // Signed greater than
                    jump = !machine.state.N && machine.state.N == machine.state.V;
                    break;
                case 0b1101: // Signed less than or equal
                    jump = !(!machine.state.N && machine.state.N == machine.state.V);
                    break;
                case 0b1110: // Always
                    jump = true;
                default: break;
            }
            // Execute jump if condition satisfied
            if (jump) {
                machine.PC += simm19 * WORD_BYTES;
            } else {
                machine.PC += WORD_BYTES;
            }
        default: break;
    }
}

/*
 * Main Emulator functions
 */

// Emulator running given file
void emulate(char readFile[], char writeFile[]) {
    // Setup ARMv8 machine
    machine.state.Z = true;

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
        if (isMaskEquals(op0, 0b1110, 0b1010)) { // Branch
            executeBranch();
        } else {
            if (machine.instruction == HALT_INSTRUCTION) { // Stop emulation
                break;
            } else if (machine.instruction == NOP_INSTRUCTION) { // Pass to next instruction
            } else if (isMaskEquals(op0, 0b1110, 0b1000)) { // DP Immediate
                executeDPImmediate();
            } else if (isMaskEquals(op0, 0b0111, 0b0101)) { // DP Register
                executeDPRegister();
            } else if (isMaskEquals(op0, 0b0101, 0b0100)) { // Loads and Stores
                executeLoadOrStore();
            } else {
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

/*
 * Tests
 */

// Test helper functions
void testHelperFunctions() {
    printf("getMask\n");
    printf("%llx\n", getMask(3)); // 7
    printf("%llx\n", getMask(8)); // ff
    printf("%llx\n", getMask(32)); // 32/4 = 8fs
    printf("%llx\n", getMask(64)); // 16fs

    printf("negate\n");
    printf("%llx\n", negate(1)); // 16fs
    printf("%llx\n", negate(-1)); // 1
    printf("%llx\n", negate(16)); // 15 fs, 0
    printf("%llx\n", negate(-16)); // 10

    printf("getMaskBetween\n");
    printf("%llx\n", getMaskBetween(64, 0)); // 16fs
    printf("%llx\n", getMaskBetween(5, 1)); // 2+4+8+16 = 16+14 = 1e
    printf("%llx\n", getMaskBetween(16, 8)); // ff00
    printf("%llx\n", getMaskBetween(64, 32)); // 8 fs, 8 0s
    printf("%llx\n", getMaskBetween(0, 0)); // 0

    printf("getResultMask\n");
    printf("%llx\n", getResultMask(0)); // 8 fs
    printf("%llx\n", getResultMask(1)); // 16 fs

    printf("getBit\n");
    printf("%i\n", getBit(0b0010, 0)); // 0
    printf("%i\n", getBit(0b0010, 1)); // 1
    printf("%i\n", getBit(0b0010, 2)); // 0
    printf("%i\n", getBit(0b0010, 3)); // 0
    printf("%i\n", getBit(0b0010, 63)); // 0
    printf("%i\n", getBit(-1, 63)); // 1

    printf("getSignBit\n");
    printf("%i\n", getSignBit(1, 0)); // 0
    printf("%i\n", getSignBit(1, 1)); // 0
    printf("%i\n", getSignBit(-1, 0)); // 1
    printf("%i\n", getSignBit(-1, 1)); // 1

    printf("isMaskEquals\n");
    printf("%i\n", isMaskEquals(0b0011, 0b0001, 0b0001)); // 1
    printf("%i\n", isMaskEquals(0b0011, 0b1111, 0b0011)); // 1
    printf("%i\n", isMaskEquals(0b1001, 0b0101, 0b0101)); // 0
    printf("%i\n", isMaskEquals(0b0110, 0b0101, 0b0100)); // 1
    printf("%i\n", isMaskEquals(0b0110, 0b0101, 0b0101)); // 0
    printf("%i\n", isMaskEquals(0b0110, 0b0101, 0b0101)); // 0
    printf("%i\n", isMaskEquals(0b0110, 0b0001, 0b0101)); // 0

    printf("getSignedPart\n");
    printf("%llx\n", getSignedPart(0b0101, 0, 0)); // 0
    printf("%llx\n", getSignedPart(0b0100, 0, 1)); // 0
    printf("%llx\n", getSignedPart(0b0100, 0, 2)); // 0
    printf("%llx\n", getSignedPart(0b0100, 0, 3)); // -4
    printf("%llx\n", getSignedPart(0b0100, 0, 4)); // 4
    printf("%llx\n", getSignedPart(0xffff0000, 0, 16)); // 0
    printf("%llx\n", getSignedPart(0xffff0000, 0, 17)); // 12 fs, 0000

    printf("arithmeticShiftRight\n");
    printf("%llx\n", arithmeticShiftRight(0xf0000000, 4, 0)); // ff000000
    printf("%llx\n", arithmeticShiftRight(0xf0000000, 8, 0)); // fff00000
    printf("%llx\n", arithmeticShiftRight(0xf000000000000000, 4, 1)); // ff, 14 0s
    printf("%llx\n", arithmeticShiftRight(0xf000000000000000, 4, 0)); // 0
    printf("%llx\n", arithmeticShiftRight(0xf000000000000000, 8, 1)); // fff, 13 0s
    printf("%llx\n", arithmeticShiftRight(0xf000000000000000, 8, 0)); // 0
    printf("%llx\n", arithmeticShiftRight(0x0f000000, 4, 0)); // f00000
    printf("%llx\n", arithmeticShiftRight(0x0f000000, 8, 0)); // f0000
    printf("%llx\n", arithmeticShiftRight(0x0f00000000000000, 4, 1)); // f, 13 0s
    printf("%llx\n", arithmeticShiftRight(0x0f00000000000000, 4, 0)); // 0
    printf("%llx\n", arithmeticShiftRight(0x0f00000000000000, 8, 1)); // f, 12 0s
    printf("%llx\n", arithmeticShiftRight(0x0f00000000000000, 8, 0)); // 0

    printf("rotateRight\n");
    printf("%llx\n", rotateRight(0xf0000000, 4, 0)); // f000000
    printf("%llx\n", rotateRight(0xf0000000, 8, 0)); // f00000
    printf("%llx\n", rotateRight(0xf000000000000000, 4, 1)); // f, 14 0s
    printf("%llx\n", rotateRight(0xf000000000000000, 4, 0)); // 0
    printf("%llx\n", rotateRight(0xf000000000000000, 8, 1)); // f, 13 0s
    printf("%llx\n", rotateRight(0xf000000000000000, 8, 0)); // 0
    printf("%llx\n", rotateRight(0x0f00000f, 4, 0)); // f0f00000
    printf("%llx\n", rotateRight(0x0f00000f, 8, 0)); // f0f0000
    printf("%llx\n", rotateRight(0x0f0000000000000f, 4, 1)); // f0f, 13 0s
    printf("%llx\n", rotateRight(0x0f0000000000000f, 4, 0)); // f0000000
    printf("%llx\n", rotateRight(0x0f0000000000000f, 8, 1)); // f0f, 12 0s
    printf("%llx\n", rotateRight(0x0f0000000000000f, 8, 0)); // f000000

    machine.instruction = 0xffff00f0;

    printf("getInstructionPart\n");
    printf("%llx\n", getInstructionPart(0, 8)); // f0
    printf("%llx\n", getInstructionPart(0, 16)); // f0
    printf("%llx\n", getInstructionPart(8, 8)); // 0

    printf("getInstructionPartSigned\n");
    printf("%llx\n", getInstructionPartSigned(0, 8)); // 15fs 0
    printf("%llx\n", getInstructionPartSigned(0, 16)); // f0
    printf("%llx\n", getInstructionPartSigned(8, 8)); // 0

    machine.memory[0] = 0xff;
    machine.memory[1] = 0x00;
    machine.memory[2] = 0xff;
    machine.memory[3] = 0x00;
    machine.memory[4] = 0xff;
    machine.memory[5] = 0x00;
    machine.memory[6] = 0xff;
    machine.memory[7] = 0x00;

    printf("loadFromMemory\n");
    printf("%llx\n", loadFromMemory(0, 0)); // ff00ff
    printf("%llx\n", loadFromMemory(1, 0)); // ff00ff00
    printf("%llx\n", loadFromMemory(0, 1)); // ff00ff00ff00ff

    printf("storeInMemory\n");
    storeInMemory(0x00ff00ff, 0, 0);
    printf("%02x%02x%02x%02x\n", machine.memory[3], machine.memory[2], machine.memory[1], machine.memory[0]);
    storeInMemory(0x00ff00ffffff0000, 0, 0);
    printf("%02x%02x%02x%02x", machine.memory[7], machine.memory[6], machine.memory[5], machine.memory[4]);
    printf("%02x%02x%02x%02x\n", machine.memory[3], machine.memory[2], machine.memory[1], machine.memory[0]);
}

void testExecuteFunctions() {

}

int main(int argc, char *argv[]) {
//    testHelperFunctions();
    testExecuteFunctions();
//    if (argc == 3) {
//        emulate(argv[1], argv[2]);
//        return EXIT_SUCCESS;
//    } else {
//        printf("Invalid number of arguments\n");
//        return EXIT_FAILURE;
//    }
}