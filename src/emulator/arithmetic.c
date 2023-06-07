#include "binary_functions.c"
#include "read_binary_file.c"
#include "emulate_shifts.c"

#define ZERO_REGISTER 0b11111

// Helper function to execute an arithmetic instruction
void executeArithmeticInstruction(uint64_t rd, uint64_t a, uint64_t b, uint64_t opc, uint64_t sf) {
    uint64_t result;
    a &= getResultMask(sf);
    b &= getResultMask(sf);

    // Calculate result
    switch (opc) {
        case 0b00: // Add
            result = a + b;
            break;
        case 0b01: // Add and set flags
            result = (a + b) & getResultMask(sf);

            machine.state.N = getSignBit(result, sf);
            machine.state.Z = result == 0;
            machine.state.C = a > getResultMask(sf) - b;
            // Overflow iff sign bits of a and b the same and result has opposite sign
            machine.state.V = (getSignBit(a, sf) == getSignBit(b, sf))
                              && (getSignBit(a, sf) != getSignBit(result, sf));
            break;
        case 0b10: // Subtract
            result = a - b;
            break;
        case 0b11: // Subtract and set flags
            result = (a - b) & getResultMask(sf);

            machine.state.N = getSignBit(result, sf);
            machine.state.Z = result == 0;
            machine.state.C = a >= b;
            // Overflow iff sign bits of a and b are different and the sign bit of result same as subtrahend
            machine.state.V = (getSignBit(a, sf) != getSignBit(b, sf))
                              && (getSignBit(b, sf) == getSignBit(result, sf));
            break;
        default: break;
    }

    // Store calculated value
    if (rd != ZERO_REGISTER) { // Cannot assign to the zero register
        machine.registers[rd] = result & getResultMask(sf);
    }
}
