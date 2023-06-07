#include "binary_functions.c"

// Performs arithmetic shift right
uint64_t arithmeticShiftRight(uint64_t value, uint64_t shiftAmount, uint64_t sf) {
    uint64_t result;
    if (getSignBit(value, sf)) { // Signed
        result = (value >> shiftAmount) | getMaskBetween(64, (sf ? 64 : 32) - shiftAmount);
    } else { // Unsigned
        result = value >> shiftAmount;
    }
    return result & getResultMask(sf);
}

// Performs rotate right shift to the given value
uint64_t rotateRight(uint64_t value, uint64_t shiftAmount, uint64_t sf) {
    uint64_t shifted, rotated;
    shifted = value >> shiftAmount;
    rotated = value << ((sf ? 64 : 32) - shiftAmount);
    return (shifted | rotated) & getResultMask(sf);
}

// Applies the given shift to the given value
uint64_t applyShift(uint64_t value, uint64_t shiftType, uint64_t shiftAmount, uint64_t sf) {
    switch (shiftType) {
        case 0b00: // lsl
            return (value << shiftAmount) & getResultMask(sf);
        case 0b01: // lsr
            return (value >> shiftAmount) & getResultMask(sf);
        case 0b10: // asr
            return arithmeticShiftRight(value, shiftAmount, sf);
        case 0b11: // ror
            return rotateRight(value, shiftAmount, sf);
        default: return 0;
    }
}
