#include "arithmetic.c"

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
            break;
        case 0b101: // Wide move
            if (rd == ZERO_REGISTER) { // Cannot assign to the zero register
                return;
            }

            shift = hw * 16;
            op = imm16 << shift;
            switch (opc) {
                case 0b00: // Move wide with NOT
                    machine.registers[rd] = ~op;
                    break;
                case 0b10: // Move wide with zero
                    machine.registers[rd] = op;
                    break;
                case 0b11: // Move wide with keep
                    lower_rd = machine.registers[rd] & getMask(shift);
                    upper_rd = machine.registers[rd] & getMaskBetween(64, shift + 16);
                    machine.registers[rd] = lower_rd | op | upper_rd;
                    break;
                default: break;
            }

            // Ensure value within bit-width
            machine.registers[rd] &= getResultMask(sf);
            break;
        default: break;
    }
}
