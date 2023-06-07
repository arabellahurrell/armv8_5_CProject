/*
 * Execute Load or Store Instructions
 */

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