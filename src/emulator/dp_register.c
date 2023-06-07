/*
 * Execute DP Register instructions
 */

void executeDPRegister() {
    uint64_t rd, rn, rm, M, opc, sf, ra, x, multiplied, imm6, shiftType,
            isArithmetic, op2, N, rnValue, result;
    // Extracts and decodes relevant parts of the instruction
    rd = getInstructionPart(0, 5);
    rn = getInstructionPart(5, 5);
    rm = getInstructionPart(16, 5);
    M = getInstructionPart(28, 1);
    opc = getInstructionPart(29, 2);
    sf = getInstructionPart(31, 1);

    if (M) { // Multiply
        // Extracts and decodes the operand for multiply instruction
        ra = getInstructionPart(10, 5);
        x = getInstructionPart(15, 1);

        multiplied = getRegisterValue(rn, sf) * getRegisterValue(rm, sf);

        // Handle negate flag
        if (x) {
            multiplied *= -1;
        }

        setRegisterValue(rd, getRegisterValue(ra, sf) + multiplied, sf);
    } else { // Arithmetic or Logical
        // Extracts and decodes the relevant parts of the operand and the
        // opr for the arithmetic and logical instructions
        imm6 = getInstructionPart(10, 6);
        shiftType = getInstructionPart(22, 2);
        isArithmetic = getInstructionPart(24, 1);

        // Apply shift to operand
        op2 = applyShift(getRegisterValue(rm, sf), shiftType, imm6, sf);

        if (isArithmetic) { // Arithmetic
            executeArithmeticInstruction(rd, getRegisterValue(rn, sf), op2, opc,
                                         sf);
        } else { // Logical
            // Extracts the bit for when the shifted register
            // is to be bitwise negated
            N = getInstructionPart(21, 1);

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

                    // Setting PSTATE
                    machine.state.N = isSigned(result, sf);
                    machine.state.Z = result == 0;
                    machine.state.C = 0;
                    machine.state.V = 0;
                    break;
                default:
                    break;
            }
        }
    }
}