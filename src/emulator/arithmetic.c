#include "read_binary_file.c"
#include "emulate_shifts.c"

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