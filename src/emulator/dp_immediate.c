/*
 * Execute DP-Immediate instructions
 */

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