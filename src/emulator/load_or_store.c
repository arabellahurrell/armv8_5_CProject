#include "arithmetic.c"

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
            address = machine.registers[xn] + imm12 * (sf ? 8 : 4);
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