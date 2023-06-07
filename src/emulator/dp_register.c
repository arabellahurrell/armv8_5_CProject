#include "arithmetic.c"

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