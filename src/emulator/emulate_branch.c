/*
 * Execute Branch instruction
 */

void executeBranch() {
    bool jump;
    uint64_t opc, simm26, xn, cond, simm19;
    // Extracts the encoding to decide the type of branch to execute
    opc = getInstructionPart(30, 2);

    switch (opc) {
        case 0b00: // Unconditional
            // Extracts the signed immediate for the unconditional branch
            simm26 = getInstructionPartSigned(0, 26);

            machine.PC += simm26 * WORD_BYTES;
            break;
        case 0b11: // Register
            // Extracts the encoding of the Xn register
            xn = getInstructionPart(5, 5);

            machine.PC = getRegisterValue(xn, 0);
            break;
        case 0b01: // Conditional
            // Extracts the encoding of the condition
            // and the signed immediate for the conditional branch
            cond = getInstructionPart(0, 4);
            simm19 = getInstructionPartSigned(5, 19);

            // Determine whether to jump to next instruction
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
                    jump = !machine.state.N && machine.state.N ==
                                               machine.state.V;
                    break;
                case 0b1101: // Signed less than or equal
                    jump = !(!machine.state.N && machine.state.N ==
                                                 machine.state.V);
                    break;
                case 0b1110: // Always
                    jump = true;
                    break;
                default:
                    break;
            }
            // Execute jump if condition satisfied
            if (jump) {
                machine.PC += simm19 * WORD_BYTES;
            } else {
                machine.PC += WORD_BYTES;
            }
            break;
        default:
            break;
    }
}