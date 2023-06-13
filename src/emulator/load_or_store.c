/*
 * Execute Load or Store Instructions
 */

// Executes Load or Store Instruction
void executeLoadOrStore() {
    uint64_t rt, l, sf, isSDT, xn, i, simm9, isRegOffset, u, imm12,
            address, xm, simm19;
    // Extracts relevant parts of the instruction
    rt = getInstructionPart(0, 5);
    l = getInstructionPart(22, 1);
    sf = getInstructionPart(30, 1);
    isSDT = getInstructionPart(31, 1);

    // Determine address to load/store
    if (isSDT) { // Single data transfer
        // Extracts the relevant parts of the single data transfer instruction
        xn = getInstructionPart(5, 5);
        i = getInstructionPart(11, 1);
        simm9 = getInstructionPartSigned(12, 9);
        isRegOffset = getInstructionPart(21, 1);
        u = getInstructionPart(24, 1);

        if (u) { // Unsigned immediate offset
            // Extracts the immediate value for the unsigned offset
            imm12 = getInstructionPart(10, 12);

            address = getRegisterValue(xn, sf) + imm12 * getWordBytes(sf);
        } else if (isRegOffset) { // Register offset
            // Extracts the Xm register for the register offset
            xm = getInstructionPart(16, 5);

            address = getRegisterValue(xn, sf) + getRegisterValue(xm, sf);
        } else if (i) { // Pre-indexed
            address = getRegisterValue(xn, sf) + simm9;

            // Implement write-back
            setRegisterValue(xn, address, sf);
        } else { // Post-indexed
            address = getRegisterValue(xn, sf);

            // Implement write-back
            setRegisterValue(xn, address + simm9, sf);
        }
    } else { // Load literal
        // Extracts the signed offset for the load literal instruction
        simm19 = getInstructionPartSigned(5, 19);

        address = machine.PC + simm19 * WORD_BYTES;
    }

    // Execute
    if (!isSDT || l) { // Load
        setRegisterValue(rt, loadFromMemory(address, sf), sf);
    } else { // Store
        storeInMemory(getRegisterValue(rt, sf), address, sf);
    }
}