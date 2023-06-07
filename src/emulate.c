// todo
// More errors

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define WORD_BYTES       4
#define NO_REGISTERS     31
#define NO_BYTES_MEMORY  0x200000
#define NOP_INSTRUCTION  0xD503201F
#define HALT_INSTRUCTION 0x8A000000
#define ZERO_REGISTER    0b11111

/*
 * Models
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

/*
 * Global Variables
 */

// The single machine entity used across all execute functions
struct Machine machine;
// Used to reset the machine to all zero values
struct Machine empty_machine = {0};

// Resets the machine to the initial state
void resetMachine() {
    machine = empty_machine;
    machine.state.Z = true;
}

/*
 * Helper Functions
 */

// Returns a mask with trailing `length` bits set to 1
uint64_t getMask(uint64_t length) {
    if (length >= 64) {
        return -1; // Return all bits set
    } else {
        return (1LL << length) - 1;
    }
}

// Returns a mask with bits from `left` - 1 to `right` (inclusive) set to 1
uint64_t getMaskBetween(uint64_t left, uint64_t right) {
    return getMask(left) ^ getMask(right);
}

// Returns the number of bits of the bit-width `sf`
uint64_t getLength(bool sf) {
    return sf ? 64 : 32;
}

// Returns the number of bytes in a word with the bit-width `sf`
uint64_t getWordBytes(bool sf) {
    return sf ? 8 : 4;
}

// Returns a mask to be applied to the result of an operation using the given bit-width `sf`
uint64_t getResultMask(bool sf) {
    return getMask(getLength(sf));
}

// Returns true if the `bit` of `value` is set using little-endian interpretation
bool isSet(uint64_t value, uint64_t bit) {
    return (value >> bit) & 1;
}

// Returns true if the most significant bit of `value` is set using the bit-width `sf`
bool isSigned(uint64_t value, bool sf) {
    return isSet(value, getLength(sf) - 1);
}

// Returns true if `mask` applied to `value` equals `target` value
bool isMaskEquals(uint64_t value, uint64_t mask, uint64_t target) {
    return (value & mask) == target;
}

// Returns a portion of `value` starting at bit `littleEnd` of `length` bits
uint64_t getPart(uint64_t value, uint64_t littleEnd, uint64_t length) {
    return (value >> littleEnd) & getMask(length);
}

// Returns a portion of `value` starting at bit `littleEnd` of `length` bits with sign extension
uint64_t getPartSigned(uint64_t value, uint64_t littleEnd, uint64_t length) {
    uint64_t part = getPart(value, littleEnd, length);
    if (isSet(part, length - 1)) { // Signed
        return part | getMaskBetween(64, length); // Sign-extend part
    } else { // Not signed
        return part;
    }
}

// Returns a portion of the current instruction starting at bit `littleEnd` of `length` bits
uint64_t getInstructionPart(uint64_t littleEnd, uint64_t length) {
    return getPart(machine.instruction, littleEnd, length);
}

// Returns a portion of the current instruction starting at bit `littleEnd` of `length` bits with sign extension
uint64_t getInstructionPartSigned(uint64_t littleEnd, uint64_t length) {
    return getPartSigned(machine.instruction, littleEnd, length);
}

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

// Performs arithmetic shift right by `shiftAmount` on the `value` using the bit-width `sf`
uint64_t arithmeticShiftRight(uint64_t value, uint64_t shiftAmount, uint64_t sf) {
    uint64_t result, mask;
    if (isSigned(value, sf)) { // Signed
        mask = getMaskBetween(64, getLength(sf) - shiftAmount);
        result = (value >> shiftAmount) | mask;
    } else { // Unsigned
        result = value >> shiftAmount;
    }
    return result & getResultMask(sf);
}

// Performs rotate right by `shiftAmount` on the `value` using the bit-width `sf`
uint64_t rotateRight(uint64_t value, uint64_t shiftAmount, uint64_t sf) {
    uint64_t shifted, rotated;
    shifted = value >> shiftAmount;
    rotated = value << (getLength(sf) - shiftAmount);
    return (shifted | rotated) & getResultMask(sf);
}

// Applies the shift given by `shiftType` to `value` by `shiftAmount` using the bit-width `sf`
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
// rd  = destination register
// a   = first operand
// b   = second operand
// opc = type of arithmetic operation
// sf  = bit-width of the operation
void executeArithmeticInstruction(uint64_t rd, uint64_t a, uint64_t b, uint64_t opc, uint64_t sf) {
    uint64_t result;
    switch (opc) {
        case 0b00: // Add
            setRegisterValue(rd, a + b, sf);
            break;
        case 0b01: // Add and set flags
            result = (a + b) & getResultMask(sf);
            setRegisterValue(rd, result, sf);

            machine.state.N = isSigned(result, sf);
            machine.state.Z = result == 0;
            machine.state.C = a > getResultMask(sf) - b;
            // Overflow iff sign bits of a and b the same and result has opposite sign
            machine.state.V = (isSigned(a, sf) == isSigned(b, sf))
                           && (isSigned(a, sf) != isSigned(result, sf));
            break;
        case 0b10: // Subtract
            setRegisterValue(rd, a - b, sf);
            break;
        case 0b11: // Subtract and set flags
            result = (a - b) & getResultMask(sf);
            setRegisterValue(rd, result, sf);

            machine.state.N = isSigned(result, sf);
            machine.state.Z = result == 0;
            machine.state.C = a >= b;
            // Overflow iff sign bits of a and b are different and the sign bit of result same as subtrahend
            machine.state.V = (isSigned(a, sf) != isSigned(b, sf))
                           && (isSigned(b, sf) == isSigned(result, sf));
            break;
        default: break;
    }
}

// Executes DP-Immediate instruction
void executeDPImmediate() {
    uint64_t sf, opc, opi, rd, sh, imm12, rn, hw, imm16, shifted, shiftAmount, lower_rd, upper_rd, result;
    // Decode instruction and extract relevant parts
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
            executeArithmeticInstruction(rd, getRegisterValue(rn, sf), imm12, opc, sf);
            break;
        case 0b101: // Wide move
            shiftAmount = hw * 16;
            shifted = imm16 << shiftAmount;
            switch (opc) {
                case 0b00: // Move wide with NOT
                    setRegisterValue(rd, ~shifted, sf);
                    break;
                case 0b10: // Move wide with zero
                    setRegisterValue(rd, shifted, sf);
                    break;
                case 0b11: // Move wide with keep
                    lower_rd = getRegisterValue(rd, 1) & getMask(shiftAmount);
                    upper_rd = getRegisterValue(rd, 1) & getMaskBetween(64, shiftAmount + 16);
                    result = lower_rd | shifted | upper_rd;
                    setRegisterValue(rd, result, sf);
                    break;
                default: break;
            }
            break;
        default: break;
    }
}

// Executes DP-Register instruction
void executeDPRegister() {
    uint64_t sf, opc, M, rm, rn, rd, shiftType, imm6, isArithmetic, N, x, ra, op2, multiplied, result, rnValue;
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
        multiplied = getRegisterValue(rn, sf) * getRegisterValue(rm, sf);

        // Handle negate flag
        if (x) {
            multiplied *= -1;
        }

        setRegisterValue(rd, getRegisterValue(ra, sf) + multiplied, sf);
    } else { // Arithmetic or Logical
        // Apply shift to operand
        op2 = applyShift(getRegisterValue(rm, sf), shiftType, imm6, sf);

        if (isArithmetic) { // Arithmetic
            executeArithmeticInstruction(rd, getRegisterValue(rn, sf), op2, opc, sf);
        } else { // Logical
            // Handle bitwise negate flag
            if (N) {
                op2 = ~op2;
            }

            rnValue = getRegisterValue(rn, sf);
            switch (opc) {
                case 0b00: // AND
                    setRegisterValue(rd, rnValue & op2, sf);
                    break;
                case 0b01: // OR
                    setRegisterValue(rd, rnValue | op2, sf);
                    break;
                case 0b10: // XOR
                    setRegisterValue(rd, rnValue ^ op2, sf);
                    break;
                case 0b11: // AND, and set flags
                    result = rnValue & op2 & getResultMask(sf);
                    setRegisterValue(rd, result, sf);

                    machine.state.N = isSigned(result, sf);
                    machine.state.Z = result == 0;
                    machine.state.C = 0;
                    machine.state.V = 0;
                    break;
                default: break;
            }
        }
    }
}

// Executes Load or Store Instruction
void executeLoadOrStore() {
    uint64_t sf, U, L, xn, rt, xm, imm12, I, isRegOffset, isSDT, simm9, simm19, address;
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
    if (isSDT) { // Single data transfer
        if (U) { // Unsigned immediate offset
            address = getRegisterValue(xn, sf) + imm12 * getWordBytes(sf);
        } else if (isRegOffset) { // Register offset
            address = getRegisterValue(xn, sf) + getRegisterValue(xm, sf);
        } else if (I) { // Pre-indexed
            address = getRegisterValue(xn, sf) + simm9;

            // Implement write-back
            setRegisterValue(xn, address, sf);
        } else { // Post-indexed
            address = getRegisterValue(xn, sf);

            // Implement write-back
            setRegisterValue(xn, address + simm9, sf);
        }
    } else { // Load literal
        address = machine.PC + simm19 * WORD_BYTES;
    }

    // Execute
    if (!isSDT || L) { // Load
        setRegisterValue(rt, loadFromMemory(address, sf), sf);
    } else { // Store
        storeInMemory(getRegisterValue(rt, sf), address, sf);
    }
}

// Executes Branch instruction
void executeBranch() {
    bool jump;
    uint64_t xn, cond, opc, simm26, simm19;
    xn = getInstructionPart(5, 5);
    cond = getInstructionPart(0, 4);
    opc = getInstructionPart(30, 2);
    simm26 = getInstructionPartSigned(0, 26);
    simm19 = getInstructionPartSigned(5, 19);

    switch (opc) {
        case 0b00: // Unconditional
            machine.PC += simm26 * WORD_BYTES;
            break;
        case 0b11: // Register
            machine.PC = getRegisterValue(xn, 0);
            break;
        case 0b01: // Conditional
            // Determine whether to jump
            jump = true;
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
                    break;
                default: break;
            }
            // Execute jump if condition satisfied
            if (jump) {
                machine.PC += simm19 * WORD_BYTES;
            } else {
                machine.PC += WORD_BYTES;
            }
            break;
        default: break;
    }
}

/*
 * Main Emulator functions
 */

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