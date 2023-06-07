#include "arithmetic.c"

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
        machine.registers[rd] = (machine.registers[ra] + mul) & getResultMask(sf);
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
                    result &= getResultMask(sf);

                    machine.state.N = getSignBit(result, sf);
                    machine.state.Z = result == 0;
                    machine.state.C = 0;
                    machine.state.V = 0;
                    break;
                default: break;
            }

            // Store calculated value
            if (rd != ZERO_REGISTER) { // Cannot assign to the zero register
                machine.registers[rd] = result & getResultMask(sf);
            }
        }
    }
}